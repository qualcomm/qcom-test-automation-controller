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

// Author: Michael Simpson

#include <qtac/TACLiteCoder.h>
#include <qtac/TACCommandHashes.h>

#include <string>
#include <variant>

namespace qtac {

// Map design-schematic pin numbers to the zero-based bus index that
// _FTDIChipset::write() uses.
static uint32_t designToPin(uint32_t designPin)
{
    switch (designPin)
    {
    // Bus A
    case 16: return 0;  case 17: return 1;  case 18: return 2;  case 19: return 3;
    case 21: return 4;  case 22: return 5;  case 23: return 6;  case 24: return 7;
    // Bus B
    case 26: return 8;  case 27: return 9;  case 28: return 10; case 29: return 11;
    case 30: return 12; case 32: return 13; case 33: return 14; case 34: return 15;
    // Bus C
    case 38: return 16; case 39: return 17; case 40: return 18; case 41: return 19;
    case 43: return 20; case 44: return 21; case 45: return 22; case 46: return 23;
    // Bus D
    case 48: return 24; case 52: return 25; case 53: return 26; case 54: return 27;
    case 55: return 28; case 57: return 29; case 58: return 30; case 59: return 31;
    default: return 0xFFFFFFF;
    }
}

TACLiteCoder::TACLiteCoder()  = default;
TACLiteCoder::~TACLiteCoder() = default;

void TACLiteCoder::reset()
{
    FrameCoder::reset();
}

void TACLiteCoder::decode(const qtac::ByteArray& /*decodeMe*/)
{
    // FTDI hardware is write-only for pin control — nothing to decode.
}

qtac::ByteArray TACLiteCoder::encode(const qtac::ByteArray& encodeMe, const Arguments& arguments)
{
    // The only command with a meaningful encode is SetPin:
    // arguments[1] holds the pin number (uint32_t design-schematic value).
    // We convert it to the bus index string that the run() loop passes to
    // _FTDIChipset::write(pin, state).
    if (arrayHash(encodeMe) == kSetPinCommandHash)
    {
        if (arguments.size() >= 2)
        {
            const uint32_t pin = std::get<uint32_t>(arguments.at(1));
            return qtac::ByteArray(std::to_string(designToPin(pin)));
        }
    }
    return qtac::ByteArray("Invalid");
}

} // namespace qtac
