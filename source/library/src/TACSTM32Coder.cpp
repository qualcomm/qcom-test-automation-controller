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

// Qt-free port of TACSTM32Coder from qcommon-console.

#include <qtac/TACSTM32Coder.h>
#include <qtac/TACCommandHashes.h>

namespace {
const uint8_t kCommandGpioSet{0x01};
const int     kReportPayloadSize{32};
} // namespace

namespace qtac {

TACSTM32Coder::TACSTM32Coder()  = default;
TACSTM32Coder::~TACSTM32Coder() = default;

void TACSTM32Coder::reset()
{
    FrameCoder::reset();
    _gpioState = 0;
}

void TACSTM32Coder::decode(const qtac::ByteArray& /*decodeMe*/)
{
    // BugHopper V2 has no framed protocol to decode responses from.
}

qtac::ByteArray TACSTM32Coder::encode(const qtac::ByteArray& encodeMe,
                                       const Arguments& arguments)
{
    qtac::ByteArray result;

    if (arrayHash(encodeMe) == kSetPinCommandHash)
    {
        if (arguments.size() >= 2)
        {
            const bool     state  = arguments.at(0).asBool();
            const uint16_t pin    = static_cast<uint16_t>(arguments.at(1).asUInt32());
            const uint8_t  pinBit = static_cast<uint8_t>(1u << pin);

            _gpioState = state ? (_gpioState | pinBit) : (_gpioState & ~pinBit);

            // 33-byte HID report: result[0] = 0 (report ID), [1] = cmd, [2] = state, [3] = pin
            result = qtac::ByteArray(kReportPayloadSize + 1, char(0));
            result[1] = static_cast<char>(kCommandGpioSet);
            result[2] = static_cast<char>(_gpioState);
            result[3] = static_cast<char>(pinBit);
        }
    }

    return result;
}

} // namespace qtac
