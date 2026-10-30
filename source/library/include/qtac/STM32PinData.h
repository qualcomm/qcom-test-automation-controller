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

// Qt-free port of STM32PinData.h from qcommon-console.

#pragma once

#include <qtac/CommandGroup.h>
#include <qtac/Map.h>
#include <qtac/List.h>
#include <qtac/PinID.h>
#include <qtac/Point.h>
#include <qtac/String.h>
#include <qtac/StringUtilities.h>

const int kMaxSTM32GpioIndex{3};

struct STM32PinData
{
    STM32PinData() = default;
    STM32PinData(const STM32PinData&) = default;
    STM32PinData(PinID pin)
    {
        _hash = STM32PinData::makeSTM32Hash(pin);
        _pin  = pin;
    }

    void clear() { *this = STM32PinData(); }

    static HashType makeSTM32Hash(PinID gpioIndex)
    {
        return strHash(qtac::String::number(static_cast<unsigned long long>(gpioIndex)));
    }

    HashType        _hash{0};
    PinID           _pin{static_cast<PinID>(-1)};
    bool            _enabled{false};
    bool            _inverted{false};
    qtac::String    _pinLabel;
    qtac::String    _pinTooltip;
    qtac::String    _pinCommand;
    CommandGroups   _commandGroup{eUnknownCommandGroup};
    qtac::Point     _cellLocation{-1, -1};
    qtac::String    _tabName{"General"};
};

using STM32PinEntries = qtac::Map<HashType, STM32PinData>;
using STM32PinList    = qtac::List<STM32PinData>;
