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

#include <qtac/TACSTM32DriveThread.h>
#include <qtac/TACSTM32Command.h>
#include <qtac/AlpacaDevice.h>
#include <qtac/TACCommandHashes.h>
#include <qtac/STM32Device.h>

#include <thread>
#include <sstream>
#include <string>

static const qtac::ByteArray kTACSTM32DriveTrainName{"TAC STM32 Drive Train"};

namespace qtac {

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

TACSTM32DriveThread::TACSTM32DriveThread(HashType hash)
    : TACDriveThread(hash)
{
    _driveTrainName = kTACSTM32DriveTrainName;
    setProtocolInterface(&_tacProtocol);
    _tacProtocol.setTACDriveTrain(this);

    AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
    if (!alpacaDevice || !alpacaDevice->active())
        return;

    _stm32Device = static_cast<STM32Device*>(alpacaDevice.get());

    _portName    = alpacaDevice->portName();
    _platformID  = alpacaDevice->platformID();
    _hardwareType = alpacaDevice->debugBoardType();
    _name        = alpacaDevice->serialNumber();
    _description = "Arduino BugHopper V2 Board";
    _uuid        = "Bug Hopper V2 - No UUID";

    setSerialNumber(alpacaDevice->serialNumber().toStdString());
    setPortName(alpacaDevice->portName());

    onDeviceDisconnected.connect([alpacaDevice]() {
        alpacaDevice->onDeviceDisconnect();
    });
    onProgress.connect([alpacaDevice](uint8_t value, NotificationLevel /*level*/) {
        if (alpacaDevice->onProgress)
            alpacaDevice->onProgress(value, 1 /*eInfoNotification*/);
    });
}

TACSTM32DriveThread::~TACSTM32DriveThread() = default;

// -----------------------------------------------------------------------
// STM32 device open
// -----------------------------------------------------------------------

bool TACSTM32DriveThread::openSTM32Device()
{
    if (_stm32Device == nullptr)
    {
        writeLogLine("TACSTM32DriveThread::openSTM32Device _stm32Device is null");
        if (onErrorOnOpen) onErrorOnOpen(qtac::ByteArray("Arduino BugHopper V2 device open failed. Check the application log"));
        return false;
    }

    if (!_stm32Device->openTransport())
    {
        if (onErrorOnOpen) onErrorOnOpen(qtac::ByteArray("Arduino BugHopper V2 device open failed. Check the application log"));
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------
// Public command methods
// -----------------------------------------------------------------------

void TACSTM32DriveThread::setPinState(uint64_t pin, bool state)
{
    {
        TACSTM32Command cmd(this, this);
        cmd.setPinState(static_cast<uint16_t>(pin), state);
    }
    waitForCompletion();
}

void TACSTM32DriveThread::sendCommandSequence(CommandEntries& commandEntries)
{
    {
        TACSTM32Command cmd(this, this);

        for (const auto& entry : commandEntries)
        {
            switch (entry->_commandAction)
            {
            case _CommandEntry::eNotSet:
                break;
            case _CommandEntry::eSetPin:
                cmd.setPinState(static_cast<uint16_t>(entry->_pinID),
                                entry->_arguement.toBool());
                break;
            case _CommandEntry::eLog:
                cmd.addLogComment(qtac::ByteArray(entry->_arguement.toString().toStdString()));
                break;
            case _CommandEntry::eDelay:
                cmd.addDelay(entry->_arguement.toUInt());
                break;
            case _CommandEntry::eBaseCommand:
                break;
            }

            if (onProgress) onProgress(0 /*kProgressActive*/, NotificationLevel::Info);
        }
    }
    waitForCompletion();
}

int  TACSTM32DriveThread::getResetCount()   { return 0; }
void TACSTM32DriveThread::clearResetCount()  {}

void TACSTM32DriveThread::i2CReadRegister(uint32_t /*addr*/, uint32_t /*reg*/)   {}
void TACSTM32DriveThread::i2CWriteRegister(uint32_t /*addr*/, uint32_t /*reg*/, uint32_t /*data*/) {}

void TACSTM32DriveThread::setName(const qtac::ByteArray& /*newName*/)
{
    // BugHopper V2 does not support rename.
}

void TACSTM32DriveThread::sendCommand(const qtac::ByteArray& command,
                                       bool console,
                                       ReceiveInterface* receiveInterface,
                                       bool shouldStore)
{
    Arguments args;
    qtac::ByteArray decoded = decodeCommand(command, args);
    send(decoded, args, console, receiveInterface, shouldStore);
}

uint32_t TACSTM32DriveThread::send(const qtac::ByteArray& sendMe,
                                    const Arguments& arguments,
                                    bool console,
                                    ReceiveInterface* receiveInterface,
                                    bool store)
{
    return _tacProtocol.sendCommand(sendMe, arguments, console, receiveInterface, store);
}

bool TACSTM32DriveThread::ready()
{
    return _tacProtocol.queueSize() == 0;
}

// -----------------------------------------------------------------------
// receive — called back from the run() loop
// -----------------------------------------------------------------------

void TACSTM32DriveThread::receive(FramePackage& framePackage)
{
    if (framePackage->endTransaction)
    {
        clearWaitForCompletion();
        if (onTransactionEnded) onTransactionEnded();
        if (onProgress)         onProgress(100 /*kProgressMax*/, NotificationLevel::Info);
    }
    else if (framePackage->valid)
    {
        switch (framePackage->requestHash)
        {
        case kSetPinCommandHash:
            {
                uint64_t pin   = 0;
                bool     state = false;

                if (framePackage->arguments.size() >= 1)
                    state = framePackage->arguments.at(0).asBool();
                if (framePackage->arguments.size() >= 2)
                    pin = static_cast<uint64_t>(framePackage->arguments.at(1).asUInt32());

                framePackage->synonym = qtac::ByteArray("Set Pin ")
                                      + std::to_string(pin).c_str() + " "
                                      + (state ? "on" : "off");

                if (onPinStateChanged) onPinStateChanged(pin, state);
            }
            break;
        default:
            break;
        }
    }
    else
    {
        clearWaitForCompletion();
        if (!framePackage->lastError.isEmpty() && onErrorOnOpen)
            onErrorOnOpen(framePackage->lastError);
    }

    log(framePackage);
    _protocolInterface->clearPendingFrame();
}

// -----------------------------------------------------------------------
// run() — drive thread main loop
// -----------------------------------------------------------------------

void TACSTM32DriveThread::run()
{
    writeLogLine("TACSTM32DriveThread::run()");

    if (openSTM32Device())
    {
        writeLogLine("TACSTM32DriveThread::run() openSTM32Device() == true");
        if (onDeviceOpen) onDeviceOpen();
        startRunning();
    }

    if (weAreRunning())
    {
        if (onHardwareVersionUpdate) onHardwareVersionUpdate("0.0.1");
        if (onNameUpdate)            onNameUpdate(_name.toStdString());
        if (onUuidUpdate)            onUuidUpdate(_uuid.toStdString());
        if (onSerialNumUpdate)       onSerialNumUpdate(_serialNumber.toStdString());
        if (onPlatformIDUpdate)      onPlatformIDUpdate(static_cast<int>(_platformID));

        if (onDeviceStatusChange) onDeviceStatusChange("Starting");

        writeLogLine("setupConnected()");
        setupConnected();
    }
    else
    {
        writeLogLine("TACSTM32DriveThread::run() emit deviceDisconnected");
        if (onDeviceDisconnected) onDeviceDisconnected();
    }

    if (weAreRunning())
    {
        writeLogLine("TACSTM32DriveThread::run() starting loop");
        bool loopFinished = false;

        while (!loopFinished)
        {
            FramePackage fp = _protocolInterface->getNextFramePackage();

            if (fp)
            {
                if (fp->delayInMilliSeconds != 0 ||
                    !fp->comment.isEmpty()       ||
                    fp->endTransaction           ||
                    checkLocalStore(fp))
                {
                    receive(fp);
                }
                else if (_stm32Device != nullptr)
                {
                    if (_stm32Device->write(fp->codedRequest))
                    {
                        receive(fp);
                    }
                    else
                    {
                        std::ostringstream oss;
                        oss << "TACSTM32DriveThread::run() write failure for request "
                            << fp->request.toStdString();
                        writeLogLine(oss.str());

                        fp->valid     = false;
                        fp->lastError = "BugHopper V2 write failed";

                        receive(fp);
                    }
                }
                else
                {
                    writeLogLine("TACSTM32DriveThread::run() _stm32Device is null.");
                    stopRunning();
                }
            }
            else
            {
                if (!weAreRunning())
                {
                    writeLogLine("TACSTM32DriveThread::run() loopFinished == true");
                    loopFinished = true;
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            }
        }
    }

    writeLogLine("TACSTM32DriveThread::run() close()");
    if (_stm32Device != nullptr)
        _stm32Device->close();

    _connected = false;
    if (onDeviceDisconnected) onDeviceDisconnected();
}

// -----------------------------------------------------------------------
// Setup callbacks
// -----------------------------------------------------------------------

void TACSTM32DriveThread::setupConnected()
{
    if (!_connected)
    {
        _connected = true;
        if (onDeviceConnected) onDeviceConnected();
    }
}

void TACSTM32DriveThread::setupDiscovery()
{
    // Nothing to do for STM32.
}

} // namespace qtac
