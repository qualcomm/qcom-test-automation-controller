# QTAC C++ API

QTAC offers the native C++ TAC API (`TACDev`) to help you automate the operations that you
can perform using the QTAC TAC Controller.

This document discusses the **TAC C++ API** in detail. The C++ interface is exposed
as a native C/C++ shared library (`TACDev.dll` on Windows, `libTACDev.so` on Linux)
with standard C linkage functions declared in `TACDev.h`.

---

## Getting Started

### Headers and Libraries

To use the C++ API in your project, include the header file and link against the library:

- **Header file**: `#include "TACDev.h"` (located in `interfaces/C++/TACDev/`)
- **Windows**: Link with `TACDev.lib` and place `TACDev.dll` in your executable path.
- **Linux**: Link with `-lTACDev` and place `libTACDev.so` in your `LD_LIBRARY_PATH`.

### Initialization & Handle Lifecycle

1. **Initialization**: Before invoking any other TACDev function, you **must** call `InitializeTACDev()`.
2. **Device Discovery**: Call `GetDeviceCount()` to detect how many devices are attached, and call `GetPortData()` to query each device's port name, description, and serial number.
3. **Handle Management**: Open a connection to a specific device using `OpenHandleByDescription()`, which returns a `TAC_HANDLE`. When finished, release resources using `CloseTACHandle()`.
4. **Invalid Handle Check**: A failed handle open returns `kBadHandle` (`0`). Always verify that the handle is valid before invoking device-specific functions.

---

## Return Codes & Error Constants

All functions returning `TAC_RESULT` or `TAC_ERROR` return an unsigned long error code:

| Constant | Value | Description |
| :--- | :--- | :--- |
| `NO_TAC_ERROR` | `0` | Operation completed successfully |
| `TACDEV_BUFFER_TOO_SMALL` | `1` | Provided output string buffer is too small |
| `TACDEV_BAD_TAC_HANDLE` | `2` | Specified `TAC_HANDLE` is invalid or already closed |
| `TACDEV_COMMAND_NOT_FOUND` | `3` | Specified command string was not found in the current platform configuration |
| `TACDEV_BAD_INDEX` | `4` | Provided index is out of bounds |
| `TACDEV_INIT_FAILED` | `5` | TACDev initialization failed or library was not initialized |
| `TACDEV_SCRIPT_VARIABLE_NOT_FOUND` | `6` | Specified script variable does not exist |

---

## API Reference

### System & Device Discovery API

These functions operate at the library level and do not require an active `TAC_HANDLE` (except where noted):

| Function name | Parameters | Return type | Description |
| :--- | :--- | :--- | :--- |
| `InitializeTACDev()` | None | `TAC_RESULT` | Initializes QTAC. Must be called before any other TACDev function. |
| `GetAlpacaVersion(alpacaVersion, bufferSize)` | `alpacaVersion`: `char*`, `bufferSize`: `int` | `TAC_RESULT` | Returns the QTAC version into the provided buffer. |
| `GetTACVersion(tacVersion, bufferSize)` | `tacVersion`: `char*`, `bufferSize`: `int` | `TAC_RESULT` | Returns the TAC version into the provided buffer. |
| `GetLastTACError(lastError, bufferSize)` | `lastError`: `char*`, `bufferSize`: `int` | `TAC_RESULT` | Copies the last error message into the provided string buffer. |
| `GetLoggingState(loggingState)` | `loggingState`: `bool*` | `TAC_RESULT` | Retrieves the current logging state. |
| `SetLoggingState(loggingState)` | `loggingState`: `bool` | `TAC_RESULT` | Enables or disables debug logging. |
| `GetDeviceCount(deviceCount)` | `deviceCount`: `int*` | `TAC_RESULT` | Queries connected debug boards and sets `deviceCount` to the number of detected devices. |
| `GetPortData(deviceIndex, portData, bufferSize)` | `deviceIndex`: `int`, `portData`: `char*`, `bufferSize`: `int` | `unsigned long` | Returns a semicolon-delimited string of device metadata (`PortName;Description;SerialNumber`) for the given zero-based device index. |
| `OpenHandleByDescription(portName)` | `portName`: `const char*` | `TAC_HANDLE` | Opens a communication handle to the specified device (by port name, description, or serial number). Returns `kBadHandle` on failure. |
| `CloseTACHandle(tacHandle)` | `tacHandle`: `TAC_HANDLE` | `TAC_RESULT` | Closes the open device handle and releases associated hardware resources. |

---

### Device Control & Query API

These functions require a valid `TAC_HANDLE` obtained from `OpenHandleByDescription()`:

#### Device Identification & Metadata

| Function name | Parameters | Return type | Description |
| :--- | :--- | :--- | :--- |
| `GetName(tacHandle, deviceName, bufferSize)` | `tacHandle`: `TAC_HANDLE`, `deviceName`: `char*`, `bufferSize`: `int` | `TAC_RESULT` | Retrieves the user-assigned or board name of the debug board. |
| `SetName(tacHandle, newName)` | `tacHandle`: `TAC_HANDLE`, `newName`: `const char*` | `TAC_RESULT` | Sets a new friendly name on the debug board. |
| `GetHardware(tacHandle, hardware, bufferSize)` | `tacHandle`: `TAC_HANDLE`, `hardware`: `char*`, `bufferSize`: `int` | `TAC_RESULT` | Retrieves the hardware type of the debug board (e.g. `FTDI`, `PSOC`, `PIC32CX`). |
| `GetHardwareVersion(tacHandle, hardwareVersion, bufferSize)` | `tacHandle`: `TAC_HANDLE`, `hardwareVersion`: `char*`, `bufferSize`: `int` | `TAC_RESULT` | Retrieves the hardware revision version string of the debug board. |
| `GetFirmwareVersion(tacHandle, firmwareVersion, bufferSize)` | `tacHandle`: `TAC_HANDLE`, `firmwareVersion`: `char*`, `bufferSize`: `int` | `TAC_RESULT` | Retrieves the firmware version currently programmed on the debug board. |
| `GetUUID(tacHandle, uuid, bufferSize)` | `tacHandle`: `TAC_HANDLE`, `uuid`: `char*`, `bufferSize`: `int` | `TAC_RESULT` | Retrieves the unique device identifier (UUID) of the debug board. |
| `GetResetCount(tacHandle, resetCount)` | `tacHandle`: `TAC_HANDLE`, `resetCount`: `int*` | `TAC_RESULT` | Retrieves the number of board resets recorded by the controller. |
| `ClearResetCount(tacHandle)` | `tacHandle`: `TAC_HANDLE` | `TAC_RESULT` | Resets the recorded debug board reset counter back to zero. |

#### Power & Key Controls

| Function name | Parameters | Return type | Description |
| :--- | :--- | :--- | :--- |
| `SetBatteryState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Connects (`true`) or disconnects (`false`) battery power to the target device. |
| `GetBatteryState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Retrieves the current battery connection state. |
| `SetExternalPowerControl(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_ERROR` | Configures the external power control relay state. |
| `PowerKey(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Sets the state of the target device power key line (`true` for pressed). |
| `GetPowerKeyState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries whether the power key line is currently asserted. |
| `VolumeUp(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Sets the state of the Volume Up key line (`true` for pressed). |
| `GetVolumeUpState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries whether the Volume Up key line is currently asserted. |
| `VolumeDown(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Sets the state of the Volume Down key line (`true` for pressed). |
| `GetVolumeDownState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries whether the Volume Down key line is currently asserted. |
| `ForcePSHoldHigh(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Forces the `PS_HOLD` line high (`true`) or releases it (`false`). |
| `GetForcePSHoldHighState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the current state of the `PS_HOLD` force switch. |

#### USB & Peripheral Routing

| Function name | Parameters | Return type | Description |
| :--- | :--- | :--- | :--- |
| `Usb0(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Connects (`true`) or disconnects (`false`) USB port 0. |
| `GetUsb0State(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the connection state of USB port 0. |
| `Usb1(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Connects (`true`) or disconnects (`false`) USB port 1. |
| `GetUsb1State(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the connection state of USB port 1. |
| `Eud(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Toggles the Embedded USB Debugger (EUD) line state. |
| `GetEUDState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the current state of the EUD line. |
| `HeadsetDisconnect(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Controls the headset disconnect simulation switch. |
| `GetHeadsetDisconnectState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the headset disconnect line status. |
| `DisconnectUIM1(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Disconnects (`true`) or reconnects (`false`) SIM 1. |
| `GetDisconnectUIM1State(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the disconnect state of SIM 1. |
| `DisconnectUIM2(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Disconnects (`true`) or reconnects (`false`) SIM 2. |
| `GetDisconnectUIM2State(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the disconnect state of SIM 2. |
| `DisconnectSDCard(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Disconnects (`true`) or reconnects (`false`) the SD card. |
| `GetDisconnectSDCardState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the disconnect state of the SD card. |

#### Boot Modes & Automated Actions

| Function name | Parameters | Return type | Description |
| :--- | :--- | :--- | :--- |
| `PowerOnButton(tacHandle)` | `tacHandle`: `TAC_HANDLE` | `TAC_RESULT` | Powers on the target device (or power-cycles if already powered). |
| `PowerOffButton(tacHandle)` | `tacHandle`: `TAC_HANDLE` | `TAC_RESULT` | Powers off the target device cleanly. |
| `BootToFastBootButton(tacHandle)` | `tacHandle`: `TAC_HANDLE` | `TAC_RESULT` | Automates the button/pin sequence to boot the target device into FastBoot mode. |
| `BootToUEFIMenuButton(tacHandle)` | `tacHandle`: `TAC_HANDLE` | `TAC_RESULT` | Automates the button/pin sequence to boot the target device into UEFI menu mode. |
| `BootToEDLButton(tacHandle)` | `tacHandle`: `TAC_HANDLE` | `TAC_RESULT` | Automates the button/pin sequence to boot the target device into Emergency Download (EDL) mode. |
| `BootToSecondaryEDLButton(tacHandle)` | `tacHandle`: `TAC_HANDLE` | `TAC_RESULT` | Boots into secondary EDL mode if supported by the debug board platform. |
| `PrimaryEDL(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Controls the primary EDL hardware signal line directly. |
| `GetPrimaryEDLState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the state of the primary EDL line. |
| `SecondaryEDL(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Controls the secondary EDL hardware signal line directly. |
| `GetSecondaryEDLState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the state of the secondary EDL line. |
| `SecondaryPmResinN(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool` | `TAC_RESULT` | Toggles the secondary `PM_RESIN_N` signal switch. |
| `GetSecondaryPmResinNState(tacHandle, state)` | `tacHandle`: `TAC_HANDLE`, `state`: `bool*` | `TAC_RESULT` | Queries the state of the secondary `PM_RESIN_N` line. |

#### Platform Commands, Script Variables & Raw GPIO

| Function name | Parameters | Return type | Description |
| :--- | :--- | :--- | :--- |
| `GetCommandCount(tacHandle, commandCount)` | `tacHandle`: `TAC_HANDLE`, `commandCount`: `unsigned long*` | `TAC_ERROR` | Returns the total number of available commands defined in the active platform configuration. |
| `GetCommand(tacHandle, commandIndex, commandBuffer, bufferSize)` | `tacHandle`: `TAC_HANDLE`, `commandIndex`: `unsigned long`, `commandBuffer`: `char*`, `bufferSize`: `int` | `TAC_ERROR` | Retrieves the command name for the specified index. |
| `GetQuickCommandCount(tacHandle, commandCount)` | `tacHandle`: `TAC_HANDLE`, `commandCount`: `unsigned long*` | `TAC_ERROR` | Returns the total count of quick setting commands for the active platform. |
| `GetQuickCommand(tacHandle, commandIndex, commandBuffer, bufferSize)` | `tacHandle`: `TAC_HANDLE`, `commandIndex`: `unsigned long`, `commandBuffer`: `char*`, `bufferSize`: `int` | `TAC_ERROR` | Retrieves the quick command name for the specified index. |
| `GetCommandState(tacHandle, command, state)` | `tacHandle`: `TAC_HANDLE`, `command`: `const char*`, `state`: `bool*` | `TAC_ERROR` | Queries the current Boolean state of a named command in the platform configuration. |
| `SendCommand(tacHandle, command, state)` | `tacHandle`: `TAC_HANDLE`, `command`: `const char*`, `state`: `bool` | `TAC_ERROR` | Sends a named command with the desired state (`true`/`false`) to the device. |
| `GetHelpText(tacHandle, helpBuffer, bufferSize, actualSize)` | `tacHandle`: `TAC_HANDLE`, `helpBuffer`: `char*`, `bufferSize`: `int`, `actualSize`: `int*` | `TAC_ERROR` | Returns the platform name and list of supported commands for the device. |
| `SetPinState(tacHandle, pin, state)` | `tacHandle`: `TAC_HANDLE`, `pin`: `int`, `state`: `bool` | `TAC_ERROR` | Directly updates the state of a GPIO pin. (Prefer `SendCommand` for named configurations). |
| `IsCommandQueueClear(tacHandle, status)` | `tacHandle`: `TAC_HANDLE`, `status`: `bool*` | `TAC_ERROR` | Checks whether the command queue is clear. |
| `GetScriptVariableCount(tacHandle, scriptVariableCount)` | `tacHandle`: `TAC_HANDLE`, `scriptVariableCount`: `unsigned long*` | `TAC_ERROR` | Returns the total number of script variables defined in the configuration. |
| `GetScriptVariable(tacHandle, scriptVariableIndex, scriptVariableBuffer, bufferSize)` | `tacHandle`: `TAC_HANDLE`, `scriptVariableIndex`: `unsigned long`, `scriptVariableBuffer`: `char*`, `bufferSize`: `int` | `TAC_ERROR` | Retrieves the script variable details for a given index. |
| `UpdateScriptVariableValue(tacHandle, scriptVariable, scriptVariableValue)` | `tacHandle`: `TAC_HANDLE`, `scriptVariable`: `const char*`, `scriptVariableValue`: `const char*` | `TAC_ERROR` | Updates the value of the specified script variable. |

---

## Code Examples

### Example 1: Device Discovery

Enumerate all connected Qualcomm debug boards and retrieve their port name and serial number:

```cpp
#include "TACDev.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

int main()
{
    // 1. Initialize TACDev
    if (InitializeTACDev() != NO_TAC_ERROR)
    {
        std::cerr << "Failed to initialize TACDev" << std::endl;
        return 1;
    }

    // 2. Query device count
    int deviceCount = 0;
    if (GetDeviceCount(&deviceCount) != NO_TAC_ERROR || deviceCount == 0)
    {
        std::cout << "No TAC devices found." << std::endl;
        return 0;
    }

    std::cout << "Connected TAC devices: " << deviceCount << std::endl;

    // 3. Print port data for each device
    char buffer[1024];
    for (int i = 0; i < deviceCount; ++i)
    {
        if (GetPortData(i, buffer, sizeof(buffer)) > 0)
        {
            // Format is "PortName;Description;SerialNumber"
            std::cout << "Device [" << i << "]: " << buffer << std::endl;
        }
    }

    return 0;
}
```

### Example 2: Device Control with RAII Guard

Open a target device, query hardware info, send a command, and safely close the handle using RAII:

```cpp
#include "TACDev.h"
#include <iostream>
#include <string>

// Scoped RAII guard to ensure handle is closed on all exit paths
class ScopedTAC
{
public:
    explicit ScopedTAC(const std::string& identifier)
        : _handle(OpenHandleByDescription(identifier.c_str())) {}

    ~ScopedTAC()
    {
        if (_handle != kBadHandle)
        {
            CloseTACHandle(_handle);
        }
    }

    TAC_HANDLE handle() const { return _handle; }
    bool isValid() const { return _handle != kBadHandle; }

private:
    TAC_HANDLE _handle{kBadHandle};
};

int main()
{
    InitializeTACDev();

    // Open target device by COM port or serial number
    ScopedTAC device("COM13");
    if (!device.isValid())
    {
        std::cerr << "Failed to open device handle." << std::endl;
        return 1;
    }

    // Query device information
    char hw[256] = {0};
    char fw[256] = {0};
    GetName(device.handle(), hw, sizeof(hw));
    GetFirmwareVersion(device.handle(), fw, sizeof(fw));

    std::cout << "Connected to: " << hw << " (Firmware: " << fw << ")" << std::endl;

    // Boot device into FastBoot mode
    std::cout << "Rebooting device into FastBoot mode..." << std::endl;
    if (BootToFastBootButton(device.handle()) == NO_TAC_ERROR)
    {
        std::cout << "Boot to FastBoot succeeded!" << std::endl;
    }

    // Handle is closed automatically when device goes out of scope
    return 0;
}
```

---

## CMake Integration

To link against `TACDev` in your CMake project:

```cmake
# Add include directory for TACDev.h
target_include_directories(my_app PRIVATE
    ${QTAC_ROOT}/interfaces/C++/TACDev
)

# Link against TACDev import library
target_link_libraries(my_app PRIVATE
    TACDev
)
```
