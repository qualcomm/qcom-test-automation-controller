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

// Qt-free port of STM32PlatformConfiguration from qcommon-console.

#include <qtac/STM32PlatformConfiguration.h>
#include <qtac/StringUtilities.h>
#include <qtac/PlatformID.h>

#include <algorithm>

_STM32PlatformConfiguration::_STM32PlatformConfiguration()
{
    initialize();
}

void _STM32PlatformConfiguration::initialize()
{
    _pinEntries.clear();

    // --- Pin definitions — exact data from upstream STM32PlatformConfiguration.cpp ---
    auto addPin = [&](PinID pin, bool enabled, bool inverted,
                      const char* label, const char* cmd, const char* tooltip,
                      CommandGroups group, const char* tab, qtac::Point cell)
    {
        STM32PinData p(pin);
        p._enabled      = enabled;
        p._inverted     = inverted;
        p._pinLabel     = label;
        p._pinCommand   = cmd;
        p._pinTooltip   = tooltip;
        p._commandGroup = group;
        p._tabName      = tab;
        p._cellLocation = cell;
        _pinEntries[p._hash] = p;
    };

    addPin(0, true,  false, "EDL",         "edl",     "Boot mode select. High = USB / EDL boot, Low = Normal boot", eSwitchGroup,     "General", {0, 0});
    addPin(2, true,  true,  "Battery",     "battery", "Disable power supply",                                        eConnectionGroup, "General", {1, 0});
    addPin(3, true,  false, "Volume Down", "voldn",   "Volume down / USB-C host mode select button state",           eSwitchGroup,     "General", {2, 0});

    // --- Button definitions ---
    _buttons.clear();

    auto addButton = [&](const char* label, const char* cmd, const char* tooltip,
                         int col, int row)
    {
        qtac::ButtonEntry btn;
        btn._name         = label;
        btn._command      = cmd;
        btn._tooltip      = tooltip;
        btn._commandGroup = static_cast<int>(eQuickSettingsGroup);
        btn._tab          = "General";
        btn._cellX        = col;
        btn._cellY        = row;
        _buttons.push_back(btn);
    };

    addButton("Power On",              "powerOn",            "Power cycle to normal boot mode",            0, 0);
    addButton("Power Off",             "powerOff",           "Power off the device",                       1, 0);
    addButton("Reset",                 "reset",              "MPU reset pulse",                            2, 0);
    addButton("Boot to EDL",           "bootToEDL",          "Power cycle to USB / EDL boot mode",         0, 1);
    addButton("Force USB-C Host Mode", "forceUsbcHostMode",  "Force the device into USB-C host mode",      1, 1);

    // --- Script variables ---
    _variables.clear();
    {
        qtac::VariableEntry v;
        v._name         = "edl";
        v._label        = "EDL timing (ms)";
        v._tooltip      = "Configurable Boot to EDL timing in milliseconds";
        v._type         = qtac::VariableType::Integer;
        v._defaultValue = 100u;
        v._cellX        = 0;
        v._cellY        = 0;
        _variables[v._name] = v;
    }

    // --- AlpacaScript (exact text from upstream) ---
    static const char* kDefaultScript =
        "def powerOff()\n"
        "logComment ====== powerOff sequence start ======\n"
        "battery 1\n"
        "logComment ====== powerOff sequence finish ======\n\n"
        "def powerOn()\n"
        "logComment ====== powerOn sequence start ======\n"
        "powerOff\n"
        "battery 0\n"
        "edl 0\n"
        "voldn 0\n"
        "logComment ====== powerOn sequence finish ======\n\n"
        "def reset()\n"
        "logComment ====== reset sequence start ======\n"
        "edl 0\n"
        "voldn 0\n"
        "powerOff\n"
        "powerOn\n"
        "logComment ====== reset sequence finish ======\n\n"
        "def bootToEDL()\n"
        "logComment ====== bootToEDL sequence start ======\n"
        "powerOff\n"
        "edl 1\n"
        "delay $edl\n"
        "battery 0\n"
        "logComment ====== bootToEDL sequence finish ======\n\n"
        "def forceUsbcHostMode()\n"
        "logComment ====== forceUsbcHostMode sequence start ======\n"
        "powerOff\n"
        "edl 0\n"
        "voldn 1\n"
        "powerOn\n"
        "logComment ====== forceUsbcHostMode sequence finish ======";

    TACCommands cmds;
    STM32PinList active = getActivePins();
    for (const auto& p : active)
    {
        if (p._pinCommand.isEmpty()) continue;
        TACCommand tc;
        tc._pin     = p._pin;
        tc._command = p._pinCommand;
        cmds.append(tc);
    }
    _script.parseScript(qtac::String(kDefaultScript), _variables, cmds);
}

// -----------------------------------------------------------------------
// Pin data accessors
// -----------------------------------------------------------------------

STM32PinData _STM32PlatformConfiguration::getPinData(PinID pin)
{
    HashType hash = STM32PinData::makeSTM32Hash(pin);
    auto it = _pinEntries.find(hash);
    return (it != _pinEntries.end()) ? it->second : STM32PinData();
}

STM32PinList _STM32PlatformConfiguration::getAllPins() const
{
    STM32PinList result;
    for (const auto& kv : _pinEntries)
        result.append(kv.second);
    std::sort(result.begin(), result.end(), [](const STM32PinData& a, const STM32PinData& b) {
        return a._pin < b._pin;
    });
    return result;
}

STM32PinList _STM32PlatformConfiguration::getActivePins() const
{
    STM32PinList result;
    for (const auto& kv : _pinEntries)
        if (kv.second._enabled)
            result.append(kv.second);
    std::sort(result.begin(), result.end(), [](const STM32PinData& a, const STM32PinData& b) {
        return a._pin < b._pin;
    });
    return result;
}

Pins _STM32PlatformConfiguration::getPins()
{
    Pins result;
    STM32PinList active = getActivePins();
    for (const auto& p : active)
    {
        PinEntry pe;
        pe._pin          = p._pin;
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
        if (a._tabName != b._tabName)       return a._tabName < b._tabName;
        if (a._commandGroup != b._commandGroup) return a._commandGroup < b._commandGroup;
        return a._pin < b._pin;
    });
    return result;
}

bool _STM32PlatformConfiguration::getPinEnableState(PinID pin) const
{
    HashType h = STM32PinData::makeSTM32Hash(pin);
    auto it = _pinEntries.find(h);
    return (it != _pinEntries.end()) ? it->second._enabled : true;
}

bool _STM32PlatformConfiguration::getPinInvertedState(PinID pin) const
{
    HashType h = STM32PinData::makeSTM32Hash(pin);
    auto it = _pinEntries.find(h);
    return (it != _pinEntries.end()) ? it->second._inverted : false;
}

qtac::String _STM32PlatformConfiguration::getPinLabel(PinID pin) const
{
    HashType h = STM32PinData::makeSTM32Hash(pin);
    auto it = _pinEntries.find(h);
    return (it != _pinEntries.end()) ? it->second._pinLabel : qtac::String();
}

qtac::String _STM32PlatformConfiguration::getPinTooltip(PinID pin) const
{
    HashType h = STM32PinData::makeSTM32Hash(pin);
    auto it = _pinEntries.find(h);
    return (it != _pinEntries.end()) ? it->second._pinTooltip : qtac::String();
}

qtac::String _STM32PlatformConfiguration::getPinCommand(PinID pin) const
{
    HashType h = STM32PinData::makeSTM32Hash(pin);
    auto it = _pinEntries.find(h);
    return (it != _pinEntries.end()) ? it->second._pinCommand : qtac::String();
}

CommandGroups _STM32PlatformConfiguration::getPinGroup(PinID pin) const
{
    HashType h = STM32PinData::makeSTM32Hash(pin);
    auto it = _pinEntries.find(h);
    return (it != _pinEntries.end()) ? it->second._commandGroup : eUnknownCommandGroup;
}

qtac::String _STM32PlatformConfiguration::getTabName(PinID pin) const
{
    HashType h = STM32PinData::makeSTM32Hash(pin);
    auto it = _pinEntries.find(h);
    return (it != _pinEntries.end()) ? it->second._tabName : qtac::String();
}

qtac::Point _STM32PlatformConfiguration::getPinCellLocation(PinID pin) const
{
    HashType h = STM32PinData::makeSTM32Hash(pin);
    auto it = _pinEntries.find(h);
    return (it != _pinEntries.end()) ? it->second._cellLocation : qtac::Point(-1, -1);
}
