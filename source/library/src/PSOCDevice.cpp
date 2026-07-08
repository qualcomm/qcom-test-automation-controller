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

#include <qtac/PSOCDevice.h>
#include <qtac/TACPSOCDriveThread.h>
#include <qtac/SerialPortInfo.h>
#include <qtac/StringUtilities.h>
#include <qtac/PlatformID.h>
#include <qtac/TcnfLoader.h>

#include <algorithm>
#include <chrono>
#include <thread>

using qtac::SerialPortInfo;
using qtac::SerialPortInfos;
using qtac::TACPSOCDriveThread;

// PSoC VID/PID pairs (from qcommon-console/PSOCSerialTableModel)
static constexpr uint16_t kPSOCVid1{0x05C6}, kPSOCPid1{0x9302};
static constexpr uint16_t kPSOCVid2{0x16C0}, kPSOCPid2{0x0483};

static constexpr int kMaxOpenIterations{60};
static constexpr int kMaxPlatformIdIterations{10};
static constexpr int kOpenPollMs{300};
static constexpr int kOpenInitMs{100};

uint32_t PSOCDevice::updateAlpacaDevices()
{
    for (auto& dev : _alpacaDevices)
        dev->setActive(false);

    SerialPortInfos ports = SerialPortInfo::availablePorts();
    for (const auto& info : ports)
    {
        if (!info.matchesVidPid(kPSOCVid1, kPSOCPid1) &&
            !info.matchesVidPid(kPSOCVid2, kPSOCPid2))
            continue;

        qtac::ByteArray portName = info.portName().toLatin1();
        HashType hash = arrayHash(portName);

        AlpacaDevice existing = findAlpacaDevice(hash);
        if (!existing)
        {
            auto* dev = new PSOCDevice;
            dev->_portName     = portName;
            dev->_hash         = hash;
            dev->_active       = true;
            dev->_boardType    = ePSOC;
            dev->_platformID   = MICRO_EPM_BOARD_ID_UNKNOWN;
            dev->_description  = info.description().toLatin1();
            dev->_serialNumber = info.serialNumber().toLatin1();
            _alpacaDevices.append(AlpacaDevice(dev));
        }
        else
        {
            existing->setActive(true);
        }
    }

    return static_cast<uint32_t>(_alpacaDevices.size());
}

bool PSOCDevice::open()
{
    bool result{false};

    if (_serialDriveThread == nullptr)
    {
        TACPSOCDriveThread* thread = new TACPSOCDriveThread(_hash);
        _serialDriveThread = thread;

        _serialDriveThread->start();

        std::this_thread::sleep_for(std::chrono::milliseconds(kOpenInitMs));

        for (int i = 0; i < kMaxOpenIterations; ++i)
        {
            if (_serialDriveThread->weAreRunning()) { result = true; break; }
            std::this_thread::sleep_for(std::chrono::milliseconds(kOpenPollMs));
        }

        if (!result)
        {
            _lastError = "PSoC device did not become ready after open";
            return false;
        }

        result = false;
        for (int i = 0; i < kMaxPlatformIdIterations; ++i)
        {
            PlatformID pid = thread->platformID();
            if (pid == MICRO_EPM_BOARD_ID_UNKNOWN)
            {
                if (thread->oldFirmware())
                {
                    _lastError = "Firmware version is too old";
                    return false;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(kOpenInitMs));
            }
            else
            {
                _platformID = pid;
                result = true;
                break;
            }
        }

        // Firmware that doesn't support 'Get Platform ID' leaves platformID UNKNOWN
        // but the device is still usable if it connected and returned a version string.
        if (!result && thread->connected())
            result = true;
    }
    else
    {
        result = _serialDriveThread->weAreRunning();
    }

    if (result)
    {
        _psocPlatformConfiguration = new _PSOCPlatformConfiguration;

        // Load .tcnf override if a config path is registered for this platform
        if (_platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
        {
            PlatformContainer::initialize();
            auto entries = PlatformContainer::getEntries();
            for (const auto& entry : entries)
            {
                if (entry && entry->_platformID == _platformID && !entry->_path.isEmpty())
                {
                    TcnfLoader::loadPSOC(entry->_path.toStdString(), _psocPlatformConfiguration);
                    break;
                }
            }
        }

        _serialDriveThread->onPinStateChanged.connect([this](uint64_t pin, bool state) {
            this->on_pinStateChanged(pin, state);
        });

        buildMapping();

        Pins pins = _psocPlatformConfiguration->getPins();
        Pins initialPins;
        for (const auto& pin : pins)
            if (pin._initializationPriority > 0)
                initialPins.append(pin);
        for (const auto& pin : pins)
            if (pin._initialValue)
                initialPins.append(pin);

        if (!initialPins.isEmpty())
        {
            std::sort(initialPins.begin(), initialPins.end(),
                [](const PinEntry& a, const PinEntry& b) {
                    return a._initializationPriority < b._initializationPriority;
                });
            for (const auto& pin : initialPins)
                _serialDriveThread->setPinState(static_cast<uint16_t>(pin._pin), true);
        }

        TACPSOCDriveThread* thread = static_cast<TACPSOCDriveThread*>(_serialDriveThread);
        _chipVersion = thread->chipVersion();
    }

    return result;
}

void PSOCDevice::buildMapping()
{
    if (!_commands.isEmpty() || _psocPlatformConfiguration == nullptr)
        return;

    Pins pins = _psocPlatformConfiguration->getPins();
    for (const auto& pin : pins)
    {
        TACCommand tacCommand;
        tacCommand._pin       = pin._pin;
        tacCommand._command   = pin._pinCommand;
        tacCommand._helpText  = pin._pinTooltip;
        tacCommand._tabName   = pin._tabName.toLatin1();
        tacCommand._groupName = CommandGroup::toString(pin._commandGroup).toLatin1();

        qtac::ByteArray cellLocation =
            qtac::ByteArray::number(pin._cellLocation.x()) + ","
            + qtac::ByteArray::number(pin._cellLocation.y());
        tacCommand._cellLocation = cellLocation;

        _commands[tacCommand._command] = tacCommand;
        _commandList.append(tacCommand);
    }
}
