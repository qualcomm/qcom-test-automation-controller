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

#include <qtac/PIC32CXPlatformConfiguration.h>
#include <qtac/StringUtilities.h>

#include <algorithm>
#include <stdexcept>

// JSON key constants
static const char* kPlatformEntries = "pins";
static const char* kPinNumber       = "pin_number";
static const char* kEnabled         = "enabled";
static const char* kInverted        = "inverted";
static const char* kName            = "name";
static const char* kToolTip         = "help_hint";
static const char* kCommand         = "command";
static const char* kCommandGroup    = "command_group";
static const char* kTabName         = "tab_name";
static const char* kRunPriority     = "run_priority";

// ---------------------------------------------------------------------------

_PIC32CXPlatformConfiguration::_PIC32CXPlatformConfiguration()
{
    initialize();
}

// ---------------------------------------------------------------------------
// Pin list accessors
// ---------------------------------------------------------------------------

Pins _PIC32CXPlatformConfiguration::getPins()
{
    Pins result;
    PIC32CXPinList active = getActivePins();
    for (const auto& p : active)
    {
        PinEntry pe;
        pe._pin          = p._setPin;
        pe._enabled      = p._enabled;
        pe._hash         = p._hash;
        pe._cellLocation = p._cellLocation;
        pe._commandGroup = p._commandGroup;
        pe._pinCommand   = p._pinCommand;
        pe._inverted     = p._inverted;
        pe._pinLabel     = p._pinLabel;
        pe._pinTooltip   = p._pinTooltip;
        pe._tabName      = p._tabName;
        result.append(pe);
    }

    std::sort(result.begin(), result.end(), [](const PinEntry& a, const PinEntry& b) {
        if (a._tabName != b._tabName)           return a._tabName < b._tabName;
        if (a._commandGroup != b._commandGroup) return a._commandGroup < b._commandGroup;
        return a._pin < b._pin;
    });
    return result;
}

PIC32CXPinList _PIC32CXPlatformConfiguration::getAllPins()
{
    PIC32CXPinList result;
    for (const auto& kv : _pinEntries)
        result.append(kv.second);
    std::sort(result.begin(), result.end(), [](const PIC32CXPinData& a, const PIC32CXPinData& b) {
        return a._setPin < b._setPin;
    });
    return result;
}

PIC32CXPinList _PIC32CXPlatformConfiguration::getActivePins()
{
    PIC32CXPinList result;
    for (const auto& kv : _pinEntries)
        if (kv.second._enabled)
            result.append(kv.second);
    std::sort(result.begin(), result.end(), [](const PIC32CXPinData& a, const PIC32CXPinData& b) {
        return a._setPin < b._setPin;
    });
    return result;
}

// ---------------------------------------------------------------------------
// Getters / setters
// ---------------------------------------------------------------------------

bool _PIC32CXPlatformConfiguration::getPinEnableState(PinID pinId) const
{
    auto it = _pinEntries.find(PIC32CXPinData::makePIC32CXHash(pinId));
    return (it != _pinEntries.end()) ? it->second._enabled : true;
}
void _PIC32CXPlatformConfiguration::setPinEnableState(HashType hash, bool newState)
{
    auto it = _pinEntries.find(hash);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinEnableState: invalid pin");
    it->second._enabled = newState;
}

bool _PIC32CXPlatformConfiguration::getPinInvertedState(PinID pinId) const
{
    auto it = _pinEntries.find(PIC32CXPinData::makePIC32CXHash(pinId));
    return (it != _pinEntries.end()) ? it->second._inverted : false;
}
void _PIC32CXPlatformConfiguration::setPinInvertedState(HashType hash, bool newState)
{
    auto it = _pinEntries.find(hash);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinInvertedState: invalid pin");
    it->second._inverted = newState;
}

qtac::String _PIC32CXPlatformConfiguration::getPinLabel(PinID pinId) const
{
    auto it = _pinEntries.find(PIC32CXPinData::makePIC32CXHash(pinId));
    return (it != _pinEntries.end()) ? it->second._pinLabel : qtac::String();
}
void _PIC32CXPlatformConfiguration::setPinLabel(HashType hash, const qtac::String& v)
{
    auto it = _pinEntries.find(hash);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinLabel: invalid pin");
    it->second._pinLabel = v;
}

qtac::String _PIC32CXPlatformConfiguration::getPinTooltip(PinID pinId) const
{
    auto it = _pinEntries.find(PIC32CXPinData::makePIC32CXHash(pinId));
    return (it != _pinEntries.end()) ? it->second._pinTooltip : qtac::String();
}
void _PIC32CXPlatformConfiguration::setPinTooltip(HashType hash, const qtac::String& v)
{
    auto it = _pinEntries.find(hash);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinTooltip: invalid pin");
    it->second._pinTooltip = v;
}

qtac::String _PIC32CXPlatformConfiguration::getPinCommand(PinID pinId) const
{
    auto it = _pinEntries.find(PIC32CXPinData::makePIC32CXHash(pinId));
    return (it != _pinEntries.end()) ? it->second._pinCommand : qtac::String();
}
void _PIC32CXPlatformConfiguration::setPinCommand(HashType hash, const qtac::String& v)
{
    auto it = _pinEntries.find(hash);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinCommand: invalid pin");
    it->second._pinCommand = v;
}

CommandGroups _PIC32CXPlatformConfiguration::getPinGroup(PinID pinId) const
{
    auto it = _pinEntries.find(PIC32CXPinData::makePIC32CXHash(pinId));
    return (it != _pinEntries.end()) ? it->second._commandGroup : eUnknownCommandGroup;
}
void _PIC32CXPlatformConfiguration::setPinGroup(HashType hash, CommandGroups commandGroup)
{
    auto it = _pinEntries.find(hash);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinGroup: invalid pin");
    it->second._commandGroup = commandGroup;
}

qtac::String _PIC32CXPlatformConfiguration::getTabName(PinID pinId) const
{
    auto it = _pinEntries.find(PIC32CXPinData::makePIC32CXHash(pinId));
    return (it != _pinEntries.end()) ? it->second._tabName : qtac::String();
}
void _PIC32CXPlatformConfiguration::setTabName(HashType hash, const qtac::String& v)
{
    auto it = _pinEntries.find(hash);
    if (it == _pinEntries.end()) throw std::invalid_argument("setTabName: invalid pin");
    it->second._tabName = v;
}

qtac::Point _PIC32CXPlatformConfiguration::getPinCellLocation(PinID pinId) const
{
    auto it = _pinEntries.find(PIC32CXPinData::makePIC32CXHash(pinId));
    return (it != _pinEntries.end()) ? it->second._cellLocation : qtac::Point(-1, -1);
}
void _PIC32CXPlatformConfiguration::setPinCellLocation(HashType hash, const qtac::Point& v)
{
    auto it = _pinEntries.find(hash);
    if (it == _pinEntries.end()) throw std::invalid_argument("setPinCellLocation: invalid pin");
    it->second._cellLocation = v;
}

PinID _PIC32CXPlatformConfiguration::bitFromSetPin(PinID setPin)
{
    auto it = _pinEntries.find(PIC32CXPinData::makePIC32CXHash(setPin));
    return (it != _pinEntries.end()) ? it->second._setPin : static_cast<PinID>(-1);
}

// ---------------------------------------------------------------------------
// Tab cascade operations
// ---------------------------------------------------------------------------

void _PIC32CXPlatformConfiguration::cascadeTabDelete(const qtac::String& tabName)
{
    for (auto& pinEntry : _pinEntries)
    {
        if (pinEntry._tabName == tabName)
            pinEntry._tabName = qtac::String();
    }
}

void _PIC32CXPlatformConfiguration::cascadeTabRename(const qtac::String& oldName,
                                                      const qtac::String& newName)
{
    for (auto& pinEntry : _pinEntries)
    {
        if (pinEntry._tabName == oldName)
            pinEntry._tabName = newName;
    }
}

// ---------------------------------------------------------------------------
// JSON I/O
// ---------------------------------------------------------------------------

bool _PIC32CXPlatformConfiguration::read(json_t& j)
{
    if (j.contains(kPlatformEntries) && j[kPlatformEntries].is_array())
    {
        for (const auto& pe : j[kPlatformEntries])
        {
            PIC32CXPinData pinData;

            if (pe.contains(kPinNumber))
                pinData._setPin = static_cast<PinID>(pe[kPinNumber].get<int>());

            pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);

            if (pe.contains(kEnabled))      pinData._enabled      = pe[kEnabled].get<bool>();
            if (pe.contains(kInverted))     pinData._inverted     = pe[kInverted].get<bool>();
            if (pe.contains(kName))         pinData._pinLabel     = pe[kName].get<std::string>();
            if (pe.contains(kToolTip))      pinData._pinTooltip   = pe[kToolTip].get<std::string>();
            if (pe.contains(kCommand))      pinData._pinCommand   = pe[kCommand].get<std::string>();
            if (pe.contains(kCommandGroup)) pinData._commandGroup = static_cast<CommandGroups>(pe[kCommandGroup].get<int>());
            if (pe.contains(kTabName))      pinData._tabName      = pe[kTabName].get<std::string>();
            if (pe.contains(kRunPriority))
                pinData._cellLocation = toPoint(qtac::String(pe[kRunPriority].get<std::string>()));

            _pinEntries[pinData._hash] = pinData;
        }
    }
    return true;
}

void _PIC32CXPlatformConfiguration::write(json_t& j)
{
    json_t pinsArray = json_t::array();
    for (const auto& kv : _pinEntries)
    {
        const PIC32CXPinData& p = kv.second;
        json_t entry;
        entry[kPinNumber]    = static_cast<int>(p._setPin);
        entry[kEnabled]      = p._enabled;
        entry[kInverted]     = p._inverted;
        entry[kName]         = p._pinLabel.toStdString();
        entry[kToolTip]      = p._pinTooltip.toStdString();
        entry[kCommand]      = p._pinCommand.toStdString();
        entry[kCommandGroup] = static_cast<int>(p._commandGroup);
        entry[kRunPriority]  = fromPoint(p._cellLocation).toStdString();
        entry[kTabName]      = p._tabName.toStdString();
        pinsArray.push_back(entry);
    }
    j[kPlatformEntries] = pinsArray;
}

// ---------------------------------------------------------------------------
// Factory defaults (ported from qcommon-console/PIC32CXPlatformConfiguration.cpp)
// ---------------------------------------------------------------------------

void _PIC32CXPlatformConfiguration::initialize()
{
    auto add = [&](PinID pin,
                   bool enabled,
                   bool inverted,
                   const char* label,
                   const char* cmd,
                   const char* tooltip,
                   CommandGroups group,
                   const char* tab,
                   qtac::Point cell)
    {
        PIC32CXPinData p(pin);
        p._enabled      = enabled;
        p._inverted     = inverted;
        p._pinLabel     = label ? label : "";
        p._pinCommand   = cmd ? cmd : "";
        p._pinTooltip   = tooltip ? tooltip : "";
        p._commandGroup = group;
        p._tabName      = tab;
        p._cellLocation = cell;
        _pinEntries[p._hash] = p;
    };

    add(3,   false, false, "JMI2C.2",              "iic2",           "Debug pin",                        eConnectionGroup,    "General", {-1,-1});
    add(4,   true,  false, "Battery",              "battery",        "Disconnects power to the device",  eConnectionGroup,    "General", { 0, 0});
    add(6,   true,  false, "KK Power Enable",      "kkpwr",          "Enables KK Power",                 eButtonGroup,        "General", { 0, 0});
    add(7,   true,  false, "MD PS HOLD",           "pshold",         "MD PS HOLD",                       eButtonGroup,        "General", { 1, 0});
    add(10,  true,  false, "KK Resin N",           "sresn",          "KK Resin N",                       eButtonGroup,        "General", { 2, 0});
    add(11,  false, false, "PMS Power On",         "pmspwr",         "PMS Power On",                     eButtonGroup,        "General", {-1,-1});
    add(16,  true,  false, "UEFI",                 "uefi",           "UEFI",                             eButtonGroup,        "General", { 0, 2});
    add(20,  true,  false, "Power Off",            "pkey",           "Power Off",                        eButtonGroup,        "General", { 0, 1});
    add(21,  true,  false, "PCIE0 Attention",      "pcie0",          "PCIE 0 Attention",                 eSwitchGroup,        "General", { 0, 0});
    add(27,  true,  false, "PCIE1 Attention",      "pcie1",          "PCIE 1 Attention",                 eSwitchGroup,        "General", { 1, 0});
    add(101, true,  false, "PCIE2 Attention",      "pcie2",          "PCIE 2 Attention",                 eSwitchGroup,        "General", { 2, 0});
    add(102, true,  false, "PCIE3 Attention",      "pcie3",          "PCIE 3 Attention",                 eSwitchGroup,        "General", { 0, 1});
    add(103, false, false, "JMI2C.10",             "iic10",          "Debug pin",                        eSwitchGroup,        "General", {-1,-1});
    add(110, false, false, "Kratos Trigger",       "kratos",         "Kratos Trigger",                   eSwitchGroup,        "General", {-1,-1});
    add(111, false, false, "JMI2C.4",              "iic4",           "Debug pin",                        eSwitchGroup,        "General", {-1,-1});
    add(114, false, false, "QCC BLD Disconnect",   "qccbld",         "QCC BLD Disconnect",               eButtonGroup,        "General", {-1,-1});
    add(115, false, false, "BB BLD Disconnect",    "bbbld",          "BB BLD Disconnect",                eSwitchGroup,        "General", {-1,-1});
    add(116, true,  false, "Secondary Fastboot",   "sfastboot",      "Fastboot SS",                      eSwitchGroup,        "General", { 1, 1});
    add(117, false, false, "JMI2C.5",              "iic5",           "Debug pin",                        eSwitchGroup,        "General", {-1,-1});
    add(118, false, false, "PG QAM VR4P5",         "inputvr4p5",     "PG QAM VR4P5",                     eButtonGroup,        "General", {-1,-1});
    add(119, false, false, "PG QAM SAIL VR3P3",    "inputvr3p3",     "PG QAM SAIL VR3P3",                eButtonGroup,        "General", {-1,-1});
    add(120, true,  false, "Force MD PS HOLD",     "forcemdpshold",  "Force MD PS HOLD",                 eButtonGroup,        "General", { 1, 1});
    add(121, true,  false, "Force SS PS HOLD",     "forcesspshold",  "Force SS PS HOLD",                 eButtonGroup,        "General", { 2, 1});
    add(123, false, false, "",                     "",               "",                                 eUnknownCommandGroup,"General", {-1,-1});
    add(128, true,  false, "USB0",                 "usb0",           "Disconnect USB0",                  eConnectionGroup,    "General", { 1, 0});
    add(129, false, false, "PMS PGOOD",            "pgood",          "-",                                eButtonGroup,        "General", { 4, 0});
    add(204, false, false, "",                     "",               "",                                 eUnknownCommandGroup,"General", {-1,-1});
    add(205, true,  false, "USB2",                 "usb2",           "USB 2 Disable",                    eConnectionGroup,    "General", { 0, 1});
    add(206, false, false, "Fast Power Off",       "inputpwr",       "Fast K Power Off Enable",          eButtonGroup,        "General", { 4, 2});
    add(207, true,  false, "Mode 0",               "mode0",          "Mode 0 for Primary SOC",           eButtonGroup,        "General", { 1, 2});
    add(215, true,  false, "Mode 1",               "mode1",          "Mode 1",                           eButtonGroup,        "General", { 2, 2});
    add(216, true,  false, "MD EDL",               "pedl",           "MD EDL",                           eButtonGroup,        "General", { 0, 3});
    add(217, false, false, "Mode 0",               "mode0",          "Mode 0",                           eButtonGroup,        "General", {-1,-1});
    add(219, true,  false, "SS EDL",               "sedl",           "SS EDL",                           eButtonGroup,        "General", { 1, 3});
    add(221, false, false, "JMI2C.13",             "iic13",          "Debug pin",                        eButtonGroup,        "General", {-1,-1});
    add(224, false, false, "PMS Fast Power Off",   "inputpoff",      "PMS Fast Power Off",               eUnknownCommandGroup,"General", {-1,-1});
    add(225, false, false, "PMS Enable",           "inputpms",       "PMS Enable",                       eUnknownCommandGroup,"General", {-1,-1});
    add(226, false, false, "BB BLD EN",            "bbblden",        "-",                                eButtonGroup,        "General", {-1,-1});
    add(227, false, false, "QCC BLD EN",           "qccblden",       "-",                                eButtonGroup,        "General", {-1,-1});
    add(228, false, false, "Mode 2",               "mode2",          "Mode 2",                           eSwitchGroup,        "General", {-1,-1});
    add(311, true,  false, "EUD",                  "eud",            "EUD for Primary SOC",              eButtonGroup,        "General", { 2, 3});
    add(312, true,  false, "USB1",                 "usb1",           "USB1 Disconnect",                  eConnectionGroup,    "General", { 2, 0});
    add(320, false, false, "JMI2C.7",              "iic7",           "Debug I2C 7",                      eButtonGroup,        "General", {-1,-1});
    add(321, true,  false, "Fastboot MD",          "fastboot",       "MD Fastboot",                      eSwitchGroup,        "General", { 2, 1});
}
