// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// RAII wrapper around a TAC_HANDLE plus helpers to resolve a device from
// either a port name or a serial number (with TACCL_DEVICE / TACCL_SERIAL
// environment-variable fallback).

#pragma once

#include <string>
#include <vector>

#include "TACDev.h"

namespace taccl {

struct DeviceEntry {
    std::string port;
    std::string serial;
    std::string raw; // full record returned by GetPortData()
};

// Enumerate all TAC devices currently visible to TACDev.
// Returns empty vector when none present or on failure.
std::vector<DeviceEntry> enumerateDevices();

// Resolve the port name to open, given CLI options and the environment.
// Exactly one of --device / --serial (or TACCL_DEVICE / TACCL_SERIAL) must
// resolve to a device. Returns empty string when nothing is available; the
// caller is responsible for turning that into the appropriate exit code.
std::string resolvePortName(const std::string& deviceOpt,
                            const std::string& serialOpt);

// RAII wrapper over OpenHandleByDescription / CloseTACHandle.
class Device {
public:
    Device() = default;
    explicit Device(const std::string& portName);
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    Device(Device&& other) noexcept;
    Device& operator=(Device&& other) noexcept;

    bool isValid() const { return handle_ != kBadHandle; }
    TAC_HANDLE handle() const { return handle_; }
    const std::string& port() const { return port_; }

    void close();

private:
    TAC_HANDLE  handle_{kBadHandle};
    std::string port_;
};

} // namespace taccl