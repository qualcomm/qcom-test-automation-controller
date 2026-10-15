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
// Qt-free reimplementation of TACLiteCoder from qcommon-console.

#pragma once

#include <qtac/FrameCoder.h>

namespace qtac {

// -----------------------------------------------------------------------
// TACLiteCoder
//
// Encode/decode for the FTDI TAC Lite hardware.
// decode() is a no-op (the FTDI chipset is write-only for pin control).
// encode() translates a command + arguments into the pin number that the
// FTDIChipset::write() call expects.
// -----------------------------------------------------------------------
class TACLiteCoder : public FrameCoder
{
public:
    TACLiteCoder();
    ~TACLiteCoder() override;

    TACLiteCoder(const TACLiteCoder&)            = delete;
    TACLiteCoder& operator=(const TACLiteCoder&) = delete;

    void        reset()                                                  override;
    void            decode(const qtac::ByteArray& decodeMe)                                    override;
    qtac::ByteArray encode(const qtac::ByteArray& encodeMe, const Arguments& arguments) override;

private:
    qtac::ByteArray _receiveBuffer;
};

} // namespace qtac
