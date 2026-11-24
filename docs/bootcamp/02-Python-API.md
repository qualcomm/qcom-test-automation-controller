# QTAC Python API

QTAC offers the TAC API to help you automate the operations that you
can perform using the QTAC TAC Controller. The TAC APIs are offered
for the following programming languages:

- Python
- C#
- C++
- Java

We will discuss in detail about the TAC Python API. If you have not already
installed the library, please refer to
[installing the libraries](./01-Bootcamp.md#installing-the-libraries) section.

To test if the installation is working properly, please refer to the 
[testing the setup](./01-Bootcamp.md#testing-the-setup) section.

## Class reference

**TACDevice** - The class that represents a TAC device. It has the following attributes:

- Port name: The name of the COM port
- Description: The desciption of the TAC device
- Serial number: The serial number of the TAC device

The available methods from the TACDevice object is described in the API reference table.

## API reference

**TACDev** API:

The following methods will be available to you as soon as you import the **TACDev** module using `import TACDev`.

| Function name | Parameters | Return type | Description |
| ------------- | ---------- | ----------- | ----------- |
| AlpacaVersion() | None |  string | It returns the installed version of QTAC |
| TACVersion() | None | string | It returns the installed version of TAC as string |
| GetDeviceCount() | None | integer | It returns the number of connected TAC devices |
| GetDevice(device_index) | device_index: The index of the device (integer) | TACDevice | It returns the TACDevice object for the given device index |

**TACDevice** API:

The following methods will be available to you when you create a **TACDevice** object. Some methods here will be available once the device is opened using
`tac_device.Open()` where the tac_device is an instance of `TACDevice` type.

**Error handling:** methods that set a state (for example `SetBatteryState()`, `Usb0()` or `SetName()`) do not return a value.
If the debug board reports an error, they raise a `RuntimeError` that describes the error. Wrap calls in
`try` / `except RuntimeError` to handle failures. The `Get...()` methods also raise `RuntimeError` on failure.

| Function name | Parameters | Return type | Description |
| ------------- | ---------- | ----------- | ----------- |
| Open() | None | boolean | It returns whether the device is already open and if not opens it |
| Close() | None | None | Closes the TACDevice. TAC operations will not work after device is closed |
| PortName() | None | string | It returns the port name of the TAC device |
| SerialNumber() | None | string | It returns the serial number of the TAC device |
| Description() | None | string | It returns the description of the TAC device |
| Valid() | None | boolean | It returns whether the TAC device is correctly initialized and valid |
| Get_Name() | None | string | It returns the name of the debug board |
| GetHardware() | None | string | It returns the type of debug board hardware |
| Get_HardwareVersion() | None | string | It returns the debug board's hardware version |
| Get_UUID() | None | string | It returns the UUID of the debug board |
| SetExternalPowerControl(state) | state: the desired state of external power control | integer | It sets the state of the external power control and returns 0 if operation is executed successfully |
| GetCommandCount() | None | integer | It returns the total number of available commands in the current platform. |
| GetCommand(commandIndex) | commandIndex: index of the command as seen from the GetHelpText() API. Index is in integer | string | It returns a string containing the command details at a given index. Index is a whole number. Maximum value of commandIndex = GetCommandCount() - 1. |
| GetQuickCommandCount() | None | integer | It returns the total number of available quick settings command in the current platform. |
| GetQuickCommand(commandIndex) | commandIndex: index of the quick settings command as seen from the GetHelpText() API. Index is in integer | string | It returns a string containing the quick settings command details at a given index. Index is a whole number. Maximum value of commandIndex = GetQuickCommandCount() - 1. |
| GetCommandState(command) | command: the string representation of the command as seen from the GetHelpText() API | boolean | It returns the current state of the command. The `state` is a boolean value while the `command` string should be among the listed command in the GetHelpText() API. |
| SendCommand(command, state) | command: the string representation of the command as seen from the GetHelpText() API, state: boolean value representing the expected state of the GPIO pin for the corresponding command. | None | It updates the state of the command for the current platform with the provided parameter `state`. The provided command should be listed as a command in the GetHelpText() API and the state is a boolean (True/False). |
| GetHelpText() | None | string | It returns the name of the current platform configuration along with the available list of commands for the platform. |
| SetPin(pin, state) | pin: the GPIO pin number to update, state: the expected state of the pin (high for True) | None | It updates the state of any GPIO pin with the expected state provided as parameter. Raises `RuntimeError` on failure. Not recommended for use unless you know what you're doing. Use the SendCommand() instead. |
| IsCommandQueueClear() | None | boolean | It returns true when all commands sent to the debug board have been executed |
| SetBatteryState(state) | state: the boolean state of the battery | None | It sets the state of the battery power. Raises `RuntimeError` on failure. It helps in resetting the device by disconnecting power |
| GetBatteryState() | None | boolean | It returns the battery state according to debug board. |
| Usb0(state) | state: The boolean state | None | It sets the state of USB 0 (on or off) according to the `state` parameter. Raises `RuntimeError` on failure |
| GetUsb0State() | None | boolean | It returns the state of the USB 0 |
| Usb1(state) | state: The boolean state | None | It sets the state of USB 1 (on or off) according to the `state` parameter. Raises `RuntimeError` on failure |
| GetUsb1State() | None | boolean | It returns the state of the USB 1 |
| PowerKey(state) | state: the boolean state of the power key | None | It sets the state of power key according to the `state` parameter. Raises `RuntimeError` on failure |
| GetPowerKeyState() | None | boolean | It returns the current state of power key according to the debug board |
| VolumeUp(state) | state: the boolean state of the volume up key | None | It sets the state of the volume up key. Raises `RuntimeError` on failure |
| GetVolumeUpState() | None | boolean | It returns the state of the volume up key according to the debug board |
| VolumeDown(state) | state: the boolean state of the volume down key | None | It sets the state of the volume down key. Raises `RuntimeError` on failure |
| GetVolumeDownState() | None | boolean | It returns the state of the volume down key according to the debug board |
| DisconnectUIM1(state) | state: whether to disconnect SIM 1 | None | It sets the disconnect state of SIM 1. Raises `RuntimeError` on failure |
| GetDisconnectUIM1State() | None | boolean | It returns the disconnect state of the SIM 1 |
| DisconnectUIM2(state) | state: whether to disconnect SIM 2 | None | It sets the disconnect state of SIM 2. Raises `RuntimeError` on failure |
| GetDisconnectUIM2State() | None | boolean | It returns the disconnect state of the SIM 2 |
| DisconnectSDCard(state) | state: whether to disconnect the SD card | None | It sets the disconnect state of the SD card. Raises `RuntimeError` on failure |
| GetDisconnectSDCardState() | None | boolean | It returns the disconnect state of the SD card |
| PrimaryEDL(state) | state: the desired state of EDL | None | It sets the device EDL state to the provided state. Raises `RuntimeError` on failure |
| GetPrimaryEDLState() | None | boolean | It returns the device's current EDL state |
| ForcePSHoldHigh(state) | state: whether to force a PSHoldHigh | None | It sets the state of the ForcePSHoldHigh to the provided state. Raises `RuntimeError` on failure |
| GetForcePSHoldHighState() | None | boolean | It returns the state of PS_HOLD force high switch |
| SecondaryEDL(state) | state: whether to switch on the secondary EDL state | None | It sets the secondary EDL state. Raises `RuntimeError` on failure |
| GetSecondaryEDLState() | None | boolean | It returns the state of the secondary EDL state |
| SecondaryPM_RESIN_N(state) | state: whether to toggle the secondary PM_RESIN_N switch | None | It sets the secondary PM_RESIN_N switch. Raises `RuntimeError` on failure |
| GetSecondaryPM_RESIN_NState() | None | boolean | It returns the state of the secondary PM_RESIN_N switch |
| Eud(state) | state: the desired state of EUD | None | It sets the state of EUD. Raises `RuntimeError` on failure |
| GetEUDState() | None | boolean | It returns the current state of the EUD |
| HeadsetDisconnect(state) | state: the desired state of headset disconnect as boolean | None | It sets the headset disconnect state. Raises `RuntimeError` on failure |
| GetHeadsetDisconnectState() | None | boolean | It returns the current state of headset disconnect |
| SetName(new_name) | new_name: sets the new name of the debug board as string | None | It sets the name of the debug board. Raises `RuntimeError` on failure |
| GetResetCount() | None | integer | It returns the count of the debug board resets |
| ClearResetCount() | None | boolean | It clears the count of the debug board resets and returns true. Raises `RuntimeError` on failure |
| PowerOnButton() | None | boolean | It powers on the device (if already powered off) and powers off and then powers on (if initially powered on) and returns true if the operation was executed successfully |
| PowerOffButton() | None | boolean | It powers off the device and returns true if the operation was executed successfully |
| BootToFastBootButton() | None | boolean | It boots the device into fastboot mode and returns true if the operation was executed successfully |
| BootToUEFIMenuButton() | None | boolean | It boots the device into UEFI mode and returns true if the operation was executed successfully |
| BootToEDLButton() | None | boolean | It boots the device into EDL mode  and returns true if the operation was executed successfully |
| BootToSecondaryEDLButton() | None | boolean | It boots the device into secondary EDL mode (if available) and returns true if the operation was executed successfully |
| GetFirmwareVersion() | None | string | It returns the firmware version of the TAC device |
| Set_Button_Assert_Time(secondsToAssert) | secondsToAssert: time in seconds | None | Deprecated. Always raises `RuntimeError`. The button assert time is now set in the platform configuration |
| Get_Button_Assert_Time() | None | None | Deprecated. Always raises `RuntimeError`. The button assert time is now set in the platform configuration |
| GetScriptVariableCount() | None | integer | It returns total number of script variables available in the configuration |
| GetScriptVariable(scriptVariableIndex) | `scriptVariableIndex`: integer representing the index of the script variable | string | It returns a string containing the script variable details for the given index. Maximum value of scriptVariableIndex = GetScriptVariableCount() - 1 |
| UpdateScriptVariableValue(scriptVariable, scriptVariableValue) | `scriptVariable`: the script variable string whose value needs to be updated, `scriptVariableValue`: the string representation of the value of the script variable to be updated | integer | It updates the value of the script variable to the `scriptVariableValue` and returns 0. Raises `RuntimeError` on failure |
