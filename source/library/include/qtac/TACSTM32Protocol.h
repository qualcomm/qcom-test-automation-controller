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

// Qt-free port of TACSTM32Protocol from qcommon-console.

#pragma once

#include <qtac/ProtocolInterface.h>
#include <qtac/ReceiveInterface.h>
#include <qtac/TACSTM32Coder.h>

namespace qtac {

class TACDriveThread;

// -----------------------------------------------------------------------
// TACSTM32Protocol
//
// Write-only protocol for the STM32 HID (BugHopper V2) hardware.
// No response lines are accumulated — the device sends no framed replies.
// -----------------------------------------------------------------------
class TACSTM32Protocol : public ProtocolInterface, public ReceiveInterface
{
public:
    TACSTM32Protocol();
    ~TACSTM32Protocol() override;

    TACSTM32Protocol(const TACSTM32Protocol&)            = delete;
    TACSTM32Protocol& operator=(const TACSTM32Protocol&) = delete;

    void setTACDriveTrain(TACDriveThread* tacDriveTrain);

    // ProtocolInterface / SendInterface
    uint32_t sendCommand(const qtac::ByteArray& command,
                         const Arguments& arguments,
                         bool console,
                         ReceiveInterface* receiveInterface,
                         bool shouldStore);

    void endTransaction(ReceiveInterface* receiveInterface);

    // ReceiveInterface
    void receive(FramePackage& framePackage) override;

    // ProtocolInterface callbacks — no-ops for a write-only device
    void idle()                                              override;
    void frameComplete(const qtac::ByteArray& completedFrame) override;
    void badFrame(const qtac::ByteArray& completedFrame)      override;

private:
    TACDriveThread* _tacDriveTrain{nullptr};
};

} // namespace qtac
