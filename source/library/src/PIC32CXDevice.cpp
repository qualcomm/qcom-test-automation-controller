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

#include <qtac/PIC32CXDevice.h>
#include <qtac/TACPIC32CXDriveThread.h>
#include <qtac/SerialPortInfo.h>
#include <qtac/StringUtilities.h>
#include <qtac/PlatformID.h>
#include <qtac/TcnfLoader.h>

#include <chrono>
#include <thread>

using qtac::SerialPortInfo;
using qtac::SerialPortInfos;
using qtac::TACPIC32CXDriveThread;

// PIC32CX VID/PID (from qcommon-console/PIC32CXSerialTableModel)
static constexpr uint16_t kPIC32CXVid{0x04D8}, kPIC32CXPid{0x000A};

static const qtac::ByteArray kPIC32CXDefaultDescription{"ALPACA PIC32CX Debug Board"};

static constexpr int kMaxOpenIterations{60};
static constexpr int kOpenPollMs{300};
static constexpr int kOpenInitMs{100};
static constexpr unsigned int kOpenThreadDelay{300};

uint32_t PIC32CXDevice::updateAlpacaDevices()
{
    SerialPortInfos ports = SerialPortInfo::availablePorts();
    for (const auto& info : ports)
    {
        if (!info.matchesVidPid(kPIC32CXVid, kPIC32CXPid))
            continue;

        qtac::ByteArray portName     = info.portName().toLatin1();
        qtac::ByteArray serialNumber = info.serialNumber().toLatin1();
        HashType hash                = arrayHash(portName);

        AlpacaDevice existing = findAlpacaDevice(hash);
        if (!existing)
        {
            auto* dev = new PIC32CXDevice;
            dev->_portName     = portName;
            dev->_hash         = hash;
            dev->_active       = true;
            dev->_boardType    = ePIC32CXAuto;
            dev->_platformID   = ALPACA_PIC32CX_ID;
            dev->_description  = kPIC32CXDefaultDescription;
            dev->_serialNumber = serialNumber;

            // Use the portion of the serial number before "XX" as a USB
            // descriptor for platform identification (mirrors the Qt original).
            qtac::String serialStr = info.serialNumber();
            int xxIdx = serialStr.indexOf("XX");
            if (xxIdx > 0)
                dev->_usbDescriptor = serialStr.left(xxIdx).toLatin1();

            _alpacaDevices.append(AlpacaDevice(dev));
        }
        else
        {
            existing->setActive(true);
        }
    }

    return static_cast<uint32_t>(_alpacaDevices.size());
}

bool PIC32CXDevice::open()
{
    bool result{false};

    if (_serialDriveThread == nullptr)
    {
        TACPIC32CXDriveThread* thread = new TACPIC32CXDriveThread(_hash);
        _serialDriveThread = thread;
        thread->setThreadDelay(kOpenThreadDelay);

        _serialDriveThread->start();

        std::this_thread::sleep_for(std::chrono::milliseconds(kOpenInitMs));

        for (int i = 0; i < kMaxOpenIterations; ++i)
        {
            if (_serialDriveThread->weAreRunning()) { result = true; break; }
            std::this_thread::sleep_for(std::chrono::milliseconds(kOpenPollMs));
        }
    }
    else
    {
        result = _serialDriveThread->weAreRunning();
    }

    if (result)
    {
        _pic32cxPlatformConfiguration = new _PIC32CXPlatformConfiguration;

        // Load .tcnf override if a config path is registered for this platform
        if (_platformID != MICRO_EPM_BOARD_ID_UNKNOWN)
        {
            PlatformContainer::initialize();
            auto entries = PlatformContainer::getEntries();
            for (const auto& entry : entries)
            {
                if (entry && entry->_platformID == _platformID && !entry->_path.isEmpty())
                {
                    TcnfLoader::loadPIC32CX(entry->_path.toStdString(), _pic32cxPlatformConfiguration);
                    break;
                }
            }
        }

        _serialDriveThread->onPinStateChanged.connect([this](uint64_t pin, bool state) {
            this->on_pinStateChanged(pin, state);
        });

        buildMapping();
    }

    TACPIC32CXDriveThread* thread = static_cast<TACPIC32CXDriveThread*>(_serialDriveThread);
    _macAddress  = _serialDriveThread->macAddress();
    _chipVersion = thread->chipVersion();
    thread->setThreadDelay(0);

    return result;
}

void PIC32CXDevice::buildMapping()
{
    if (!_commands.isEmpty() || _pic32cxPlatformConfiguration == nullptr)
        return;

    Pins pins = _pic32cxPlatformConfiguration->getPins();
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
