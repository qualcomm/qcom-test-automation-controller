// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.

#include "Device.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

namespace taccl {

namespace {

// TACDev's GetPortData returns a semicolon-separated record:
// "PORT;DESCRIPTION;SERIAL;FLAGS"
// Index 0 = port name, index 1 = description, index 2 = serial number.
DeviceEntry parsePortData(const char* raw)
{
    DeviceEntry entry;
    entry.raw = raw ? raw : "";

    std::stringstream ss(entry.raw);
    std::string token;
    int index = 0;
    while (std::getline(ss, token, ';')) {
        if (index == 0) entry.port   = token;
        else if (index == 2) entry.serial = token;
        ++index;
    }
    return entry;
}

std::string getEnv(const char* name)
{
#ifdef _WIN32
    char*  buf = nullptr;
    size_t len = 0;
    if (_dupenv_s(&buf, &len, name) == 0 && buf) {
        std::string value(buf);
        free(buf);
        return value;
    }
    return {};
#else
    const char* value = std::getenv(name);
    return value ? std::string(value) : std::string();
#endif
}

} // namespace

std::vector<DeviceEntry> enumerateDevices()
{
    std::vector<DeviceEntry> devices;

    int count = 0;
    if (GetDeviceCount(&count) != NO_TAC_ERROR || count <= 0)
        return devices;

    for (int i = 0; i < count; ++i) {
        char buffer[1024]{};
        // GetPortData returns the number of bytes written (not an error code).
        // Accept any non-empty result.
        GetPortData(i, buffer, static_cast<int>(sizeof(buffer)));
        if (buffer[0] != '\0')
            devices.push_back(parsePortData(buffer));
    }
    return devices;
}

std::string resolvePortName(const std::string& deviceOpt,
                            const std::string& serialOpt)
{
    // Calling enumerateDevices() populates TACDev's internal device list,
    // which is required before OpenHandleByDescription() will succeed.
    // Do this unconditionally so the --device fast path also works.
    enumerateDevices();

    // Explicit --device wins.
    if (!deviceOpt.empty())
        return deviceOpt;

    // --serial -> look up port name via enumeration.
    if (!serialOpt.empty()) {
        for (const auto& dev : enumerateDevices()) {
            if (dev.serial == serialOpt)
                return dev.port;
        }
        return {}; // serial provided but no device matched
    }

    // Environment fallbacks.
    std::string envDevice = getEnv("TACCL_DEVICE");
    if (!envDevice.empty())
        return envDevice;

    std::string envSerial = getEnv("TACCL_SERIAL");
    if (!envSerial.empty()) {
        for (const auto& dev : enumerateDevices()) {
            if (dev.serial == envSerial)
                return dev.port;
        }
    }

    return {};
}

// ---------------------------------------------------------------------------
// Device RAII wrapper
// ---------------------------------------------------------------------------

Device::Device(const std::string& portName)
    : port_(portName)
{
    // OpenHandleByDescription takes non-const char*; copy for safety.
    std::string mutablePort = portName;
    handle_ = OpenHandleByDescription(mutablePort.data());
}

Device::~Device()
{
    close();
}

Device::Device(Device&& other) noexcept
    : handle_(other.handle_), port_(std::move(other.port_))
{
    other.handle_ = kBadHandle;
}

Device& Device::operator=(Device&& other) noexcept
{
    if (this != &other) {
        close();
        handle_ = other.handle_;
        port_   = std::move(other.port_);
        other.handle_ = kBadHandle;
    }
    return *this;
}

void Device::close()
{
    if (handle_ != kBadHandle) {
        CloseTACHandle(handle_);
        handle_ = kBadHandle;
    }
}

} // namespace taccl