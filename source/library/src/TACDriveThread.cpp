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
// Qt-free reimplementation of TACDriveThread.cpp from qcommon-console.

#include <qtac/TACDriveThread.h>
#include <qtac/AlpacaDevice.h>
#include <qtac/TACCommandHashes.h>
#include <qtac/PlatformID.h>

#include <chrono>
#include <sstream>
#include <thread>

// Forward declarations for concrete subclasses — included by the concrete
// compilation units, not here, to keep TACDriveThread.cpp thin.
namespace qtac {
class TACLiteDriveThread;
class TACPSOCDriveThread;
class TACPIC32CXDriveThread;
} // namespace qtac

namespace qtac {

// -----------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------

static uint64_t tickCount()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(
        duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

// -----------------------------------------------------------------------
// Command string constants (mirrors TACCommands.h kXxx values)
// -----------------------------------------------------------------------

static const qtac::ByteArray kSetNameCommandStr    {"Set Name"};
static const qtac::ByteArray kSetNameAliasStr      {"setname"};
static const qtac::ByteArray kSetButtonAssertTime  {"Set Button Assert Time"};
static const qtac::ByteArray kSetButtonAssertAlias {"setbtnassert"};
static const qtac::ByteArray kSetPowerKeyDelay     {"Set Power Key Delay"};
static const qtac::ByteArray kSetPowerKeyDelayAlias{"setpkdelay"};
static const qtac::ByteArray kSetPinCommandStr     {"SetPin"};
static const qtac::ByteArray kSetPinCommandLower   {"setpin"};

// -----------------------------------------------------------------------
// Construction / destruction
// -----------------------------------------------------------------------

TACDriveThread::TACDriveThread(HashType hash)
    : _hash(hash)
{
}

TACDriveThread::~TACDriveThread() = default;

// -----------------------------------------------------------------------
// Factory
// -----------------------------------------------------------------------

TACDriveThread* TACDriveThread::openPort(const qtac::ByteArray& portName)
{
    // Concrete subclass headers are included by the concrete .cpp files.
    // This factory performs the lookup only; callers link against the
    // concrete implementations.
    if (_AlpacaDevice::updateAlpacaDevices() == 0)
        return nullptr;

    AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(portName);
    if (!alpacaDevice || !alpacaDevice->active())
        return nullptr;

    // The concrete thread types are forward-declared here; their headers
    // must be included by any translation unit that uses openPort().
    // Return nullptr — the factory is intentionally left incomplete at
    // this layer so each concrete subclass can register itself.
    (void)alpacaDevice;
    return nullptr;
}

// -----------------------------------------------------------------------
// Hash accessor
// -----------------------------------------------------------------------

HashType TACDriveThread::hash()
{
    return _hash;
}

// -----------------------------------------------------------------------
// Completion wait
// -----------------------------------------------------------------------

void TACDriveThread::waitForCompletion()
{
    constexpr int kTimeoutMs    = 5000; // 50 × 100 ms
    constexpr int kPollIntervalMs = 100;

    std::unique_lock<std::mutex> lock(_completionMutex);
    const bool timedOut = !_completionCv.wait_for(
        lock,
        std::chrono::milliseconds(kTimeoutMs),
        [this] { return !_waitForCompletion; });

    if (timedOut)
    {
        writeLogLine("Wait for completion timed out.");
        _waitForCompletion = false;
    }
}

void TACDriveThread::setWaitForCompletion()
{
    std::lock_guard<std::mutex> lock(_completionMutex);
    _waitForCompletion = true;
}

void TACDriveThread::clearWaitForCompletion()
{
    {
        std::lock_guard<std::mutex> lock(_completionMutex);
        _waitForCompletion = false;
    }
    _completionCv.notify_all();
}

bool TACDriveThread::waitForCompletionStatus() const
{
    return _waitForCompletion;
}

// -----------------------------------------------------------------------
// decodeCommand
//
// Normalises a freeform command string into a canonical command + populates
// the Arguments list with any trailing on/off/1/0 or numeric parameter.
// -----------------------------------------------------------------------

qtac::ByteArray TACDriveThread::decodeCommand(const qtac::ByteArray& command, Arguments& args)
{
    qtac::String result = qtac::String(command.toStdString()).toLower().trimmed();

    if (result.endsWith(" on"))
    {
        args.push_back(true);
        result = result.left(result.size() - 3);
    }
    else if (result.endsWith(" off"))
    {
        args.push_back(false);
        result = result.left(result.size() - 4);
    }
    else if (result.endsWith(" 1"))
    {
        args.push_back(true);
        result = result.left(result.size() - 2);
    }
    else if (result.endsWith(" 0"))
    {
        args.push_back(false);
        result = result.left(result.size() - 2);
    }

    const qtac::String setNameLower    = qtac::String(kSetNameCommandStr.toStdString()).toLower();
    const qtac::String btnAssertLower  = qtac::String(kSetButtonAssertTime.toStdString()).toLower();
    const qtac::String pkDelayLower    = qtac::String(kSetPowerKeyDelay.toStdString()).toLower();

    if (result.startsWith(setNameLower) || result.startsWith(qtac::String(kSetNameAliasStr.toStdString())))
    {
        if (result.startsWith(setNameLower))
            result = result.mid(setNameLower.size());
        else
            result = result.mid(static_cast<int>(kSetNameAliasStr.size()));

        result = result.trimmed();
        args.push_back(result.toStdString());
        result = qtac::String(kSetNameCommandStr.toStdString());
    }
    else if (result.startsWith(btnAssertLower) || result.startsWith(qtac::String(kSetButtonAssertAlias.toStdString())))
    {
        if (result.startsWith(btnAssertLower))
            result = result.mid(btnAssertLower.size());
        else
            result = result.mid(static_cast<int>(kSetButtonAssertAlias.size()));

        result = result.trimmed();
        args.push_back(static_cast<uint32_t>(std::stoul(result.toStdString())));
        result = qtac::String(kSetButtonAssertTime.toStdString());
    }
    else if (result.startsWith(pkDelayLower) || result.startsWith(qtac::String(kSetPowerKeyDelayAlias.toStdString())))
    {
        if (result.startsWith(pkDelayLower))
            result = result.mid(pkDelayLower.size());
        else
            result = result.mid(static_cast<int>(kSetPowerKeyDelayAlias.size()));

        result = result.trimmed();
        args.push_back(static_cast<uint32_t>(std::stoul(result.toStdString())));
        result = qtac::String(kSetPowerKeyDelay.toStdString());
    }
    else if (result.startsWith(qtac::String(kSetPinCommandLower.toStdString())))
    {
        result = result.mid(static_cast<int>(kSetPinCommandLower.size()));
        result = result.trimmed();
        args.push_back(static_cast<uint32_t>(std::stoul(result.toStdString())));
        result = qtac::String(kSetPinCommandStr.toStdString());
    }

    return qtac::ByteArray(result.toStdString());
}

// -----------------------------------------------------------------------
// checkLocalStore
// -----------------------------------------------------------------------

bool TACDriveThread::checkLocalStore(FramePackage& framePackage)
{
    switch (framePackage->requestHash)
    {
    case kVersionCommandHash:
        if (!_versionString.isEmpty())
        {
            framePackage->responses.push_back(_versionString);
            return true;
        }
        break;

    case kGetNameCommandHash:
        if (!_name.isEmpty())
        {
            framePackage->responses.push_back(_name);
            return true;
        }
        break;

    case kGetUUIDCommandHash:
        if (!_uuid.isEmpty())
        {
            framePackage->responses.push_back(qtac::ByteArray(_uuid.toStdString()));
            return true;
        }
        break;

    case kGetPlatformIDCommandHash:
        if (_platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
        {
            const qtac::String desc = PlatformContainer::toString(_platformID);
            std::ostringstream oss;
            oss << desc.toStdString() << "(" << static_cast<int>(_platformID) << ")";
            framePackage->responses.push_back(qtac::ByteArray(oss.str()));
            return true;
        }
        break;

    default:
        break;
    }

    return false;
}

// -----------------------------------------------------------------------
// Hardware-identity properties
// -----------------------------------------------------------------------

DebugBoardType TACDriveThread::debugBoardType()
{
    return _hardwareType;
}

qtac::String TACDriveThread::debugBoardTypeString()
{
    return debugBoardTypeToString(_hardwareType);
}

PlatformID TACDriveThread::platformID()
{
    return _platformID;
}

qtac::String TACDriveThread::hardwareVersionString()
{
    return PlatformContainer::toString(_platformID);
}

// -----------------------------------------------------------------------
// Firmware properties
// -----------------------------------------------------------------------

bool TACDriveThread::oldFirmware()
{
    return _oldFirmware;
}

qtac::String TACDriveThread::firmwareVersion()
{
    return qtac::String(_firmwareString.toStdString());
}

unsigned int TACDriveThread::majorVersion()    { return _firmwareMajor;    }
unsigned int TACDriveThread::chipVersion()     { return _firmwareChip;     }
unsigned int TACDriveThread::minorVersion()    { return _firmwareMinor;    }
unsigned int TACDriveThread::revisionVersion() { return _firmwareRevision; }

// -----------------------------------------------------------------------
// Name / port
// -----------------------------------------------------------------------

qtac::ByteArray TACDriveThread::name() const    { return _name;     }
qtac::ByteArray TACDriveThread::portName() const { return _portName; }

void TACDriveThread::setName(const qtac::ByteArray& newName)   { _name = newName;   }
void TACDriveThread::setPortName(const qtac::ByteArray& p)     { _portName = p;     }

qtac::String TACDriveThread::description() const               { return _description; }
void TACDriveThread::setDescription(const qtac::String& d)     { _description = d;   }

qtac::String TACDriveThread::serialNumber() const              { return _serialNumber; }
void TACDriveThread::setSerialNumber(const qtac::String& sn)   { _serialNumber = sn;  }

qtac::String    TACDriveThread::uuid()       { return _uuid;       }
qtac::ByteArray TACDriveThread::macAddress() { return _macAddress; }

void TACDriveThread::setThreadDelay(unsigned int delay)
{
    _delay = delay;
}

// -----------------------------------------------------------------------
// Logging
// -----------------------------------------------------------------------

void TACDriveThread::writeLogLine(const qtac::ByteArray& line)
{
    if (onLogLine)
        onLogLine(line);
}

void TACDriveThread::log(FramePackage& framePackage)
{
    writeLogLine("");
    writeLogLine("Frame Package Start");
    writeLogLine(framePackage->valid ? "Frame Valid" : "Frame Invalid");

    if (framePackage->delayInMilliSeconds > 0)
    {
        std::ostringstream oss;
        oss << " Delay " << framePackage->delayInMilliSeconds << " in msecs";
        const qtac::ByteArray entry(oss.str().c_str());
        timeStampLogMessage(qtac::String(oss.str()));
        writeLogLine(entry);
    }
    else if (!framePackage->comment.isEmpty())
    {
        writeLogLine(framePackage->comment);
    }
    else
    {
        qtac::ByteArray entry;
        if (framePackage->console)
            entry = "Request from console: " + framePackage->request;
        else
            entry = "Request: " + framePackage->request;
        writeLogLine(entry);

        if (!framePackage->synonym.isEmpty())
            writeLogLine("Synonym: " + framePackage->synonym);

        writeLogLine("Responses");
        for (const auto& r : framePackage->responses)
            writeLogLine("   " + r);
    }

    writeLogLine("Frame Package End");
    writeLogLine("");
}

void TACDriveThread::timeStampLogMessage(const qtac::String& timeStampMe)
{
    static uint64_t lastTimeStamp{0};

    if (lastTimeStamp == 0)
        lastTimeStamp = tickCount();

    const uint64_t current = tickCount();

    std::ostringstream oss;
    oss << timeStampMe.toStdString()
        << "Time: " << current
        << " elapsed: " << (current - lastTimeStamp) << " (ms)";
    writeLogLine(qtac::ByteArray(oss.str().c_str()));

    lastTimeStamp = current;
}

} // namespace qtac
