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

#include <qtac/TACPSOCDriveThread.h>
#include <qtac/TACPSOCCommand.h>
#include <qtac/AlpacaDevice.h>
#include <qtac/TACCommandHashes.h>
#include <qtac/PlatformID.h>
#include <qtac/StringUtilities.h>

#include <cassert>
#include <sstream>
#include <thread>
#include <variant>

static const std::string kTACSerialDriveTrainName{"TAC Serial Drive Train"};
static const std::string kHelpCommand{"Help"};

namespace qtac {

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

TACPSOCDriveThread::TACPSOCDriveThread(HashType hash)
    : TACDriveThread(hash)
{
    _driveTrainName = kTACSerialDriveTrainName;
    setProtocolInterface(&_tacProtocol);
    _tacProtocol.setTACDriveTrain(this);

    AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
    if (!alpacaDevice || !alpacaDevice->active())
        return;

    _portName     = alpacaDevice->portName();
    _serialNumber = qtac::String(alpacaDevice->serialNumber().toStdString());
    _description  = qtac::String(alpacaDevice->description().toStdString());

    // Wire callbacks so _AlpacaDevice is notified of pin changes and progress.
    onPinStateChanged = [alpacaDevice](uint64_t pin, bool state) {
        alpacaDevice->on_pinStateChanged(pin, state);
    };
    onProgress = [alpacaDevice](uint8_t value, NotificationLevel /*level*/) {
        if (alpacaDevice->onProgress)
            alpacaDevice->onProgress(value, 1 /*eInfoNotification*/);
    };
}

TACPSOCDriveThread::~TACPSOCDriveThread() = default;

// -----------------------------------------------------------------------
// openSerialDevice
// -----------------------------------------------------------------------

bool TACPSOCDriveThread::openSerialDevice()
{
    // Look up the port by name, then fall back to serial number.
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
        writeLogLine("TACPSOCDriveThread::openSerialDevice: port not found for " + portNameStr);
        if (onErrorOnOpen)
            onErrorOnOpen(qtac::ByteArray("PSOC Device Open Failed, Check the Application Log"));
        return false;
    }

    SerialPortSettings settings;
    settings.baudRate = 115200;

    _serialPort = std::make_unique<SerialPort>(_tacPortInfo);
    _serialPort->setSettings(settings);

    // Wire the readyRead callback so readSerialData() knows when data arrived.
    _serialPort->onReadyRead = [this]() { _readyRead = true; };

    if (!_serialPort->open())
    {
        std::ostringstream oss;
        oss << "Unable to open TAC Port. " << _serialPort->errorString().toStdString();
        writeLogLine(oss.str());
        if (onErrorOnOpen)
            onErrorOnOpen(qtac::ByteArray("PSOC Device Open Failed, Check the Application Log"));
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

    return true;
}

// -----------------------------------------------------------------------
// sendCommand
// -----------------------------------------------------------------------

void TACPSOCDriveThread::sendCommand(const qtac::ByteArray& command,
                                      bool              console,
                                      ReceiveInterface* receiveInterface,
                                      bool              shouldStore)
{
    if (command.toStdString().find(kHelpCommand) != 0)
    {
        Arguments args;
        qtac::ByteArray decoded = decodeCommand(command, args);
        send(decoded.toStdString(), args, console, receiveInterface, shouldStore);
    }
}

void TACPSOCDriveThread::endTransaction(ReceiveInterface* receiveInterface)
{
    _tacProtocol.endTransaction(receiveInterface);
}

// -----------------------------------------------------------------------
// Public command methods
// -----------------------------------------------------------------------

void TACPSOCDriveThread::setPinState(uint16_t pin, bool state)
{
    {
        TACPSOCCommand cmd(this, this);
        cmd.setPinState(pin, state);
    }
    waitForCompletion();
}

void TACPSOCDriveThread::sendCommandSequence(CommandEntries& commandEntries)
{
    TACPSOCCommand cmd(this, this);

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
            cmd.addLogComment(qtac::ByteArray(entry->_arguement.toString().toStdString() + "\n"));
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

int TACPSOCDriveThread::getResetCount()
{
    {
        TACPSOCCommand cmd(this, this);
        cmd.getResetCount();
    }
    waitForCompletion();
    return _resetCount;
}

void TACPSOCDriveThread::clearResetCount()
{
    {
        TACPSOCCommand cmd(this, this);
        cmd.clearResetCount();
    }
    waitForCompletion();
}

void TACPSOCDriveThread::i2CReadRegister(uint32_t addr, uint32_t reg)
{
    {
        TACPSOCCommand cmd(this, this);
        try   { cmd.i2CReadRegister(addr, reg); }
        catch (...) {}
    }
    waitForCompletion();
}

void TACPSOCDriveThread::i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)
{
    {
        TACPSOCCommand cmd(this, this);
        try   { cmd.i2CWriteRegister(addr, reg, data); }
        catch (...) {}
    }
    waitForCompletion();
}

void TACPSOCDriveThread::setName(const qtac::ByteArray& newName)
{
    const std::string s = newName.toStdString();
    if (s.size() <= 32 && s != _name.toStdString() && isAlphaNumeric(newName))
    {
        {
            TACPSOCCommand cmd(this, this);
            cmd.setName(newName);
        }
        waitForCompletion();
    }
}

// -----------------------------------------------------------------------
// SendInterface
// -----------------------------------------------------------------------

uint32_t TACPSOCDriveThread::send(const qtac::ByteArray& sendMe, const Arguments& arguments,
                                   bool console, ReceiveInterface* receiveInterface, bool store)
{
    return _tacProtocol.sendCommand(sendMe.toStdString(), arguments, console, receiveInterface, store);
}

bool TACPSOCDriveThread::ready()
{
    return _tacProtocol.queueSize() == 0;
}

// -----------------------------------------------------------------------
// readSerialData
// -----------------------------------------------------------------------

bool TACPSOCDriveThread::readSerialData()
{
    if (!_readyRead)
        return false;

    _readyRead = false;

    const qtac::ByteArray buffer = _serialPort->readAll();
    if (buffer.isEmpty())
        return false;

    _protocolInterface->handleReceivedData(buffer);
    return true;
}

// -----------------------------------------------------------------------
// receive — dispatches incoming frame packages
// -----------------------------------------------------------------------

void TACPSOCDriveThread::receive(FramePackage& framePackage)
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
        case kVersionCommandHash:           handleVersionResponse(framePackage); break;
        case kGetPlatformIDCommandHash:     handlePlatformID(framePackage);      break;
        case kGetUUIDCommandHash:           handleUUIDResponse(framePackage);    break;
        case kGetNameCommandHash:           handleGetNameResponse(framePackage); break;
        case kSetNameCommandHash:           handleSetName(framePackage);         break;
        case kClearResetCountCommandHash:
            if (onResetCountCleared) onResetCountCleared();
            break;
        case kI2CReadRegisterCommandHash:
        case kI2CReadRegisterValueCommandHash:
            handleI2CRead(framePackage);  break;
        case kI2CWriteRegisterCommandHash:  handleI2CWrite(framePackage);        break;
        case kSetPinCommandHash:            handleSetPin(framePackage);          break;
        default: break;
        }
    }
    else
    {
        // Invalid frame — don't hang on error.
        clearWaitForCompletion();

        switch (framePackage->requestHash)
        {
        case kGetPlatformIDCommandHash:
        {
            TACPSOCCommand cmd(this, this);
            cmd.version();
            break;
        }
        case kI2CReadRegisterCommandHash:
        case kI2CReadRegisterValueCommandHash:
            handleI2CRead(framePackage);
            break;
        case kI2CWriteRegisterCommandHash:
            handleI2CWrite(framePackage);
            break;
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

void TACPSOCDriveThread::run()
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
        TACPSOCCommand cmd(this, this);
        cmd.platformID();
        cmd.version();
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
                    writeLogLine("TACPSOCDriveThread::run()::Write: " + framePackage->codedRequest.toStdString());

                    const int bytesWritten = _serialPort->write(framePackage->codedRequest);

                    if (bytesWritten == -1)
                    {
                        std::ostringstream oss;
                        oss << "TACPSOCDriveThread::run()::bytesWritten == -1";
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
    writeLogLine("TACPSOCDriveThread::run(): Serial port closed");

    _connected = false;
    if (onDeviceDisconnected) onDeviceDisconnected();
}

// -----------------------------------------------------------------------
// Frame handlers
// -----------------------------------------------------------------------

void TACPSOCDriveThread::handleGetNameResponse(FramePackage& framePackage)
{
    if (framePackage->responses.size() > 1)
    {
        _name = framePackage->responses.at(1);
        if (onNameUpdate) onNameUpdate(qtac::String(_name.toStdString()));
    }
}

void TACPSOCDriveThread::handleGetResetCount(FramePackage& framePackage)
{
    if (framePackage->responses.size() >= 3)
    {
        if (framePackage->responses.at(0) == "getresetcount" &&
            framePackage->responses.at(2) == "ok")
        {
            try { _resetCount = static_cast<int>(std::stoul(framePackage->responses.at(1).toStdString())); }
            catch (...) {}
        }
    }
    if (onResetCountUpdate) onResetCountUpdate(static_cast<uint32_t>(_resetCount));
}

void TACPSOCDriveThread::handleI2CRead(FramePackage& framePackage)
{
    if (framePackage->requestHash == kI2CReadRegisterValueCommandHash)
    {
        if (framePackage->valid)
        {
            // Pop the trailing "ok" line if present.
            auto responses = framePackage->responses;
            if (!responses.empty())
                responses.pop_back();

            const qtac::ByteArray response =
                responses.empty() ? framePackage->responses.back() : responses.back();

            if (onI2CReadResult)
                onI2CReadResult(response, true);
        }
        else
        {
            if (onI2CReadResult && !framePackage->responses.empty())
                onI2CReadResult(framePackage->responses.back(), false);
        }
    }
}

void TACPSOCDriveThread::handleI2CWrite(FramePackage& framePackage)
{
    if (onI2CWriteResult)
    {
        if (framePackage->valid)
        {
            // Build "Request arg0 Successful"
            std::string argStr;
            if (!framePackage->arguments.empty())
                argStr = std::get<std::string>(framePackage->arguments.at(0));
            const qtac::ByteArray msg = framePackage->request + " " + argStr.c_str() + " Successful";
            onI2CWriteResult(msg);
        }
        else
        {
            const qtac::ByteArray last =
                framePackage->responses.empty() ? qtac::ByteArray() : framePackage->responses.back();
            onI2CWriteResult(qtac::ByteArray("\"") + last + "\" Are the parameters correct?");
        }
    }
}

void TACPSOCDriveThread::handleSetPin(FramePackage& framePackage)
{
    // arguments: [0]=bool state, [1]=uint32_t pin
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

void TACPSOCDriveThread::handleSetName(FramePackage& framePackage)
{
    if (!framePackage->arguments.empty())
    {
        _name = qtac::ByteArray(std::get<std::string>(framePackage->arguments.at(0)));
        framePackage->synonym = qtac::ByteArray("Set Name ") + _name;
    }
    if (onNameUpdate) onNameUpdate(qtac::String(_name.toStdString()));
}

void TACPSOCDriveThread::handleUUIDResponse(FramePackage& framePackage)
{
    if (framePackage->responses.size() > 1)
    {
        _uuid = qtac::String(framePackage->responses.at(1).toStdString());
        if (onUuidUpdate) onUuidUpdate(_uuid);
    }
}

void TACPSOCDriveThread::handleVersionResponse(FramePackage& framePackage)
{
    static int retryCount{1};

    if (framePackage->responses.size() > 1)
    {
        bool        isEPM{false};
        std::string versionString = framePackage->responses.at(0).toStdString();
        if (versionString.find("EPM") != std::string::npos)
        {
            isEPM = true;
        }
        else
        {
            versionString = framePackage->responses.at(1).toStdString();
            if (versionString.find("EPM") != std::string::npos)
                isEPM = true;
        }

        _versionString = qtac::ByteArray(versionString);
        writeLogLine("Version String: " + versionString);

        retryCount = 1;

        _hardwareType = isEPM ? ePSOC : eSpiderBoard;

        if (onHardwareTypeUpdate) onHardwareTypeUpdate(debugBoardTypeString());

        // Parse "... FW <type> <version> ..." to extract firmware version string.
        // Split on whitespace.
        std::vector<std::string> parts;
        {
            std::istringstream iss(versionString);
            std::string tok;
            while (iss >> tok)
                parts.push_back(tok);
        }

        if (parts.size() >= 7)
        {
            auto it = std::find(parts.begin(), parts.end(), "FW");
            if (it != parts.end() && (it + 2) < parts.end())
            {
                try
                {
                    const std::string firmwareStr = *(it + 2);
                    _firmwareString = qtac::ByteArray(firmwareStr);

                    // Split version on "."
                    std::vector<std::string> vParts;
                    {
                        std::istringstream viss(firmwareStr);
                        std::string vtok;
                        while (std::getline(viss, vtok, '.'))
                            vParts.push_back(vtok);
                    }
                    if (vParts.size() >= 4)
                    {
                        _firmwareMajor    = static_cast<unsigned int>(std::stoul(vParts.at(0)));
                        _firmwareChip     = static_cast<unsigned int>(std::stoul(vParts.at(1)));
                        _firmwareMinor    = static_cast<unsigned int>(std::stoul(vParts.at(2)));
                        _firmwareRevision = static_cast<unsigned int>(std::stoul(vParts.at(3)));

                        if (_firmwareMinor < 15)
                            _oldFirmware = true;
                    }

                    if (onFirmwareVersionUpdate)
                        onFirmwareVersionUpdate(qtac::String(firmwareStr));
                }
                catch (...)
                {
                    writeLogLine("Version Parts Failed");
                }
            }
        }

        if (onDeviceStatusChange) onDeviceStatusChange(qtac::String("TAC Version Good"));
        setupConnected();
    }
    else if (retryCount < 4)
    {
        writeLogLine("Version Failed Retry " + std::to_string(retryCount));
        TACPSOCCommand cmd(this, this);
        cmd.version();
        ++retryCount;
    }
}

void TACPSOCDriveThread::handlePlatformID(FramePackage& framePackage)
{
    if (framePackage->responses.size() >= 2)
    {
        try
        {
            _platformID = static_cast<PlatformID>(std::stoi(framePackage->responses.at(1).toStdString()));
        }
        catch (...) {}
    }
    if (onPlatformIDUpdate) onPlatformIDUpdate(static_cast<int>(_platformID));
    setupConnected();
}

// -----------------------------------------------------------------------
// Setup callbacks
// -----------------------------------------------------------------------

void TACPSOCDriveThread::setupConnected()
{
    if (!_connected)
    {
        if (onHardwareVersionUpdate) onHardwareVersionUpdate(hardwareVersionString());

        _connected = true;

        {
            TACPSOCCommand cmd(this, this);
            cmd.uuid();
            cmd.name();
        }

        if (onDeviceConnected) onDeviceConnected();
    }
}

void TACPSOCDriveThread::setupDiscovery()
{
    if (onHardwareVersionUpdate) onHardwareVersionUpdate(hardwareVersionString());

    TACPSOCCommand cmd(this, this);
    cmd.uuid();
    cmd.name();
    cmd.platformID();
}

} // namespace qtac
