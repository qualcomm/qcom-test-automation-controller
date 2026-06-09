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

#include <algorithm>
#include <cctype>
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

static std::string toLower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

static std::string trim(const std::string& s)
{
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos)
        return {};
    const auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

static bool endsWith(const std::string& s, const std::string& suffix)
{
    if (suffix.size() > s.size())
        return false;
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static bool startsWith(const std::string& s, const std::string& prefix)
{
    return s.size() >= prefix.size() &&
           s.compare(0, prefix.size(), prefix) == 0;
}

static uint64_t tickCount()
{
    using namespace std::chrono;
    return static_cast<uint64_t>(
        duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

// -----------------------------------------------------------------------
// Command string constants (mirrors TACCommands.h kXxx values)
// -----------------------------------------------------------------------

static const std::string kSetNameCommandStr    {"Set Name"};
static const std::string kSetNameAliasStr      {"setname"};
static const std::string kSetButtonAssertTime  {"Set Button Assert Time"};
static const std::string kSetButtonAssertAlias {"setbtnassert"};
static const std::string kSetPowerKeyDelay     {"Set Power Key Delay"};
static const std::string kSetPowerKeyDelayAlias{"setpkdelay"};
static const std::string kSetPinCommandStr     {"SetPin"};
static const std::string kSetPinCommandLower   {"setpin"};

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
    std::string result = toLower(trim(command.toStdString()));

    if (endsWith(result, " on"))
    {
        args.push_back(true);
        result.erase(result.size() - 3);
    }
    else if (endsWith(result, " off"))
    {
        args.push_back(false);
        result.erase(result.size() - 4);
    }
    else if (endsWith(result, " 1"))
    {
        args.push_back(true);
        result.erase(result.size() - 2);
    }
    else if (endsWith(result, " 0"))
    {
        args.push_back(false);
        result.erase(result.size() - 2);
    }

    const std::string setNameLower   = toLower(kSetNameCommandStr);
    const std::string btnAssertLower = toLower(kSetButtonAssertTime);
    const std::string pkDelayLower   = toLower(kSetPowerKeyDelay);

    if (startsWith(result, setNameLower) || startsWith(result, kSetNameAliasStr))
    {
        // Strip the command prefix, leaving only the name argument.
        if (startsWith(result, setNameLower))
            result.erase(0, setNameLower.size());
        else
            result.erase(0, kSetNameAliasStr.size());

        result = trim(result);
        args.push_back(result);
        result = kSetNameCommandStr;
    }
    else if (startsWith(result, btnAssertLower) || startsWith(result, kSetButtonAssertAlias))
    {
        if (startsWith(result, btnAssertLower))
            result.erase(0, btnAssertLower.size());
        else
            result.erase(0, kSetButtonAssertAlias.size());

        result = trim(result);
        args.push_back(static_cast<uint32_t>(std::stoul(result)));
        result = kSetButtonAssertTime;
    }
    else if (startsWith(result, pkDelayLower) || startsWith(result, kSetPowerKeyDelayAlias))
    {
        if (startsWith(result, pkDelayLower))
            result.erase(0, pkDelayLower.size());
        else
            result.erase(0, kSetPowerKeyDelayAlias.size());

        result = trim(result);
        args.push_back(static_cast<uint32_t>(std::stoul(result)));
        result = kSetPowerKeyDelay;
    }
    else if (startsWith(result, kSetPinCommandLower))
    {
        result.erase(0, kSetPinCommandLower.size());
        result = trim(result);
        args.push_back(static_cast<uint32_t>(std::stoul(result)));
        result = kSetPinCommandStr;
    }

    // Resolve via hash — if the lower-case result maps to a known command,
    // use the canonical long-form name.  The concrete TACCommands registry
    // (CommandStringToHash / CommandHashToCommandEntry) is linked separately.
    // Here we return whatever normalised string we have; subclasses that need
    // full hash resolution can override or post-process.
    return qtac::ByteArray(result);
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
            framePackage->responses.push_back(_versionString.toStdString());
            return true;
        }
        break;

    case kGetNameCommandHash:
        if (!_name.isEmpty())
        {
            framePackage->responses.push_back(_name.toStdString());
            return true;
        }
        break;

    case kGetUUIDCommandHash:
        if (!_uuid.isEmpty())
        {
            framePackage->responses.push_back(_uuid.toStdString());
            return true;
        }
        break;

    case kGetPlatformIDCommandHash:
        if (_platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
        {
            const qtac::String desc = PlatformContainer::toString(_platformID);
            std::ostringstream oss;
            oss << desc.toStdString() << "(" << static_cast<int>(_platformID) << ")";
            framePackage->responses.push_back(oss.str());
            return true;
        }
        break;

    case kPIC32CXVersionCommandHash:
        if (!_versionString.isEmpty())
        {
            framePackage->responses.push_back(_versionString.toStdString());
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

void TACDriveThread::writeLogLine(const std::string& line)
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
        const std::string entry = oss.str();
        timeStampLogMessage(qtac::String(entry));
        writeLogLine(entry);
    }
    else if (!framePackage->comment.empty())
    {
        writeLogLine(framePackage->comment);
    }
    else
    {
        std::string entry;
        if (framePackage->console)
            entry = "Request from console: " + framePackage->request;
        else
            entry = "Request: " + framePackage->request;
        writeLogLine(entry);

        if (!framePackage->synonym.empty())
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
    writeLogLine(oss.str());

    lastTimeStamp = current;
}

} // namespace qtac
