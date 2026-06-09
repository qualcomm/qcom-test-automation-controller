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

// Author: Biswajit Roy
// Qt-free reimplementation of TACPIC32CXDriveThread from qcommon-console.

#pragma once

#include <qtac/TACDriveThread.h>
#include <qtac/TACPIC32CXProtocol.h>
#include <qtac/SerialPort.h>
#include <qtac/SerialPortInfo.h>

#include <memory>

namespace qtac {

class TACPIC32CXDriveThread : public TACDriveThread
{
public:
    explicit TACPIC32CXDriveThread(HashType hash);
    ~TACPIC32CXDriveThread() override;

    TACPIC32CXDriveThread(const TACPIC32CXDriveThread&)            = delete;
    TACPIC32CXDriveThread& operator=(const TACPIC32CXDriveThread&) = delete;

    // --- DriveThread ---
    void run() override;

    // --- TACDriveThread interface ---
    void sendCommand(const qtac::ByteArray& command,
                     bool              console          = false,
                     ReceiveInterface* receiveInterface = nullptr,
                     bool              shouldStore      = true) override;

    void endTransaction(ReceiveInterface* receiveInterface = nullptr);

    void setPinState(uint16_t pin, bool state) override;
    void sendCommandSequence(CommandEntries& commandEntries) override;

    // PIC32CX does not use reset count.
    int  getResetCount()  override { return 0; }
    void clearResetCount() override {}

    // PIC32CX does not use I2C.
    void i2CReadRegister(uint32_t /*addr*/, uint32_t /*reg*/)                   override {}
    void i2CWriteRegister(uint32_t /*addr*/, uint32_t /*reg*/, uint32_t /*data*/) override {}

    void setName(const qtac::ByteArray& newName) override;

    // --- SendInterface ---
    uint32_t send(const std::string& sendMe, const Arguments& arguments,
                  bool console, ReceiveInterface* receiveInterface,
                  bool store = true) override;
    bool ready() override;

    // --- ReceiveInterface ---
    void receive(FramePackage& framePackage) override;

protected:
    bool openSerialDevice();

    void setupConnected() override;
    void setupDiscovery() override;

private:
    TACPIC32CXProtocol              _tacProtocol;
    bool                            _connected{false};
    SerialPortInfo                  _tacPortInfo;
    std::unique_ptr<SerialPort>     _serialPort;
    bool                            _readyRead{false};
    std::string                     _serialBuffer;

    bool readSerialData();

    void handleSetPin(FramePackage& framePackage);
    void handleVersionResponse(FramePackage& framePackage);
    void handleClearBuffer(FramePackage& framePackage);
};

} // namespace qtac
