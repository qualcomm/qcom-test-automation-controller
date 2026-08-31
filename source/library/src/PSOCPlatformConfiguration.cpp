// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <qtac/PSOCPlatformConfiguration.h>
#include <qtac/StringUtilities.h>

#include <algorithm>
#include <stdexcept>

// JSON key constants
static const char* kPlatformEntries        = "pins";
static const char* kPinNumber              = "pin_number";
static const char* kEnabled                = "enabled";
static const char* kInitialValue           = "initial_value";
static const char* kPriority               = "initialization_priority";
static const char* kInverted               = "inverted";
static const char* kName                   = "name";
static const char* kToolTip                = "help_hint";
static const char* kButtonTooltip          = "tooltip";
static const char* kAuthor                 = "author";
static const char* kDescription           = "description";
static const char* kCommand                = "command";
static const char* kCommandGroup           = "command_group";
static const char* kClassicAction          = "classic_action";
static const char* kTabName                = "tab_name";
static const char* kRunPriority            = "run_priority";
static const char* kMinFirmwareVersion     = "supportedFirmwareVer";
static const char* kButtons                = "buttons";
static const char* kVariables              = "variables";
static const char* kScript                 = "script";
static const char* kModificationDate       = "modification_date";
static const char* kFileVersion            = "fileVersion";
static const char* kLabel                  = "label";
static const char* kTab                    = "tab";
static const char* kCellLocation           = "cellLocation";
static const char* kDefaultValue           = "default_value";
static const char* kType                   = "type";

// Static storage for factory defaults
PSOCPinEntries _PSOCPlatformConfiguration::_classicActions;

// ---------------------------------------------------------------------------

_PSOCPlatformConfiguration::_PSOCPlatformConfiguration()
{
    if (_classicActions.isEmpty())
        _PSOCPlatformConfiguration::initialize();

    for (const auto& kv : _classicActions)
        _pinEntries[kv.first] = kv.second;
}

// ---------------------------------------------------------------------------
// Pin list accessors
// ---------------------------------------------------------------------------

Pins _PSOCPlatformConfiguration::getPins()
{
    Pins result;
    PSOCPinList active = getActivePins();
    for (const auto& p : active)
    {
        PinEntry pe;
        pe._pin                    = p._pin;
        pe._enabled                = p._enabled;
        pe._hash                   = p._hash;
        pe._cellLocation           = p._cellLocation;
        pe._commandGroup           = p._commandGroup;
        pe._pinCommand             = p._pinCommand;
        pe._initialValue           = p._initialValue;
        pe._inverted               = p._inverted;
        pe._pinLabel               = p._pinLabel;
        pe._pinTooltip             = p._pinTooltip;
        pe._tabName                = p._tabName;
        pe._initializationPriority = p._initializationPriority;
        result.append(pe);
    }

    std::sort(result.begin(), result.end(), [](const PinEntry& a, const PinEntry& b) {
        if (a._tabName != b._tabName)       return a._tabName < b._tabName;
        if (a._commandGroup != b._commandGroup) return a._commandGroup < b._commandGroup;
        return a._pin < b._pin;
    });
    return result;
}

PSOCPinList _PSOCPlatformConfiguration::getAllPins()
{
    PSOCPinList result;
    for (const auto& kv : _pinEntries)
        result.append(kv.second);
    std::sort(result.begin(), result.end(), [](const PSOCPinData& a, const PSOCPinData& b) {
        return a._pin < b._pin;
    });
    return result;
}

PSOCPinList _PSOCPlatformConfiguration::getActivePins()
{
    PSOCPinList result;
    for (const auto& kv : _pinEntries)
        if (kv.second._enabled)
            result.append(kv.second);
    std::sort(result.begin(), result.end(), [](const PSOCPinData& a, const PSOCPinData& b) {
        return a._pin < b._pin;
    });
    return result;
}

// ---------------------------------------------------------------------------
// Getters / setters
// ---------------------------------------------------------------------------

bool _PSOCPlatformConfiguration::getPinEnableState(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._enabled : true;
}
void _PSOCPlatformConfiguration::setPinEnableState(PinID pinId, bool newState)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinEnableState: invalid pin");
    it->second._enabled = newState;
}

bool _PSOCPlatformConfiguration::getInitialPinValue(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._initialValue : false;
}
void _PSOCPlatformConfiguration::setInitialPinValue(PinID pinId, bool newState)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setInitialPinValue: invalid pin");
    it->second._initialValue = newState;
}

uint64_t _PSOCPlatformConfiguration::getPinInitializationPriority(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? static_cast<uint64_t>(it->second._initializationPriority) : 0;
}
void _PSOCPlatformConfiguration::setPinInitializationPriority(PinID pinId, int priority)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinInitializationPriority: invalid pin");
    it->second._initializationPriority = priority;
}

bool _PSOCPlatformConfiguration::getPinInvertedState(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._inverted : false;
}
void _PSOCPlatformConfiguration::setPinInvertedState(PinID pinId, bool newState)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinInvertedState: invalid pin");
    it->second._inverted = newState;
}

qtac::String _PSOCPlatformConfiguration::getPinLabel(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._pinLabel : qtac::String();
}
void _PSOCPlatformConfiguration::setPinLabel(PinID pinId, const qtac::String& v)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinLabel: invalid pin");
    it->second._pinLabel = v;
}

qtac::String _PSOCPlatformConfiguration::getPinTooltip(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._pinTooltip : qtac::String();
}
void _PSOCPlatformConfiguration::setPinTooltip(PinID pinId, const qtac::String& v)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinTooltip: invalid pin");
    it->second._pinTooltip = v;
}

qtac::String _PSOCPlatformConfiguration::getPinCommand(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._pinCommand : qtac::String();
}
void _PSOCPlatformConfiguration::setPinCommand(PinID pinId, const qtac::String& v)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinCommand: invalid pin");
    it->second._pinCommand = v;
}

CommandGroups _PSOCPlatformConfiguration::getPinGroup(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._commandGroup : eUnknownCommandGroup;
}
void _PSOCPlatformConfiguration::setPinGroup(PinID pinId, CommandGroups commandGroup)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinGroup: invalid pin");
    it->second._commandGroup = commandGroup;
}

qtac::String _PSOCPlatformConfiguration::getClassicAction(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._classicAction : qtac::String();
}
void _PSOCPlatformConfiguration::setClassicAction(PinID pinId, const qtac::String& v)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setClassicAction: invalid pin");
    it->second._classicAction = v;
}

qtac::String _PSOCPlatformConfiguration::getTabName(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._tabName : qtac::String();
}
void _PSOCPlatformConfiguration::setTabName(PinID pinId, const qtac::String& v)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setTabName: invalid pin");
    it->second._tabName = v;
}

qtac::Point _PSOCPlatformConfiguration::getPinCellLocation(PinID pinId) const
{
    auto it = _pinEntries.find(pinId);
    return (it != _pinEntries.end()) ? it->second._cellLocation : qtac::Point(-1, -1);
}
void _PSOCPlatformConfiguration::setPinCellLocation(PinID pinId, const qtac::Point& v)
{
    auto it = _pinEntries.find(pinId);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinCellLocation: invalid pin");
    it->second._cellLocation = v;
}

// ---------------------------------------------------------------------------
// Tab cascade operations
// ---------------------------------------------------------------------------

void _PSOCPlatformConfiguration::cascadeTabDelete(const qtac::String& tabName)
{
    for (auto& kv : _pinEntries)
    {
        if (kv.second._tabName == tabName)
            kv.second._tabName = qtac::String();
    }
}

void _PSOCPlatformConfiguration::cascadeTabRename(const qtac::String& oldName,
                                                   const qtac::String& newName)
{
    for (auto& kv : _pinEntries)
    {
        if (kv.second._tabName == oldName)
            kv.second._tabName = newName;
    }
}

// ---------------------------------------------------------------------------
// JSON I/O
// ---------------------------------------------------------------------------

bool _PSOCPlatformConfiguration::read(json_t& j)
{
    const auto* jobj = j.if_object();
    if (!jobj) return false;

    if (jobj->contains(kPlatformEntries) && jobj->at(kPlatformEntries).is_array())
    {
        for (const auto& peVal : jobj->at(kPlatformEntries).as_array())
        {
            const auto* pe = peVal.if_object();
            if (!pe) continue;
            PSOCPinData pinData;

            if (pe->contains(kPinNumber))
                pinData._pin = static_cast<PinID>(std::stoull(qtac::json_util::toString(pe->at(kPinNumber))));

            pinData._hash = strHash(qtac::String::number(static_cast<uint64_t>(pinData._pin)));

            if (pe->contains(kEnabled))       pinData._enabled                = qtac::json_util::toBool(pe->at(kEnabled));
            if (pe->contains(kInitialValue))  pinData._initialValue           = qtac::json_util::toBool(pe->at(kInitialValue));
            if (pe->contains(kPriority))      pinData._initializationPriority = qtac::json_util::toInt(pe->at(kPriority));
            if (pe->contains(kInverted))      pinData._inverted               = qtac::json_util::toBool(pe->at(kInverted));
            if (pe->contains(kName))          pinData._pinLabel               = qtac::json_util::toString(pe->at(kName));
            if (pe->contains(kToolTip))       pinData._pinTooltip             = qtac::json_util::toString(pe->at(kToolTip));
            if (pe->contains(kCommand))       pinData._pinCommand             = qtac::json_util::toString(pe->at(kCommand));
            if (pe->contains(kCommandGroup))  pinData._commandGroup           = static_cast<CommandGroups>(qtac::json_util::toInt(pe->at(kCommandGroup)));
            if (pe->contains(kClassicAction)) pinData._classicAction          = qtac::json_util::toString(pe->at(kClassicAction));
            if (pe->contains(kTabName))       pinData._tabName                = qtac::json_util::toString(pe->at(kTabName));
            if (pe->contains(kRunPriority))
                pinData._cellLocation = toPoint(qtac::String(qtac::json_util::toString(pe->at(kRunPriority))));

            _pinEntries[pinData._pin] = pinData;
        }
    }

    // --- Load buttons ---
    _buttons.clear();
    if (jobj->contains(kButtons) && jobj->at(kButtons).is_array())
    {
        for (const auto& jbVal : jobj->at(kButtons).as_array())
        {
            const auto* jb = jbVal.if_object();
            if (!jb) continue;
            qtac::ButtonEntry btn;
            if (jb->contains(kName))         btn._name         = qtac::json_util::toString(jb->at(kName));
            if (jb->contains(kCommand))      btn._command      = qtac::json_util::toString(jb->at(kCommand));
            if (jb->contains(kCommandGroup)) btn._commandGroup = qtac::json_util::toInt(jb->at(kCommandGroup));
            if (jb->contains(kTab))          btn._tab          = qtac::json_util::toString(jb->at(kTab));
            if (jb->contains(kButtonTooltip)) btn._tooltip     = qtac::json_util::toString(jb->at(kButtonTooltip));
            if (jb->contains(kCellLocation))
            {
                qtac::Point pt = toPoint(qtac::String(qtac::json_util::toString(jb->at(kCellLocation))));
                btn._cellX = pt.x();
                btn._cellY = pt.y();
            }
            if (!btn._command.isEmpty() && !btn._name.isEmpty())
                _buttons.push_back(btn);
        }
    }

    // If no buttons were defined in the tcnf, inject the classic default Quick Settings.
    if (_buttons.empty())
    {
        static const struct { const char* name; const char* cmd; const char* tip; int col; int row; }
        kDefaultButtons[] = {
            { "Power On",              "powerOn",            "Powers on the MTP/Device",                        0, 0 },
            { "Power Off",             "powerOff",           "Powers off the MTP/Device",                       1, 0 },
            { "Boot to EDL",           "bootToEDL",          "Boots the device to emergency download",          2, 0 },
            { "Boot to Fastboot",      "bootToFastboot",     "Boots the device to fastboot",                    0, 1 },
            { "Boot to UEFI",          "bootToUEFI",         "Boots the device to UEFI Menu",                   1, 1 },
            { "Boot to Secondary EDL", "bootToSecondaryEDL", "Boots the device to secondary emergency download",2, 1 },
        };
        for (const auto& d : kDefaultButtons)
        {
            qtac::ButtonEntry btn;
            btn._name         = d.name;
            btn._command      = d.cmd;
            btn._tooltip      = d.tip;
            btn._commandGroup = 4;  // eQuickSettingsGroup
            btn._tab          = "General";
            btn._cellX        = d.col;
            btn._cellY        = d.row;
            _buttons.push_back(btn);
        }
    }

    // --- Load variables ---
    _variables.clear();
    if (jobj->contains(kVariables) && jobj->at(kVariables).is_array())
    {
        for (const auto& jvVal : jobj->at(kVariables).as_array())
        {
            const auto* jv = jvVal.if_object();
            if (!jv) continue;
            qtac::VariableEntry var;
            if (jv->contains(kName))    var._name    = qtac::json_util::toString(jv->at(kName));
            if (jv->contains(kLabel))   var._label   = qtac::json_util::toString(jv->at(kLabel));
            if (jv->contains(kButtonTooltip)) var._tooltip = qtac::json_util::toString(jv->at(kButtonTooltip));
            if (jv->contains(kType))    var._type    = static_cast<qtac::VariableType>(qtac::json_util::toInt(jv->at(kType)));
            if (jv->contains(kDefaultValue))
            {
                std::string dv = qtac::json_util::toString(jv->at(kDefaultValue));
                if (var._type == qtac::VariableType::Boolean)
                    var._defaultValue = (dv == "1" || dv == "true");
                else if (var._type == qtac::VariableType::Float)
                {
                    try { var._defaultValue = std::stof(dv); } catch (...) {}
                }
                else
                {
                    try { var._defaultValue = static_cast<unsigned int>(std::stoul(dv)); }
                    catch (...) { var._defaultValue = 0u; }
                }
            }
            if (jv->contains(kCellLocation))
            {
                qtac::Point pt = toPoint(qtac::String(qtac::json_util::toString(jv->at(kCellLocation))));
                var._cellX = pt.x();
                var._cellY = pt.y();
            }
            if (!var._name.isEmpty())
                _variables[var._name] = var;
        }
    }

    // Inject defaults for script timing variables if not defined in the tcnf.
    auto ensureVar = [&](const char* name, const char* label, const char* tooltip,
                         unsigned int defaultMs, int cellX, int cellY)
    {
        if (_variables.find(qtac::String(name)) == _variables.end())
        {
            qtac::VariableEntry v;
            v._name         = name;
            v._label        = label;
            v._tooltip      = tooltip;
            v._type         = qtac::VariableType::Integer;
            v._defaultValue = defaultMs;
            v._cellX        = cellX;
            v._cellY        = cellY;
            _variables[v._name] = v;
        }
    };
    ensureVar("edl",      "EDL timing (ms)",     "Configurable Boot to EDL timing in milliseconds",      1300, 0, 0);
    ensureVar("uefi",     "UEFI timing (ms)",     "Configurable Boot to UEFI timing in milliseconds",     8000, 0, 1);
    ensureVar("fastboot", "Fastboot timing (ms)", "Configurable Boot to fastboot timing in milliseconds", 8000, 1, 0);

    // --- Load and parse script ---
    if (jobj->contains(kScript) && jobj->at(kScript).is_string())
    {
        qtac::String scriptText = qtac::json_util::toString(jobj->at(kScript));
        TACCommands cmds;
        PSOCPinList activePins = getActivePins();
        for (const auto& p : activePins)
        {
            if (p._pinCommand.isEmpty()) continue;
            TACCommand tc;
            tc._pin     = p._pin;
            tc._command = p._pinCommand;
            cmds.append(tc);
        }
        _script.parseScript(scriptText, _variables, cmds);
    }

    if (jobj->contains(kModificationDate) && jobj->at(kModificationDate).is_string())
        _modificationDate = qtac::String(qtac::json_util::toString(jobj->at(kModificationDate)));
    if (jobj->contains(kFileVersion) && (jobj->at(kFileVersion).is_int64() || jobj->at(kFileVersion).is_uint64()))
        _fileVersion = qtac::json_util::toInt(jobj->at(kFileVersion));
    if (jobj->contains(kName) && jobj->at(kName).is_string())
        _name = qtac::String(qtac::json_util::toString(jobj->at(kName)));
    if (jobj->contains(kAuthor) && jobj->at(kAuthor).is_string())
        _author = qtac::String(qtac::json_util::toString(jobj->at(kAuthor)));
    if (jobj->contains(kDescription) && jobj->at(kDescription).is_string())
        _description = qtac::String(qtac::json_util::toString(jobj->at(kDescription)));

    return true;
}

void _PSOCPlatformConfiguration::loadDefaultScript(const qtac::String& scriptText)
{
    if (scriptText.isEmpty() || !_script.isEmpty())
        return;

    TACCommands cmds;
    PSOCPinList activePins = getActivePins();
    for (const auto& p : activePins)
    {
        if (p._pinCommand.isEmpty()) continue;
        TACCommand tc;
        tc._pin     = p._pin;
        tc._command = p._pinCommand;
        cmds.append(tc);
    }
    _script.parseScript(scriptText, _variables, cmds);
}

void _PSOCPlatformConfiguration::write(json_t& j)
{
    boost::json::array pinsArray;
    for (const auto& kv : _pinEntries)
    {
        const PSOCPinData& p = kv.second;
        boost::json::object entry;
        entry[kPinNumber]     = qtac::String::number(static_cast<uint64_t>(p._pin)).toStdString();
        entry[kEnabled]       = p._enabled;
        entry[kInitialValue]  = p._initialValue;
        entry[kPriority]      = p._initializationPriority;
        entry[kInverted]      = p._inverted;
        entry[kName]          = p._pinLabel.toStdString();
        entry[kToolTip]       = p._pinTooltip.toStdString();
        entry[kCommand]       = p._pinCommand.toStdString();
        entry[kCommandGroup]  = static_cast<int>(p._commandGroup);
        entry[kClassicAction] = p._classicAction.toStdString();
        entry[kRunPriority]   = fromPoint(p._cellLocation).toStdString();
        entry[kTabName]       = p._tabName.toStdString();
        pinsArray.push_back(entry);
    }
    j.as_object()[kPlatformEntries] = pinsArray;
}

// ---------------------------------------------------------------------------
// Factory defaults (ported from qcommon-console/PSOCPlatformConfiguration.cpp)
// ---------------------------------------------------------------------------

void _PSOCPlatformConfiguration::initialize()
{
    auto add = [&](PinID pin,
                   const char* label,
                   const char* cmd,
                   const char* tooltip,
                   int priority,
                   CommandGroups group,
                   bool enabled,
                   const char* classic,
                   qtac::Point cell,
                   bool initialValue,
                   bool inverted,
                   const char* tab = "General")
    {
        PSOCPinData p;
        p._pin                    = pin;
        p._hash                   = strHash(qtac::String::number(static_cast<uint64_t>(pin)));
        p._pinLabel               = label ? label : "";
        p._pinCommand             = cmd ? cmd : "";
        p._pinTooltip             = tooltip ? tooltip : "";
        p._initializationPriority = priority;
        p._commandGroup           = group;
        p._enabled                = enabled;
        p._classicAction          = classic ? classic : "";
        p._cellLocation           = cell;
        p._initialValue           = initialValue;
        p._inverted               = inverted;
        p._tabName                = tab;
        _classicActions[pin]      = p;
    };

    add(53, "Battery",           "battery", "Battery Disconnect",                           2, eConnectionGroup, true,  "TAC_POWER_OFF",         {0,0}, false, true);
    add(55, "USB 0",             "usb0",    "Disconnnects VBUS for USB 0",                  2, eConnectionGroup, true,  "TAC_USB0_DIS",          {1,0}, false, true);
    add(36, "USB 1",             "usb1",    "Disconnects VBUS for USB1",                    2, eConnectionGroup, true,  "TAC_USB1_DIS",          {2,0}, false, true);
    add(50, "Power Key",         "pkey",    "Presses the power key button",                -1, eButtonGroup,     true,  "TAC_TC_START",          {0,0}, false, false);
    add(34, "Volume Up",         "volup",   "Presses the volume up button",                -1, eButtonGroup,     true,  "TAC_VOL_UP",            {0,1}, false, false);
    add(51, "Volume Down",       "voldn",   "Presses the volume down button",              -1, eButtonGroup,     true,  "TAC_RESET_PRI",         {0,2}, false, false);
    add(18, "Disconnect UIM 1",  "uim1",    "Disconnects UIM 1",                           -1, eSwitchGroup,     true,  "TAC_UIM1_DIS",          {0,0}, false, false);
    add(19, "Disconnect UIM 2",  "uim2",    "Disconnects UIM 2",                           -1, eSwitchGroup,     true,  "TAC_UIM2_DIS",          {1,0}, false, false);
    add(54, "Emergency Download Mode (EDL)", "pedl", "Enables Primary Emergency Download Mode", -1, eSwitchGroup, true, "TAC_SW_DWNLD_PRI",     {0,1}, false, false);
    add(37, "Force PS_HOLD High","pshold",  "Forces PSHold high",                          -1, eSwitchGroup,     true,  "TAC_FORCE_PS_HOLD",     {0,2}, false, false);
    add(47, "Disconnect SD Card","sdcard",  "Disconnects the SD card slot",                -1, eSwitchGroup,     true,  "TAC_SDCARD_DISC",       {0,3}, false, false);
    add(16, "Embedded USB Debug (EUD)", "eud", "Enabled the embedded USB debugger",        -1, eSwitchGroup,     true,  "TC_EUD_EARLY_BOOT_EN",  {0,4}, false, false);
    add(21, "Headset Disconnect","headset", "Disconnects the headset",                     -1, eSwitchGroup,     true,  "TAC_HEADSET_DIS",       {0,5}, false, false);
    add(30, "Secondary Emergency Download Mode (EDL)", "sedl", "Secondary EDL",           -1, eSwitchGroup,     true,  "TAC_FORCE_USB_BOOT_SEC",{0,0}, false, false, "Fusion");
    add(31, "Secondary PM_RESIN_N", "sresn","Secondary PM_RESIN_N",                        -1, eSwitchGroup,     true,  "TAC_RESET_SEC",         {0,1}, false, false, "Fusion");
    add(29, "",                  "",        "",                                            -1, eUnknownCommandGroup, false, "", {-1,-1}, false, false);
    add(38, "",                  "",        "",                                            -1, eUnknownCommandGroup, false, "", {-1,-1}, false, false);
    add(39, "",                  "",        "",                                            -1, eUnknownCommandGroup, false, "", {-1,-1}, false, false);
    add(46, "",                  "",        "",                                            -1, eUnknownCommandGroup, false, "", {-1,-1}, false, false);
    add(48, "",                  "",        "",                                            -1, eUnknownCommandGroup, false, "", {-1,-1}, false, false);
    add(49, "",                  "",        "",                                            -1, eUnknownCommandGroup, false, "", {-1,-1}, false, false);
    add(15, "",                  "",        "",                                            -1, eUnknownCommandGroup, false, "TAC_SOFT_RESET", {-1,-1}, false, false);
}
