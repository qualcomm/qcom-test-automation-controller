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

// Authors: Michael Simpson, Biswajit Roy
// Qt-free reimplementation of TACPSOCDriveThread from qcommon-console.
// QSerialPort  -> qtac::SerialPort
// Qt signals   -> std::function<> callbacks (inherited from TACDriveThread)

#pragma once

#include <qtac/TACDriveThread.h>
#include <qtac/TACPSOCProtocol.h>
#include <qtac/SerialPort.h>
#include <qtac/SerialPortInfo.h>

#include <memory>

namespace qtac {

class TACPSOCDriveThread : public TACDriveThread
{
public:
    explicit TACPSOCDriveThread(HashType hash);
    ~TACPSOCDriveThread() override;

    TACPSOCDriveThread(const TACPSOCDriveThread&)            = delete;
    TACPSOCDriveThread& operator=(const TACPSOCDriveThread&) = delete;

    qtac::String locked() { return {}; }

    // --- DriveThread ---
    void run() override;

    // --- TACDriveThread interface ---
    void sendCommand(const qtac::ByteArray& command,
                     bool              console          = false,
                     ReceiveInterface* receiveInterface = nullptr,
                     bool              shouldStore      = true) override;

    void endTransaction(ReceiveInterface* receiveInterface = nullptr);

    void setPinState(uint64_t pin, bool state) override;
    void sendCommandSequence(CommandEntries& commandEntries) override;

    int  getResetCount()  override;
    void clearResetCount() override;

    void i2CReadRegister(uint32_t addr, uint32_t reg) override;
    void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data) override;

    void setName(const qtac::ByteArray& newName) override;

    // --- SendInterface ---
    uint32_t send(const qtac::ByteArray& sendMe, const Arguments& arguments,
                  bool console, ReceiveInterface* receiveInterface,
                  bool store = true) override;
    bool ready() override;

    bool connected() const { return _connected; }

    // --- ReceiveInterface ---
    void receive(FramePackage& framePackage) override;

protected:
    bool openSerialDevice();

    void setupConnected() override;
    void setupDiscovery() override;

private:
    TACPSOCProtocol              _tacProtocol;
    bool                         _connected{false};
    SerialPortInfo               _tacPortInfo;
    std::unique_ptr<SerialPort>  _serialPort;
    bool                         _readyRead{false};
    int                          _versionRetryCount{1};

    bool readSerialData();

    void handleGetNameResponse(FramePackage& framePackage);
    void handleGetResetCount(FramePackage& framePackage);
    void handleI2CRead(FramePackage& framePackage);
    void handleI2CWrite(FramePackage& framePackage);
    void handleSetPin(FramePackage& framePackage);
    void handlePlatformID(FramePackage& framePackage);
    void handleSetName(FramePackage& framePackage);
    void handleUUIDResponse(FramePackage& framePackage);
    void handleVersionResponse(FramePackage& framePackage);
};

} // namespace qtac
