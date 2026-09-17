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

// Qt-free port of TACSTM32DriveThread from qcommon-console.

#pragma once

#include <qtac/TACDriveThread.h>
#include <qtac/TACSTM32Protocol.h>

// Forward declaration (global scope — STM32Device is not in namespace qtac)
class STM32Device;

namespace qtac {

// -----------------------------------------------------------------------
// TACSTM32DriveThread
//
// Concrete drive thread for STM32 HID (BugHopper V2) hardware.
// The device is write-only: no responses are read back.
// run() loop:
//   1. Opens the HID transport via STM32Device::openTransport().
//   2. Fires initial status callbacks.
//   3. Dequeues FramePackages and writes them via STM32Device::write().
// -----------------------------------------------------------------------
class TACSTM32DriveThread : public TACDriveThread
{
public:
    explicit TACSTM32DriveThread(HashType hash);
    ~TACSTM32DriveThread() override;

    TACSTM32DriveThread(const TACSTM32DriveThread&)            = delete;
    TACSTM32DriveThread& operator=(const TACSTM32DriveThread&) = delete;

    // --- DriveThread ---
    void run() override;

    // --- TACDriveThread interface ---
    void sendCommand(const qtac::ByteArray& command,
                     bool              console          = false,
                     ReceiveInterface* receiveInterface = nullptr,
                     bool              shouldStore      = true) override;

    void setPinState(uint64_t pin, bool state)               override;
    void sendCommandSequence(CommandEntries& commandEntries)  override;

    int  getResetCount()   override;
    void clearResetCount() override;

    void i2CReadRegister(uint32_t addr, uint32_t reg)                  override;
    void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)  override;

    void setName(const qtac::ByteArray& newName) override;

    // --- SendInterface ---
    uint32_t send(const qtac::ByteArray& sendMe, const Arguments& arguments,
                  bool console, ReceiveInterface* receiveInterface,
                  bool store = true) override;
    bool ready() override;

    // --- ReceiveInterface ---
    void receive(FramePackage& framePackage) override;

protected:
    bool openSTM32Device();

    void setupConnected() override;
    void setupDiscovery() override;

private:
    TACSTM32Protocol  _tacProtocol;
    STM32Device*      _stm32Device{nullptr};
    bool              _connected{false};
};

} // namespace qtac
