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

#pragma once

#include <qtac/CommandGroup.h>
#include <qtac/Map.h>
#include <qtac/List.h>
#include <qtac/PinEntry.h>
#include <qtac/PinID.h>
#include <qtac/Point.h>
#include <qtac/String.h>
#include <qtac/StringUtilities.h>
#include <qtac/AlpacaScript.h>

// Disable nlohmann versioned inline namespace so 'nlohmann::json' is unambiguous
#ifndef NLOHMANN_JSON_NAMESPACE_NO_VERSION
#  define NLOHMANN_JSON_NAMESPACE_NO_VERSION 1
#endif
#include <nlohmann/json.hpp>

#include <memory>
#include <vector>

#ifndef QTAC_JSON_T_DEFINED
#define QTAC_JSON_T_DEFINED
using json_t = nlohmann::json;
#endif

const PinID kMaxPSOCPlatformId{255};

struct PSOCPinData
{
    PSOCPinData() = default;
    PSOCPinData(const PSOCPinData&) = default;
    PSOCPinData(PinID pin)
    {
        _pin = pin;
        _hash = strHash(qtac::String::number(static_cast<uint64_t>(pin)));
    }

    void clear() { *this = PSOCPinData(); }

    PinID           _pin{0};
    HashType        _hash{0};
    bool            _enabled{true};
    qtac::String    _pinLabel;
    qtac::String    _pinTooltip;
    bool            _initialValue{false};
    int             _initializationPriority{0};
    bool            _inverted{false};
    qtac::String    _pinCommand;
    CommandGroups   _commandGroup{eUnknownCommandGroup};
    qtac::String    _classicAction;
    qtac::Point     _cellLocation{-1, -1};
    qtac::String    _tabName;
};

using PSOCPinEntries = qtac::Map<PinID, PSOCPinData>;
using PSOCPinList    = qtac::List<PSOCPinData>;

class _PSOCPlatformConfiguration
{
public:
    _PSOCPlatformConfiguration();
    _PSOCPlatformConfiguration(const _PSOCPlatformConfiguration&) = delete;
    _PSOCPlatformConfiguration& operator=(const _PSOCPlatformConfiguration&) = delete;
    ~_PSOCPlatformConfiguration() = default;

    Pins getPins();

    PSOCPinList getAllPins();
    PSOCPinList getActivePins();

    bool getPinEnableState(PinID pinId) const;
    void setPinEnableState(PinID pinId, bool newState);

    bool getInitialPinValue(PinID pinId) const;
    void setInitialPinValue(PinID pinId, bool newState);

    uint64_t getPinInitializationPriority(PinID pinId) const;
    void setPinInitializationPriority(PinID pinId, int priority);

    bool getPinInvertedState(PinID pinId) const;
    void setPinInvertedState(PinID pinId, bool newState);

    qtac::String getPinLabel(PinID pinId) const;
    void setPinLabel(PinID pinId, const qtac::String& pinLabel);

    qtac::String getPinTooltip(PinID pinId) const;
    void setPinTooltip(PinID pinId, const qtac::String& pinTooltip);

    qtac::String getPinCommand(PinID pinId) const;
    void setPinCommand(PinID pinId, const qtac::String& pinCommand);

    CommandGroups getPinGroup(PinID pinId) const;
    void setPinGroup(PinID pinId, CommandGroups commandGroup);

    qtac::String getClassicAction(PinID pinId) const;
    void setClassicAction(PinID pinId, const qtac::String& classicAction);

    qtac::String getTabName(PinID pinId) const;
    void setTabName(PinID pinId, const qtac::String& tabName);

    qtac::Point getPinCellLocation(PinID pinId) const;
    void setPinCellLocation(PinID pinId, const qtac::Point& cellLocation);

    void cascadeTabDelete(const qtac::String& tabName);
    void cascadeTabRename(const qtac::String& oldName, const qtac::String& newName);

    const qtac::ButtonEntries&   getButtons()   const { return _buttons;   }
    const qtac::VariableEntries& getVariables() const { return _variables; }
    const qtac::AlpacaScript&    getScript()    const { return _script;    }

    qtac::String modificationDate() const { return _modificationDate; }
    int          fileVersion()      const { return _fileVersion;      }

    void setVariableValue(const qtac::String& name, const qtac::Variant& value)
    {
        auto it = _variables.find(name);
        if (it != _variables.end()) it->second._defaultValue = value;
    }

    bool read(json_t& parentLevel);
    void write(json_t& parentLevel);

private:
    static void initialize();

    PSOCPinEntries _pinEntries;
    qtac::ButtonEntries   _buttons;
    qtac::VariableEntries _variables;
    qtac::AlpacaScript    _script;
    qtac::String          _modificationDate;
    int                   _fileVersion{0};

    static PSOCPinEntries _classicActions;
};

using PSOCPlatformConfiguration = std::shared_ptr<_PSOCPlatformConfiguration>;
