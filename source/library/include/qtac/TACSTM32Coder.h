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

#pragma once

#include <qtac/FrameCoder.h>

#include <cstdint>

namespace qtac {

// -----------------------------------------------------------------------
// TACSTM32Coder
//
// Encoder for the STM32 HID (BugHopper V2) drive thread.
// decode() is a no-op — BugHopper V2 sends no responses.
// encode() builds a 33-byte HID report for the SetPin command.
// -----------------------------------------------------------------------
class TACSTM32Coder : public FrameCoder
{
public:
    TACSTM32Coder();
    ~TACSTM32Coder() override;

    TACSTM32Coder(const TACSTM32Coder&)            = delete;
    TACSTM32Coder& operator=(const TACSTM32Coder&) = delete;

    void            reset()                                                         override;
    void            decode(const qtac::ByteArray& decodeMe)                         override;
    qtac::ByteArray encode(const qtac::ByteArray& encodeMe, const Arguments& args)  override;

private:
    uint8_t _gpioState{0};
};

} // namespace qtac
