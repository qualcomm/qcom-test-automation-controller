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

#pragma once

#include <qtac/STM32PinData.h>
#include <qtac/AlpacaScript.h>
#include <qtac/PinEntry.h>
#include <qtac/PlatformID.h>

#include <memory>

// -----------------------------------------------------------------------
// _STM32PlatformConfiguration
//
// Holds the hardcoded pin configuration for the BugHopper V2 (STM32).
// Config is never read from a tcnf file — all data is hardcoded in
// initialize().
// -----------------------------------------------------------------------
class _STM32PlatformConfiguration
{
public:
    _STM32PlatformConfiguration();
    _STM32PlatformConfiguration(const _STM32PlatformConfiguration&) = delete;
    _STM32PlatformConfiguration& operator=(const _STM32PlatformConfiguration&) = delete;
    ~_STM32PlatformConfiguration() = default;

    void initialize();

    STM32PinData getPinData(PinID pin);

    STM32PinList getAllPins() const;
    STM32PinList getActivePins() const;

    Pins getPins();

    bool getPinEnableState(PinID pin) const;
    bool getPinInvertedState(PinID pin) const;

    qtac::String getPinLabel(PinID pin) const;
    qtac::String getPinTooltip(PinID pin) const;
    qtac::String getPinCommand(PinID pin) const;
    CommandGroups getPinGroup(PinID pin) const;
    qtac::String getTabName(PinID pin) const;
    qtac::Point  getPinCellLocation(PinID pin) const;

    const qtac::ButtonEntries&   getButtons()   const { return _buttons;   }
    const qtac::VariableEntries& getVariables() const { return _variables; }
    const qtac::AlpacaScript&    getScript()    const { return _script;    }

private:
    STM32PinEntries       _pinEntries;
    qtac::ButtonEntries   _buttons;
    qtac::VariableEntries _variables;
    qtac::AlpacaScript    _script;
};

using STM32PlatformConfiguration = std::shared_ptr<_STM32PlatformConfiguration>;
