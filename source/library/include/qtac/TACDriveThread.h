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
// Qt-free reimplementation of TACDriveThread from qcommon-console.
// Qt signals replaced with std::function<> callbacks.

#pragma once

#include <qtac/DriveThread.h>
#include <qtac/ReceiveInterface.h>
#include <qtac/ByteArray.h>
#include <qtac/String.h>
#include <qtac/DebugBoardType.h>
#include <qtac/PlatformID.h>
#include <qtac/StringUtilities.h>
#include <qtac/Variant.h>
#include <qtac/FTDIPinSet.h>

#include <qtac/Signal.h>

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

namespace qtac {

// -----------------------------------------------------------------------
// _CommandEntry — Qt-free equivalent of the AlpacaScript _CommandEntry
// struct from qcommon-console.  Used by sendCommandSequence().
// -----------------------------------------------------------------------
struct _CommandEntry
{
    enum CommandAction
    {
        eNotSet,
        eSetPin,
        eLog,
        eDelay,
        eBaseCommand
    };

    CommandAction _commandAction{eNotSet};
    uint64_t      _pinID{static_cast<uint64_t>(-1)};
    qtac::ByteArray _action;
    qtac::Variant _arguement;    // original spelling preserved
};

using CommandEntry   = std::shared_ptr<_CommandEntry>;
using CommandEntries = std::vector<CommandEntry>;

// Notification level — mirrors NotificationLevel enum from qcommon-console.
enum class NotificationLevel
{
    Debug = 0,
    Info,
    Warn,
    Error
};

// -----------------------------------------------------------------------
// TACDriveThread
//
// Abstract mid-level drive thread.  Concrete hardware variants
// (TACLiteDriveThread, TACPSOCDriveThread, TACPIC32CXDriveThread) extend
// this class.
//
// Qt signals have been replaced with std::function<> callbacks.  The host
// application assigns callbacks immediately after construction (or after
// openPort()) before calling start().
// -----------------------------------------------------------------------
class TACDriveThread :
    public DriveThread,
    public ReceiveInterface
{
public:
    explicit TACDriveThread(HashType hash);
    virtual ~TACDriveThread();

    // Factory — creates the correct concrete subclass for the given port.
    static TACDriveThread* openPort(const qtac::ByteArray& portName);

    HashType hash();

    // Completion-wait helpers.  The caller sets the flag before queuing a
    // command, then calls waitForCompletion() which blocks (with a timeout)
    // until the drive thread calls clearWaitForCompletion().
    void waitForCompletion();
    void setWaitForCompletion();
    void clearWaitForCompletion();
    bool waitForCompletionStatus() const;

    // Decode a freeform command string into a canonical command + argument list.
    qtac::ByteArray decodeCommand(const qtac::ByteArray& command, Arguments& args);

    // Return true (and fill framePackage->responses) if this request can be
    // satisfied from cached local state without a round-trip to hardware.
    bool checkLocalStore(FramePackage& framePackage);

    // --- Pure virtual interface for concrete hardware threads ---

    virtual void sendCommand(const qtac::ByteArray& command, bool console = false,
                             ReceiveInterface* receiveInterface = nullptr, bool shouldStore = true) = 0;

    virtual void setPinState(uint64_t pin, bool state) = 0;
    virtual void setPinSets(FTDIPinSets /*pinsets*/) {} // no-op for non-FTDI threads
    virtual void sendCommandSequence(CommandEntries& commandEntries)    = 0;

    virtual int  getResetCount()   = 0;
    virtual void clearResetCount() = 0;

    virtual void i2CReadRegister(uint32_t addr, uint32_t reg)                   = 0;
    virtual void i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)   = 0;

    // --- Hardware-identity properties ---
    DebugBoardType debugBoardType();
    qtac::String   debugBoardTypeString();

    PlatformID   platformID();
    virtual qtac::String hardwareVersionString();

    // --- Firmware properties ---
    bool         oldFirmware();
    qtac::String firmwareVersion();
    unsigned int majorVersion();
    unsigned int chipVersion();
    unsigned int minorVersion();
    unsigned int revisionVersion();

    // --- Name / port ---
    qtac::ByteArray name() const;
    virtual void setName(const qtac::ByteArray& newName);

    qtac::ByteArray portName() const;
    void setPortName(const qtac::ByteArray& portName);

    // --- Optional descriptive properties ---
    qtac::String description() const;
    void setDescription(const qtac::String& description);

    qtac::String serialNumber() const;
    void setSerialNumber(const qtac::String& serialNumber);

    qtac::String uuid();

    qtac::ByteArray macAddress();

    // --- SendInterface ---
    virtual uint32_t send(const qtac::ByteArray& sendMe, const Arguments& arguments,
                          bool command, ReceiveInterface* receiveInterface,
                          bool store = true) = 0;
    virtual bool ready() = 0;

    // --- ReceiveInterface ---
    virtual void receive(FramePackage& framePackage) = 0;

    void setThreadDelay(unsigned int delay);

    // -----------------------------------------------------------------------
    // Signals (replace Qt signals)
    //
    // Connect subscribers before calling start().
    // Each connect() returns an ID; pass it to disconnect() to unsubscribe.
    // -----------------------------------------------------------------------
    qtac::Signal<uint64_t, bool>                        onPinStateChanged;
    qtac::Signal<>                                      onTransactionEnded;
    qtac::Signal<uint8_t, NotificationLevel>            onProgress;

    qtac::Signal<>                                      onDeviceOpen;
    qtac::Signal<const qtac::ByteArray&>                onErrorOnOpen;

    qtac::Signal<const qtac::String&>                   onDeviceStatusChange;

    qtac::Signal<const qtac::String&>                   onHardwareTypeUpdate;
    qtac::Signal<const qtac::String&>                   onHardwareVersionUpdate;
    qtac::Signal<const qtac::String&>                   onFirmwareVersionUpdate;
    qtac::Signal<const qtac::String&>                   onNameUpdate;
    qtac::Signal<const qtac::String&>                   onUuidUpdate;
    qtac::Signal<const qtac::String&>                   onSerialNumUpdate;
    qtac::Signal<int>                                   onPlatformIDUpdate;

    qtac::Signal<>                                      onDeviceConnected;
    qtac::Signal<>                                      onDeviceDisconnected;

    qtac::Signal<>                                      onResetCountCleared;
    qtac::Signal<uint32_t>                              onResetCountUpdate;

    qtac::Signal<const qtac::ByteArray&, bool>          onI2CReadResult;
    qtac::Signal<const qtac::ByteArray&>                onI2CWriteResult;

    // Optional log sink — if set, log() and error messages route here instead
    // of to a global AppCore.
    qtac::Signal<const qtac::ByteArray&>                onLogLine;

protected:
    bool            _waitForCompletion{false};
    std::mutex              _completionMutex;
    std::condition_variable _completionCv;

    HashType        _hash{0};
    bool            _oldFirmware{false};
    qtac::ByteArray _portName;
    DebugBoardType  _hardwareType{eUnknownDebugBoard};
    PlatformID      _platformID{MICRO_EPM_BOARD_ID_UNKNOWN};
    qtac::ByteArray _versionString;
    qtac::ByteArray _firmwareString;
    unsigned int    _firmwareMajor{0};
    unsigned int    _firmwareChip{0};
    unsigned int    _firmwareMinor{0};
    unsigned int    _firmwareRevision{0};
    qtac::ByteArray _name;
    qtac::ByteArray _macAddress;
    qtac::String    _description;
    qtac::String    _uuid;
    qtac::String    _serialNumber;
    qtac::String    _mcnNumber;
    qtac::String    _helpText;

    int          _resetCount{0};
    unsigned int _delay{0};

    virtual void setupConnected() = 0;
    virtual void setupDiscovery() = 0;

    // Logging helpers.
    void log(FramePackage& framePackage);
    void timeStampLogMessage(const qtac::String& timeStampMe);
    void writeLogLine(const qtac::ByteArray& line);
};

} // namespace qtac
