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

namespace qtac {

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
    // arguments[1] holds the bus index (0-31) computed by getSetPinIndex().
    // We write it as a string so the run() loop can parse it with stoi() and
    // pass it directly to _FTDIChipset::write(pin, state).
    if (arrayHash(encodeMe) == kSetPinCommandHash)
    {
        if (arguments.size() >= 2)
        {
            const uint32_t pin = arguments.at(1).asUInt32();
            return qtac::ByteArray(std::to_string(pin));
        }
    }
    return qtac::ByteArray("Invalid");
}

} // namespace qtac
