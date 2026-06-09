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

#include <qtac/TACLiteDriveThread.h>
#include <qtac/TACLiteCommand.h>
#include <qtac/AlpacaDevice.h>
#include <qtac/TACCommandHashes.h>
#include <qtac/FTDIChipset.h>
#include <qtac/PlatformID.h>

#include <sstream>
#include <thread>
#include <variant>

static const qtac::ByteArray kTACLiteDriveTrainName{"TAC Lite Drive Train"};
static const qtac::ByteArray kHelpCommand{"Help"};

namespace qtac {

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

TACLiteDriveThread::TACLiteDriveThread(HashType hash)
    : TACDriveThread(hash)
{
    _driveTrainName = kTACLiteDriveTrainName;
    setProtocolInterface(&_tacProtocol);
    _tacProtocol.setTACDriveTrain(this);

    AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
    if (!alpacaDevice || !alpacaDevice->active())
        return;

    _ftdiChipset = _FTDIChipset::getDevice(alpacaDevice->portName());
    if (!_ftdiChipset)
        return;

    _portName        = _ftdiChipset->portName();
    _platformID      = _ftdiChipset->platformID();
    _name            = _ftdiChipset->serialNumber();
    _versionString   = qtac::ByteArray(_ftdiChipset->versionString().toStdString());
    _firmwareString  = qtac::ByteArray(_ftdiChipset->firmwareString().toStdString());
    _firmwareMajor   = 1;
    _firmwareChip    = 10000;
    _firmwareMinor   = 1;
    _firmwareRevision = 1;
    _description     = "VTP Port";
    _uuid            = "FTDI - No UUID";
    _hardwareType    = eFTDI;

    setSerialNumber(_ftdiChipset->serialNumber().toStdString());
    setPortName(_ftdiChipset->portName());

    // Wire callbacks so _AlpacaDevice is notified of disconnect / progress.
    onDeviceDisconnected = [alpacaDevice]() {
        alpacaDevice->onDeviceDisconnect();
    };
    onProgress = [alpacaDevice](uint8_t value, NotificationLevel /*level*/) {
        if (alpacaDevice->onProgress)
            alpacaDevice->onProgress(value, 1 /*eInfoNotification*/);
    };
}

TACLiteDriveThread::~TACLiteDriveThread() = default;

// -----------------------------------------------------------------------
// FTDI device open
// -----------------------------------------------------------------------

bool TACLiteDriveThread::openFTDIDevice()
{
    if (!_ftdiChipset)
    {
        writeLogLine("TACLiteDriveThread::openFTDIDevice _ftdiChipset is null");
        if (onErrorOnOpen) onErrorOnOpen(qtac::ByteArray("FTDI device open failed. Check the application log"));
        return false;
    }

    if (_ftdiChipset->isOpen())
    {
        writeLogLine("TACLiteDriveThread::openFTDIDevice already open");
        if (onErrorOnOpen) onErrorOnOpen(qtac::ByteArray("FTDI device open failed. Check the application log"));
        return false;
    }

    if (!_ftdiChipset->open(_pinsets))
    {
        writeLogLine("TACLiteDriveThread::openFTDIDevice open(pinsets) failed");
        if (onErrorOnOpen) onErrorOnOpen(qtac::ByteArray("FTDI device open failed. Check the application log"));
        return false;
    }

    std::ostringstream oss;
    oss << "Opened port " << _ftdiChipset->portName().toStdString() << "\n";
    writeLogLine(oss.str());
    return true;
}

// -----------------------------------------------------------------------
// Public command methods
// -----------------------------------------------------------------------

void TACLiteDriveThread::externalPowerControl(bool state)
{
    {
        TACLiteCommand cmd(this, this);
        cmd.externalPowerControl(state);
    }
    waitForCompletion();
}

void TACLiteDriveThread::setPinState(uint16_t pin, bool state)
{
    {
        TACLiteCommand cmd(this, this);
        cmd.setPinState(pin, state);
    }
    waitForCompletion();
}

void TACLiteDriveThread::sendCommandSequence(CommandEntries& commandEntries)
{
    {
        TACLiteCommand cmd(this, this);

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

int  TACLiteDriveThread::getResetCount()  { return 0; }
void TACLiteDriveThread::clearResetCount() {}

void TACLiteDriveThread::i2CReadRegister(uint32_t /*addr*/, uint32_t /*reg*/)   {}
void TACLiteDriveThread::i2CWriteRegister(uint32_t /*addr*/, uint32_t /*reg*/, uint32_t /*data*/) {}

void TACLiteDriveThread::setName(const qtac::ByteArray& /*newName*/)
{
    // FTDI hardware does not support rename.
}

uint32_t TACLiteDriveThread::send(const qtac::ByteArray& sendMe,
                                   const Arguments& arguments,
                                   bool console,
                                   ReceiveInterface* receiveInterface,
                                   bool store)
{
    return _tacProtocol.sendCommand(sendMe, arguments, console, receiveInterface, store);
}

bool TACLiteDriveThread::ready()
{
    return _tacProtocol.queueSize() == 0;
}

// -----------------------------------------------------------------------
// receive — called back by TACLiteProtocol (and directly from run())
// -----------------------------------------------------------------------

void TACLiteDriveThread::receive(FramePackage& framePackage)
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
        case kVersionCommandHash:     handleVersionResponse(framePackage); break;
        case kGetPlatformIDCommandHash: handlePlatformID(framePackage);    break;
        case kGetUUIDCommandHash:     handleUUIDResponse(framePackage);    break;
        case kSetNameCommandHash:     handleSetName(framePackage);         break;
        case kSetPinCommandHash:      handleSetPin(framePackage);          break;
        default: break;
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

void TACLiteDriveThread::run()
{
    writeLogLine("TACLiteDriveThread::run()");

    if (openFTDIDevice())
    {
        writeLogLine("TACLiteDriveThread::run() openFTDIDevice() == true");
        if (onDeviceOpen) onDeviceOpen();
        startRunning();
    }

    if (weAreRunning())
    {
        if (onHardwareTypeUpdate)   onHardwareTypeUpdate(_versionString.toStdString());
        if (onHardwareVersionUpdate) onHardwareVersionUpdate("0.1.1");
        if (onFirmwareVersionUpdate) onFirmwareVersionUpdate(_firmwareString.toStdString());
        if (onNameUpdate)           onNameUpdate(_name.toStdString());
        if (onUuidUpdate)           onUuidUpdate(_uuid.toStdString());
        if (onSerialNumUpdate)      onSerialNumUpdate(_serialNumber.toStdString());
        if (onPlatformIDUpdate)     onPlatformIDUpdate(static_cast<int>(_platformID));

        if (onDeviceOpen)         onDeviceOpen();
        if (onDeviceStatusChange) onDeviceStatusChange("Starting");

        writeLogLine("setupConnected()");
        setupConnected();
    }
    else
    {
        writeLogLine("TACLiteDriveThread::run() deviceDisconnected");
        if (onDeviceDisconnected) onDeviceDisconnected();
    }

    {
        std::ostringstream oss;
        oss << "Hardware: " << static_cast<int>(_hardwareType) << "\n";
        writeLogLine(oss.str());
    }
    {
        const qtac::String pidStr = PlatformContainer::toString(_platformID);
        std::ostringstream oss;
        oss << "Platform ID: " << pidStr.toStdString()
            << "(" << static_cast<int>(_platformID) << ")\n";
        writeLogLine(oss.str());
    }

    if (weAreRunning())
    {
        writeLogLine("TACLiteDriveThread::run() starting loop");
        bool loopFinished = false;

        while (!loopFinished)
        {
            FramePackage framePackage = _protocolInterface->getNextFramePackage();

            if (framePackage)
            {
                if (framePackage->delayInMilliSeconds != 0 ||
                    !framePackage->comment.isEmpty()      ||
                    framePackage->endTransaction          ||
                    checkLocalStore(framePackage))
                {
                    receive(framePackage);
                }
                else
                {
                    // Parse the encoded pin index string from codedRequest.
                    int pin = 0;
                    try { pin = std::stoi(framePackage->codedRequest.toStdString()); }
                    catch (...) {}

                    const bool state = getElectricalPinValue(framePackage->requestHash,
                                                             framePackage->arguments);

                    {
                        std::ostringstream oss;
                        oss << "TACLiteDriveThread::run() write "
                            << framePackage->codedRequest.toStdString()
                            << " state:" << (state ? "on" : "off");
                        writeLogLine(oss.str());
                    }

                    if (!_ftdiChipset)
                    {
                        writeLogLine("TACLiteDriveThread::run() ftdiChipSet is null.");
                        stopRunning();
                    }
                    else if (!_ftdiChipset->write(pin, state))
                    {
                        std::ostringstream oss;
                        oss << "TACLiteDriveThread::run() write failure pin " << pin;
                        writeLogLine(oss.str());
                        stopRunning();
                    }
                    else
                    {
                        // Build response string from the first argument.
                        std::string argStr;
                        if (!framePackage->arguments.empty())
                        {
                            const Argument& arg = framePackage->arguments.at(0);
                            if (std::holds_alternative<bool>(arg))
                                argStr = std::get<bool>(arg) ? "on" : "off";
                            else if (std::holds_alternative<uint32_t>(arg))
                                argStr = std::to_string(std::get<uint32_t>(arg));
                            else
                                argStr = std::get<std::string>(arg);
                        }
                        framePackage->responses.push_back(framePackage->request + " " + argStr.c_str());
                        receive(framePackage);
                    }
                }
            }
            else
            {
                if (!weAreRunning())
                {
                    writeLogLine("TACLiteDriveThread::run() loopFinished == true\n");
                    loopFinished = true;
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }
        }
    }

    writeLogLine("TACLiteDriveThread::run() _ftdiChipset->close()");
    if (_ftdiChipset)
        _ftdiChipset->close();

    _connected = false;
    if (onDeviceDisconnected) onDeviceDisconnected();
}

// -----------------------------------------------------------------------
// sendCommand
// -----------------------------------------------------------------------

void TACLiteDriveThread::sendCommand(const qtac::ByteArray& command,
                                      bool console,
                                      ReceiveInterface* receiveInterface,
                                      bool shouldStore)
{
    if (!command.startsWith(kHelpCommand))
    {
        Arguments args;
        qtac::ByteArray decoded = decodeCommand(command, args);
        send(decoded.toStdString(), args, console, receiveInterface, shouldStore);
    }
}

// -----------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------

bool TACLiteDriveThread::getElectricalPinValue(HashType commandHash,
                                                const Arguments& arguments)
{
    if (commandHash == kSetPinCommandHash && !arguments.empty())
        return std::get<bool>(arguments.at(0));
    return false;
}

qtac::String TACLiteDriveThread::portDescription()
{
    return _description;
}

// -----------------------------------------------------------------------
// Frame handlers
// -----------------------------------------------------------------------

void TACLiteDriveThread::handleSetName(FramePackage& framePackage)
{
    if (!framePackage->arguments.empty())
    {
        const Argument& arg = framePackage->arguments.at(0);
        _name = qtac::ByteArray(std::get<std::string>(arg));
        framePackage->synonym = qtac::ByteArray("Set Name ") + _name;
    }
    if (onNameUpdate) onNameUpdate(_name.toStdString());
}

void TACLiteDriveThread::handleSetPin(FramePackage& framePackage)
{
    uint64_t pin   = 0;
    bool     state = false;

    if (framePackage->arguments.size() >= 1)
        state = std::get<bool>(framePackage->arguments.at(0));
    if (framePackage->arguments.size() >= 2)
        pin   = static_cast<uint64_t>(std::get<uint32_t>(framePackage->arguments.at(1)));

    framePackage->synonym = qtac::ByteArray("Set Pin ") + std::to_string(pin).c_str() + " " + (state ? "on" : "off");
    if (onPinStateChanged) onPinStateChanged(pin, state);
}

void TACLiteDriveThread::handleUUIDResponse(FramePackage& framePackage)
{
    if (framePackage->responses.size() > 1)
    {
        _uuid = framePackage->responses.at(1).toStdString();
        if (onUuidUpdate) onUuidUpdate(_uuid.toStdString());
    }
}

void TACLiteDriveThread::handleVersionResponse(FramePackage& /*framePackage*/)
{
    if (onDeviceStatusChange) onDeviceStatusChange("TAC Version Good");
    setupConnected();
}

void TACLiteDriveThread::handlePlatformID(FramePackage& framePackage)
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

void TACLiteDriveThread::setupConnected()
{
    if (!_connected)
    {
        _connected = true;
        if (onDeviceConnected) onDeviceConnected();
    }
}

void TACLiteDriveThread::setupDiscovery()
{
    // Nothing to do for FTDI.
}

} // namespace qtac
