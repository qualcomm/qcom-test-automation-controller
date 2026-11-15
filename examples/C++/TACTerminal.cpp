// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QTAC TACDev C++ API
#include "TACDev.h"

// Standard C++
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

// Tokenize a string by delimiter
std::vector<std::string> splitTokens(const std::string& str, char delim)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// Device information parsed from GetPortData
struct DeviceEntry
{
    int index{0};
    std::string port;
    std::string description;
    std::string serialNumber;
};

// Enumerate all currently connected TAC devices
std::vector<DeviceEntry> queryConnectedDevices()
{
    std::vector<DeviceEntry> devices;
    int count = 0;
    if (GetDeviceCount(&count) != NO_TAC_ERROR || count <= 0)
    {
        return devices;
    }

    char buffer[1024];
    for (int i = 0; i < count; ++i)
    {
        if (GetPortData(i, buffer, sizeof(buffer)) > 0)
        {
            auto tokens = splitTokens(buffer, ';');
            DeviceEntry entry;
            entry.index = i;
            if (tokens.size() > 0) entry.port = tokens[0];
            if (tokens.size() > 1) entry.description = tokens[1];
            if (tokens.size() > 2) entry.serialNumber = tokens[2];
            devices.push_back(entry);
        }
    }

    return devices;
}

// Scoped RAII guard: opens device and guarantees automatic closure on exit
class ScopedTACDevice
{
public:
    explicit ScopedTACDevice(const std::string& identifier)
    {
        _handle = OpenHandleByDescription(identifier.c_str());
    }

    ~ScopedTACDevice()
    {
        if (_handle != kBadHandle)
        {
            CloseTACHandle(_handle);
            _handle = kBadHandle;
        }
    }

    ScopedTACDevice(const ScopedTACDevice&) = delete;
    ScopedTACDevice& operator=(const ScopedTACDevice&) = delete;

    ScopedTACDevice(ScopedTACDevice&& other) noexcept : _handle(other._handle)
    {
        other._handle = kBadHandle;
    }

    ScopedTACDevice& operator=(ScopedTACDevice&& other) noexcept
    {
        if (this != &other)
        {
            if (_handle != kBadHandle)
            {
                CloseTACHandle(_handle);
            }
            _handle = other._handle;
            other._handle = kBadHandle;
        }
        return *this;
    }

    TAC_HANDLE handle() const { return _handle; }
    bool isValid() const { return _handle != kBadHandle; }

private:
    TAC_HANDLE _handle{kBadHandle};
};

void showHelp(const char* progName)
{
    std::cout << "TACTerminal - Qualcomm Test Automation Controller CLI\n\n"
              << "Usage:\n"
              << "  " << progName << " [OPTIONS]\n\n"
              << "Device Selection:\n"
              << "  -d, --device <ID>       Specify target device by COM port (e.g. COM13, /dev/ttyUSB0)\n"
              << "                          or serial number (e.g. TDC1176234, 6102020216202B84).\n"
              << "                          Device is opened automatically for the requested command\n"
              << "                          and safely closed upon completion.\n\n"
              << "Device Discovery:\n"
              << "  -l, --list-devices      List all connected TAC devices.\n"
              << "  -n, --count             Print the count of connected TAC devices.\n\n"
              << "Device Operations:\n"
              << "  --list-commands         List all supported configuration commands and quick commands.\n"
              << "  -c, --command <CMD>     Send a command to the device (e.g. boottoedl, boottofastboot).\n"
              << "  -i, --info              Display device hardware, firmware, and UUID information.\n\n"
              << "General Options:\n"
              << "  -h, --help              Show this help message and exit.\n"
              << "  -v, --version           Show TAC version information and exit.\n\n"
              << "Examples:\n"
              << "  " << progName << " --list-devices\n"
              << "  " << progName << " --count\n"
              << "  " << progName << " -d COM13 -c boottoedl\n"
              << "  " << progName << " -d TDC1176234 --list-commands\n"
              << "  " << progName << " -d COM3 --info\n"
              << "  " << progName << " -c boottofastboot    (auto-targets if only one device is connected)"
              << std::endl;
}

void showVersion()
{
    char buf[256] = {0};
    std::cout << "TACTerminal CLI Application\n";
    if (GetTACVersion(buf, sizeof(buf)) == NO_TAC_ERROR)
    {
        std::cout << "TAC Library Version:  " << buf << "\n";
    }
    if (GetAlpacaVersion(buf, sizeof(buf)) == NO_TAC_ERROR)
    {
        std::cout << "QTAC Version:         " << buf << "\n";
    }
    std::cout << std::endl;
}

int doListDevices()
{
    auto devices = queryConnectedDevices();
    if (devices.empty())
    {
        std::cout << "No connected TAC devices found.\n";
        return 0;
    }

    std::cout << "Connected TAC Devices (" << devices.size() << " found):\n";
    for (const auto& dev : devices)
    {
        std::cout << "  [" << dev.index << "] Port: " << std::left << std::setw(8) << dev.port
                  << " | Serial: " << std::left << std::setw(20) << (dev.serialNumber.empty() ? "(none)" : dev.serialNumber)
                  << " | Description: " << dev.description << "\n";
    }
    return 0;
}

int doCountDevices()
{
    int count = 0;
    GetDeviceCount(&count);
    std::cout << count << "\n";
    return 0;
}

int doDeviceInfo(TAC_HANDLE handle)
{
    char buf[1024] = {0};

    std::cout << "--- TAC Device Information ---\n";
    if (GetName(handle, buf, sizeof(buf)) == NO_TAC_ERROR)
        std::cout << "Device Name:      " << buf << "\n";

    if (GetFirmwareVersion(handle, buf, sizeof(buf)) == NO_TAC_ERROR)
        std::cout << "Firmware Version: " << buf << "\n";

    if (GetHardware(handle, buf, sizeof(buf)) == NO_TAC_ERROR && buf[0] != '\0')
        std::cout << "Hardware:         " << buf << "\n";

    if (GetHardwareVersion(handle, buf, sizeof(buf)) == NO_TAC_ERROR && buf[0] != '\0')
        std::cout << "Hardware Version: " << buf << "\n";

    if (GetUUID(handle, buf, sizeof(buf)) == NO_TAC_ERROR && buf[0] != '\0')
        std::cout << "UUID:             " << buf << "\n";

    return 0;
}

int doListCommands(TAC_HANDLE handle)
{
    char buf[1024] = {0};

    // 1. Supported Configuration Commands
    unsigned long cmdCount = 0;
    if (GetCommandCount(handle, &cmdCount) == NO_TAC_ERROR)
    {
        std::cout << "Supported Configuration Commands (" << cmdCount << "):\n";
        for (unsigned long i = 0; i < cmdCount; ++i)
        {
            if (GetCommand(handle, i, buf, sizeof(buf)) == NO_TAC_ERROR)
            {
                auto tokens = splitTokens(buf, ';');
                std::string cmd = (tokens.size() > 0) ? tokens[0] : "";
                std::string help = (tokens.size() > 1) ? tokens[1] : "";
                std::string pin = (tokens.size() > 2) ? tokens[2] : "";

                std::cout << "  " << std::left << std::setw(28) << cmd << " " << help;
                if (!pin.empty() && pin != "0")
                {
                    std::cout << " (Pin: " << pin << ")";
                }
                std::cout << "\n";
            }
        }
    }

    // 2. Quick Commands
    unsigned long quickCount = 0;
    if (GetQuickCommandCount(handle, &quickCount) == NO_TAC_ERROR && quickCount > 0)
    {
        std::cout << "\nQuick Commands (" << quickCount << "):\n";
        for (unsigned long i = 0; i < quickCount; ++i)
        {
            if (GetQuickCommand(handle, i, buf, sizeof(buf)) == NO_TAC_ERROR)
            {
                auto tokens = splitTokens(buf, ';');
                std::string btnName = (tokens.size() > 0) ? tokens[0] : "";
                std::string cmdName = (tokens.size() > 1) ? tokens[1] : "";
                std::string help = (tokens.size() > 2) ? tokens[2] : "";

                std::cout << "  " << std::left << std::setw(28) << cmdName << " " << help;
                if (!btnName.empty())
                {
                    std::cout << " [" << btnName << "]";
                }
                std::cout << "\n";
            }
        }
    }

    return 0;
}

int doSendCommand(TAC_HANDLE handle, const std::string& command)
{
    std::cout << "Sending command '" << command << "' to TAC device...\n";

    TAC_ERROR err = SendCommand(handle, command.c_str(), true);
    if (err != NO_TAC_ERROR)
    {
        char errBuf[512] = {0};
        GetLastTACError(errBuf, sizeof(errBuf));
        std::cerr << "Error: SendCommand failed: "
                  << (errBuf[0] ? errBuf : ("error code " + std::to_string(err))) << "\n";
        return 1;
    }

    std::cout << "Command '" << command << "' sent successfully. Waiting for completion...\n";

    // Monitor command queue completion with a 10s timeout
    const auto startTime = std::chrono::steady_clock::now();
    bool isClear = false;
    while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - startTime).count() < 10)
    {
        if (IsCommandQueueClear(handle, &isClear) == NO_TAC_ERROR && isClear)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    if (isClear)
    {
        std::cout << "Command queue clear: Execution complete.\n";
    }
    else
    {
        std::cout << "Command dispatched (queue busy or timed out waiting for clear status).\n";
    }

    return 0;
}

// Resolve target device identifier. If omitted, automatically resolves if exactly 1 device is connected.
std::string resolveTargetDevice(const std::string& userDevice)
{
    if (!userDevice.empty())
    {
        return userDevice;
    }

    auto devices = queryConnectedDevices();
    if (devices.empty())
    {
        std::cerr << "Error: No connected TAC devices found.\n";
        return "";
    }

    if (devices.size() == 1)
    {
        std::cout << "[Auto-selected device on " << devices[0].port;
        if (!devices[0].serialNumber.empty())
        {
            std::cout << " (Serial: " << devices[0].serialNumber << ")";
        }
        std::cout << "]\n";
        return devices[0].port;
    }

    std::cerr << "Error: Multiple TAC devices detected (" << devices.size() << ").\n"
              << "Please specify target device using -d <port|serial>:\n";
    for (const auto& dev : devices)
    {
        std::cerr << "  -d " << dev.port << "  (Serial: " << dev.serialNumber << ", " << dev.description << ")\n";
    }
    return "";
}

} // namespace

int main(int argc, char* argv[])
{
    // Initialize TACDev library
    InitializeTACDev();

    if (argc < 2)
    {
        showHelp(argv[0]);
        return 0;
    }

    std::string deviceId;
    std::string command;
    bool actionListDevices = false;
    bool actionCountDevices = false;
    bool actionListCommands = false;
    bool actionInfo = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            showHelp(argv[0]);
            return 0;
        }
        else if (arg == "-v" || arg == "--version")
        {
            showVersion();
            return 0;
        }
        else if (arg == "-l" || arg == "--list-devices" || arg == "--list")
        {
            actionListDevices = true;
        }
        else if (arg == "-n" || arg == "--count")
        {
            actionCountDevices = true;
        }
        else if (arg == "--list-commands")
        {
            actionListCommands = true;
        }
        else if (arg == "-i" || arg == "--info")
        {
            actionInfo = true;
        }
        else if (arg == "-d" || arg == "--device")
        {
            if (i + 1 < argc)
            {
                deviceId = argv[++i];
            }
            else
            {
                std::cerr << "Error: " << arg << " requires a device argument (COM port or serial number).\n";
                return 1;
            }
        }
        else if (arg == "-c" || arg == "--command")
        {
            if (i + 1 < argc)
            {
                command = argv[++i];
            }
            else
            {
                std::cerr << "Error: " << arg << " requires a command string.\n";
                return 1;
            }
        }
        else
        {
            std::cerr << "Error: Unrecognized option '" << arg << "'. Use --help for usage.\n";
            return 1;
        }
    }

    // Handle discovery options that don't need a specific device opened
    if (actionListDevices)
    {
        return doListDevices();
    }

    if (actionCountDevices)
    {
        return doCountDevices();
    }

    // Operations that require opening a device:
    // Device is opened via RAII ScopedTACDevice and automatically closed on exit.
    if (actionListCommands || actionInfo || !command.empty())
    {
        std::string target = resolveTargetDevice(deviceId);
        if (target.empty())
        {
            return 1;
        }

        ScopedTACDevice device(target);
        if (!device.isValid())
        {
            char errBuf[512] = {0};
            GetLastTACError(errBuf, sizeof(errBuf));
            std::cerr << "Error: Failed to open device '" << target << "': "
                      << (errBuf[0] ? errBuf : "Device not accessible") << "\n";
            return 1;
        }

        // Give device driver a moment to establish state
        std::this_thread::sleep_for(std::chrono::milliseconds(250));

        int result = 0;

        if (actionInfo)
        {
            result = doDeviceInfo(device.handle());
        }

        if (actionListCommands)
        {
            result = doListCommands(device.handle());
        }

        if (!command.empty())
        {
            result = doSendCommand(device.handle(), command);
        }

        return result;
    }

    showHelp(argv[0]);
    return 0;
}
