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

// Qt-free port of STM32Device from qcommon-console.

#include <qtac/STM32Device.h>
#include <qtac/TACSTM32DriveThread.h>
#include <qtac/PlatformID.h>
#include <qtac/StringUtilities.h>
#include <qtac/CommandGroup.h>

#include "hidapi.h"

#include <thread>
#include <chrono>
#include <map>
#include <string>
#include <cstring>

// -----------------------------------------------------------------------
// HID device identifiers for BugHopper V2 (Arduino STM32)
// -----------------------------------------------------------------------
namespace {

const uint16_t kSTM32VendorID  = 0x2341;
const uint16_t kSTM32ProductID = 0xB001;
const uint16_t kSTM32UsagePage = 0xFF60;
const uint16_t kSTM32Usage     = 0x0061;

const int kMaxOpenIterations = 50;
const int kOpenPollMs        = 20;

// In-memory VTP port name map — keyed by serial number string.
// Replaces QSettings; resets each process run (sufficient for headless library use).
std::map<std::string, std::string>& vtpPortMap()
{
    static std::map<std::string, std::string> instance;
    return instance;
}

int& vtpCounter()
{
    static int counter{0};
    return counter;
}

} // anonymous namespace

// -----------------------------------------------------------------------
// STM32Device destructor
// -----------------------------------------------------------------------

STM32Device::~STM32Device()
{
    delete _stm32PlatformConfiguration;
    _stm32PlatformConfiguration = nullptr;
    // _hidHandle is closed by the drive thread calling close(); destructor
    // closes it only as a safety net.
    if (_hidHandle != nullptr)
    {
        hid_close(static_cast<hid_device*>(_hidHandle));
        _hidHandle = nullptr;
    }
}

// -----------------------------------------------------------------------
// HID enumeration helpers
// -----------------------------------------------------------------------

STM32HIDDeviceInfoList STM32Device::enumerateHIDDevices()
{
    STM32HIDDeviceInfoList result;

    struct hid_device_info* devs = hid_enumerate(kSTM32VendorID, kSTM32ProductID);
    struct hid_device_info* cur  = devs;

    while (cur != nullptr)
    {
        if (cur->usage_page == kSTM32UsagePage && cur->usage == kSTM32Usage)
        {
            STM32HIDDeviceInfo info;
            if (cur->path)
                info._devicePath = qtac::ByteArray(cur->path);

            if (cur->serial_number)
            {
                // Convert wchar_t serial number to narrow string
                std::wstring ws(cur->serial_number);
                std::string  narrow(ws.begin(), ws.end());
                info._serialNumber = qtac::ByteArray(narrow.c_str());
            }

            if (!info._serialNumber.isEmpty())
                info._hash = strHash(info._serialNumber);
            else if (!info._devicePath.isEmpty())
                info._hash = strHash(info._devicePath);

            result.append(info);
        }
        cur = cur->next;
    }

    hid_free_enumeration(devs);
    return result;
}

qtac::ByteArray STM32Device::vtpPortName(const qtac::ByteArray& serialNumber)
{
    std::string key = serialNumber.toStdString();
    auto& map = vtpPortMap();

    auto it = map.find(key);
    if (it != map.end())
        return qtac::ByteArray(it->second.c_str());

    std::ostringstream oss;
    oss << "VTP" << (++vtpCounter());
    std::string name = oss.str();
    map[key] = name;
    return qtac::ByteArray(name.c_str());
}

// -----------------------------------------------------------------------
// Static device registry
// -----------------------------------------------------------------------

uint32_t STM32Device::updateAlpacaDevices()
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    STM32HIDDeviceInfoList devices = enumerateHIDDevices();

    for (const auto& info : devices)
    {
        AlpacaDevice existing = _AlpacaDevice::findAlpacaDevice(info._hash);
        if (existing == nullptr)
        {
            STM32Device* dev = new STM32Device;

            dev->_active       = true;
            dev->_boardType    = eSTM32;
            dev->_hash         = info._hash;
            dev->_serialNumber = info._serialNumber;
            dev->_platformID   = ALPACA_STM32_ID;
            dev->_description  = qtac::ByteArray("Arduino BugHopper V2 Board");
            dev->_devicePath   = info._devicePath;
            dev->_portName     = vtpPortName(info._serialNumber);

            AlpacaDevice alpacaDevice = AlpacaDevice(dev);
            _AlpacaDevice::_alpacaDevices.append(alpacaDevice);
        }
        else
        {
            existing->setActive();
        }
    }

    return static_cast<uint32_t>(_AlpacaDevice::_alpacaDevices.size());
}

// -----------------------------------------------------------------------
// open() — create and start the drive thread
// -----------------------------------------------------------------------

bool STM32Device::open()
{
    bool result{false};

    if (_serialDriveThread == nullptr)
    {
        qtac::TACSTM32DriveThread* thread;

        if (_driveThread != nullptr)
        {
            // GUI path: caller already injected a drive thread. Alias it so
            // close() can detect the alias and avoid double-delete.
            thread = static_cast<qtac::TACSTM32DriveThread*>(_driveThread);
            _serialDriveThread = thread;
        }
        else
        {
            // Headless / test path: create our own thread (self-owned, deleted by close()).
            thread = new qtac::TACSTM32DriveThread(_hash);
            _serialDriveThread = thread;
        }

        _serialDriveThread->start();

        for (int i = 0; i < kMaxOpenIterations; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(kOpenPollMs));
            if (_serialDriveThread->weAreRunning()) { result = true; break; }
        }

        if (!result)
        {
            _lastError = "BugHopper V2 device did not become ready after open";
            return false;
        }
    }
    else
    {
        result = _serialDriveThread->weAreRunning();
    }

    if (result)
    {
        _stm32PlatformConfiguration = new _STM32PlatformConfiguration;

        _serialDriveThread->onPinStateChanged.connect([this](uint64_t pin, bool state) {
            this->on_pinStateChanged(pin, state);
        });

        buildMapping();
    }

    return result;
}

// -----------------------------------------------------------------------
// buildMapping / buildCommandList
// -----------------------------------------------------------------------

void STM32Device::buildMapping()
{
    if (_stm32PlatformConfiguration == nullptr)
        return;
    buildCommandList();
}

void STM32Device::buildCommandList()
{
    if (!_commands.isEmpty() || _stm32PlatformConfiguration == nullptr)
        return;

    STM32PinList pinList = _stm32PlatformConfiguration->getActivePins();

    for (const auto& stm32Pin : pinList)
    {
        TACCommand tacCommand;

        tacCommand._pin          = stm32Pin._pin;
        tacCommand._command      = stm32Pin._pinCommand;
        tacCommand._helpText     = stm32Pin._pinTooltip;
        tacCommand._currentState = false;
        tacCommand._isInverted   = stm32Pin._inverted;
        tacCommand._tabName      = stm32Pin._tabName.toLatin1();
        tacCommand._groupName    = CommandGroup::toString(stm32Pin._commandGroup).toLatin1();

        qtac::ByteArray cellLocation =
            qtac::ByteArray::number(stm32Pin._cellLocation.x()) + ","
            + qtac::ByteArray::number(stm32Pin._cellLocation.y());
        tacCommand._cellLocation = cellLocation;

        _commands[tacCommand._command] = tacCommand;
    }
}

// -----------------------------------------------------------------------
// setPinState — invert physical state if configured, then delegate
// -----------------------------------------------------------------------

void STM32Device::setPinState(PinID pin, bool state)
{
    if (_stm32PlatformConfiguration != nullptr)
    {
        if (_stm32PlatformConfiguration->getPinInvertedState(pin))
            state = !state;
    }
    _AlpacaDevice::setPinState(pin, state);
}

// -----------------------------------------------------------------------
// HID transport (called from TACSTM32DriveThread)
// -----------------------------------------------------------------------

bool STM32Device::openTransport()
{
    if (_devicePath.isEmpty())
        return false;

    hid_device* handle = hid_open_path(_devicePath.data());
    if (handle == nullptr)
        return false;

    _hidHandle = static_cast<void*>(handle);
    return true;
}

bool STM32Device::write(const qtac::ByteArray& report)
{
    if (_hidHandle == nullptr)
        return false;

    hid_device* handle = static_cast<hid_device*>(_hidHandle);
    int written = hid_write(handle,
                            reinterpret_cast<const unsigned char*>(report.data()),
                            static_cast<size_t>(report.size()));
    return written >= 0;
}

void STM32Device::close()
{
    if (_hidHandle != nullptr)
    {
        hid_close(static_cast<hid_device*>(_hidHandle));
        _hidHandle = nullptr;
    }
}
