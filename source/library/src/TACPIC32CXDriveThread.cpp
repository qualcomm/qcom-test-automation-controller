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

#include <qtac/TACPIC32CXDriveThread.h>
#include <qtac/TACPIC32CXCommand.h>
#include <qtac/AlpacaDevice.h>
#include <qtac/TACCommandHashes.h>
#include <qtac/PlatformID.h>
#include <qtac/StringUtilities.h>

#include <cassert>
#include <sstream>
#include <variant>

static const std::string kTACPIC32CXDriveTrainName{"TAC PIC32CX Drive Train"};

namespace qtac {

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

TACPIC32CXDriveThread::TACPIC32CXDriveThread(HashType hash)
    : TACDriveThread(hash)
{
    _driveTrainName = kTACPIC32CXDriveTrainName;
    setProtocolInterface(&_tacProtocol);
    _tacProtocol.setTACDriveTrain(this);

    AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
    if (!alpacaDevice || !alpacaDevice->active())
        return;

    _hardwareType     = ePIC32CXAuto;
    _firmwareMajor    = 1;
    _firmwareMinor    = 1;
    _firmwareRevision = 1;
    _description      = qtac::String("PIC32CX (Automotive) Board");
    _uuid             = qtac::String("PIC32CX - No UUID");

    _portName     = alpacaDevice->portName();
    _serialNumber = qtac::String(alpacaDevice->serialNumber().toStdString());
    _description  = qtac::String(alpacaDevice->description().toStdString());

    onPinStateChanged = [alpacaDevice](uint64_t pin, bool state) {
        alpacaDevice->on_pinStateChanged(pin, state);
    };
    onProgress = [alpacaDevice](uint8_t value, NotificationLevel /*level*/) {
        if (alpacaDevice->onProgress)
            alpacaDevice->onProgress(value, 1 /*eInfoNotification*/);
    };
}

TACPIC32CXDriveThread::~TACPIC32CXDriveThread() = default;

// -----------------------------------------------------------------------
// openSerialDevice
// -----------------------------------------------------------------------

bool TACPIC32CXDriveThread::openSerialDevice()
{
    const std::string portNameStr = _portName.toStdString();
    for (const auto& info : SerialPortInfo::availablePorts())
    {
        if (info.portName().toStdString() == portNameStr ||
            info.serialNumber().toStdString() == portNameStr)
        {
            _tacPortInfo = info;
            break;
        }
    }

    if (_tacPortInfo.isNull())
    {
        writeLogLine("TACPIC32CXDriveThread::openSerialDevice: port not found for " + portNameStr);
        if (onErrorOnOpen)
            onErrorOnOpen(qtac::ByteArray("PIC32CX Device Open Failed. Check the Application Log"));
        return false;
    }

    SerialPortSettings settings;
    settings.baudRate = 115200;

    _serialPort = std::make_unique<SerialPort>(_tacPortInfo);
    _serialPort->setSettings(settings);

    _serialPort->onReadyRead = [this]() { _readyRead = true; };

    if (!_serialPort->open())
    {
        std::ostringstream oss;
        oss << "Unable to open TAC Port. " << _serialPort->errorString().toStdString();
        _lastErrorMessage = oss.str();
        writeLogLine(_lastErrorMessage);
        if (onErrorOnOpen)
            onErrorOnOpen(qtac::ByteArray("PIC32CX Device Open Failed. Check the Application Log"));
        _serialPort.reset();
        return false;
    }

    setSerialNumber(_tacPortInfo.serialNumber());
    setPortName(qtac::ByteArray(_tacPortInfo.portName().toStdString()));

    if (onSerialNumUpdate)
        onSerialNumUpdate(_serialNumber);

    {
        std::ostringstream oss;
        oss << "Device " << _tacPortInfo.serialNumber().toStdString() << " opened\n";
        oss << "Com port " << _tacPortInfo.portName().toStdString() << "\n";
        writeLogLine(oss.str());
    }

    // Clear any stale data before sending the first command.
    {
        TACPIC32CXCommand cmd(this, this);
        cmd.clearBuffer();
    }

    return true;
}

// -----------------------------------------------------------------------
// sendCommand
// -----------------------------------------------------------------------

void TACPIC32CXDriveThread::sendCommand(const qtac::ByteArray& command,
                                         bool              console,
                                         ReceiveInterface* receiveInterface,
                                         bool              shouldStore)
{
    Arguments args;
    qtac::ByteArray decoded = decodeCommand(command, args);
    send(decoded.toStdString(), args, console, receiveInterface, shouldStore);
}

void TACPIC32CXDriveThread::endTransaction(ReceiveInterface* receiveInterface)
{
    _tacProtocol.endTransaction(receiveInterface);
}

// -----------------------------------------------------------------------
// Public command methods
// -----------------------------------------------------------------------

void TACPIC32CXDriveThread::setPinState(uint16_t pin, bool state)
{
    {
        TACPIC32CXCommand cmd(this, this);
        cmd.setPinState(pin, state);
    }
    waitForCompletion();
}

void TACPIC32CXDriveThread::sendCommandSequence(CommandEntries& commandEntries)
{
    for (const auto& entry : commandEntries)
    {
        {
            TACPIC32CXCommand cmd(this, this);

            switch (entry->_commandAction)
            {
            case _CommandEntry::eNotSet:
                break;
            case _CommandEntry::eSetPin:
                cmd.setPinState(static_cast<uint16_t>(entry->_pinID),
                                entry->_arguement.toBool());
                break;
            case _CommandEntry::eLog:
                cmd.addLogComment(qtac::ByteArray(
                    entry->_arguement.toString().toStdString() + "\n"));
                break;
            case _CommandEntry::eDelay:
                cmd.addDelay(entry->_arguement.toUInt());
                break;
            case _CommandEntry::eBaseCommand:
                break;
            }
        }

        if (onProgress) onProgress(0 /*kProgressActive*/, NotificationLevel::Info);
    }
}

void TACPIC32CXDriveThread::setName(const qtac::ByteArray& newName)
{
    const std::string s = newName.toStdString();
    if (s.size() <= 32 && s != _name.toStdString() && isAlphaNumeric(newName))
        waitForCompletion();
}

// -----------------------------------------------------------------------
// SendInterface
// -----------------------------------------------------------------------

uint32_t TACPIC32CXDriveThread::send(const qtac::ByteArray& sendMe, const Arguments& arguments,
                                      bool console, ReceiveInterface* receiveInterface, bool store)
{
    return _tacProtocol.sendCommand(sendMe.toStdString(), arguments, console, receiveInterface, store);
}

bool TACPIC32CXDriveThread::ready()
{
    return _tacProtocol.queueSize() == 0;
}

// -----------------------------------------------------------------------
// readSerialData
// -----------------------------------------------------------------------

bool TACPIC32CXDriveThread::readSerialData()
{
    if (!_readyRead)
        return false;

    _readyRead = false;
    _serialBuffer.clear();

    // Drain all available bytes (mirrors original chunked readAll loop).
    while (true)
    {
        const qtac::ByteArray chunk = _serialPort->readAll(0 /*non-blocking*/);
        if (chunk.isEmpty())
            break;
        _serialBuffer += chunk;
    }

    if (_serialBuffer.isEmpty())
        return false;

    _protocolInterface->handleReceivedData(_serialBuffer);
    return true;
}

// -----------------------------------------------------------------------
// receive
// -----------------------------------------------------------------------

void TACPIC32CXDriveThread::receive(FramePackage& framePackage)
{
    if (framePackage->endTransaction)
    {
        clearWaitForCompletion();
        if (onTransactionEnded) onTransactionEnded();

        if (_protocolInterface->queueSize() == 0)
            if (onProgress) onProgress(100 /*kProgressMax*/, NotificationLevel::Info);
    }
    else if (framePackage->valid)
    {
        switch (framePackage->requestHash)
        {
        case kPIC32CXVersionCommandHash: handleVersionResponse(framePackage); break;
        case kPIC32CXSetPinCommandHash:  handleSetPin(framePackage);          break;
        case kPIC32CXClearBufferHash:    handleClearBuffer(framePackage);     break;
        default: break;
        }
    }
    else
    {
        clearWaitForCompletion();

        switch (framePackage->requestHash)
        {
        case kPIC32CXVersionCommandHash:
        {
            TACPIC32CXCommand cmd(this, this);
            cmd.platformID();
            break;
        }
        default:
            break;
        }
    }

    log(framePackage);
    _protocolInterface->clearPendingFrame();
}

// -----------------------------------------------------------------------
// run() — drive thread main loop
// -----------------------------------------------------------------------

void TACPIC32CXDriveThread::run()
{
    assert(_protocolInterface != nullptr);

    if (!openSerialDevice())
    {
        if (onErrorOnOpen) onErrorOnOpen(qtac::ByteArray(_lastErrorMessage));
        return;
    }

    if (onDeviceOpen) onDeviceOpen();

    startRunning();

    if (onDeviceStatusChange) onDeviceStatusChange(qtac::String("Starting"));

    _readyRead = false;
    _connected = false;

    {
        TACPIC32CXCommand cmd(this, this);
        cmd.platformID();
    }

    if (weAreRunning())
    {
        bool loopFinished = false;

        while (!loopFinished)
        {
            FramePackage framePackage = _protocolInterface->getNextFramePackage();

            if (framePackage)
            {
                if (framePackage->delayInMilliSeconds != 0 ||
                    !framePackage->comment.isEmpty()        ||
                    framePackage->endTransaction            ||
                    checkLocalStore(framePackage))
                {
                    receive(framePackage);
                }
                else
                {
                    writeLogLine("TACPIC32CXDriveTrain::run()::Write: " + framePackage->codedRequest.toStdString());

                    // Clear serial buffer before sending the version/identify command.
                    if (framePackage->requestHash == kPIC32CXVersionCommandHash)
                    {
                        if (_serialPort->clear())
                            writeLogLine("Buffer cleared before identifying PIC32CX board");
                    }

                    const int bytesWritten = _serialPort->write(framePackage->codedRequest);

                    if (bytesWritten == -1)
                    {
                        std::ostringstream oss;
                        oss << "TACPIC32CXDriveTrain::run()::bytesWritten == -1";
                        const std::string errStr = _serialPort->errorString().toStdString();
                        if (!errStr.empty())
                            oss << "\nError on write " << errStr;
                        writeLogLine(oss.str());
                        stopRunning();
                    }
                }
            }
            else
            {
                if (!weAreRunning())
                    loopFinished = true;
                else
                    _serialPort->waitForReadyRead(10);
            }

            if (readSerialData())
                _protocolInterface->idle();
        }
    }

    _serialPort->close();
    writeLogLine("TACPIC32CXDriveThread::run(): Serial port closed");

    _connected = false;
    if (onDeviceDisconnected) onDeviceDisconnected();
}

// -----------------------------------------------------------------------
// Frame handlers
// -----------------------------------------------------------------------

void TACPIC32CXDriveThread::handleSetPin(FramePackage& framePackage)
{
    if (framePackage->arguments.size() >= 2)
    {
        const bool     state = std::get<bool>(framePackage->arguments.at(0));
        const uint64_t pin   = static_cast<uint64_t>(
                                   std::get<uint32_t>(framePackage->arguments.at(1)));

        framePackage->synonym =
            qtac::ByteArray("Pin ") + std::to_string(pin).c_str() + " " + (state ? "on" : "off");

        if (onPinStateChanged) onPinStateChanged(pin, state);
    }
}

void TACPIC32CXDriveThread::handleVersionResponse(FramePackage& framePackage)
{
    if (!framePackage->responses.empty())
    {
        // Response format: name,firmware,?,mac,boardID,serial,mcn (comma-separated).
        const std::string raw = framePackage->responses.at(0).toStdString();

        std::vector<std::string> parts;
        {
            std::istringstream iss(raw);
            std::string tok;
            while (std::getline(iss, tok, ','))
                parts.push_back(tok);
        }

        if (parts.size() < 7)
        {
            writeLogLine("Incomplete board response: " + raw);
        }
        else
        {
            _name           = qtac::ByteArray(parts.at(0));
            writeLogLine("PIC32CX board name: " + parts.at(0));

            _firmwareString = qtac::ByteArray(parts.at(1));
            writeLogLine("PIC32CX board firmware version: " + parts.at(1));

            _macAddress     = qtac::ByteArray(parts.at(3));
            writeLogLine("PIC32CX board MAC: " + parts.at(3));

            try
            {
                _platformID = static_cast<PlatformID>(std::stoi(parts.at(4)));
            }
            catch (...)
            {
                _platformID = ALPACA_PIC32CX_ID;
            }
            writeLogLine("Identified platform id: " + std::to_string(static_cast<int>(_platformID)));

            _serialNumber   = qtac::String(parts.at(5));
            writeLogLine("PIC32CX board Serial Number: " + parts.at(5));

            _mcnNumber      = qtac::String(parts.at(6));
            writeLogLine("PIC32CX board MCN: " + parts.at(6));
        }
    }

    if (onPlatformIDUpdate) onPlatformIDUpdate(static_cast<int>(_platformID));
    setupConnected();
}

void TACPIC32CXDriveThread::handleClearBuffer(FramePackage& framePackage)
{
    if (!framePackage->responses.empty())
    {
        std::string joined;
        for (size_t i = 0; i < framePackage->responses.size(); ++i)
        {
            if (i > 0) joined += ',';
            joined += framePackage->responses[i].toStdString();
        }
        writeLogLine("Buffer cleared. Board response: '" + joined + "'");
    }
}

// -----------------------------------------------------------------------
// Setup callbacks
// -----------------------------------------------------------------------

void TACPIC32CXDriveThread::setupConnected()
{
    if (!_connected)
    {
        if (onHardwareVersionUpdate) onHardwareVersionUpdate(hardwareVersionString());

        _connected = true;

        {
            TACPIC32CXCommand cmd(this, this);
            cmd.platformID();
        }

        if (onDeviceConnected) onDeviceConnected();
    }
}

void TACPIC32CXDriveThread::setupDiscovery()
{
    if (onHardwareVersionUpdate) onHardwareVersionUpdate(hardwareVersionString());

    TACPIC32CXCommand cmd(this, this);
    cmd.platformID();
}

} // namespace qtac
