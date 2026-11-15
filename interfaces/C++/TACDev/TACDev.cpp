// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACDev.h"
#include "TACDevCore.h"

#include "TACDefines.h"
#include "AlpacaDefines.h"
#include "AlpacaDevice.h"
#include "TACException.h"
#include "StringUtilities.h"

// QCommon
#include "mymemcpy.h"
#include "version.h"

#include <chrono>
#include <string>
#include <thread>

DevTACCore gDevTACCore;

const std::string kTACDevHandleNotOpen("TAC device is not open. Please reopen the TAC device");
const std::string kTACDevBufferTooSmall("TACDev buffer is too small");
const std::string kTACBadIndex("User provided invalid index");

TAC_RESULT InitializeTACDev()
{
	TAC_RESULT result{NO_TAC_ERROR};
	static bool initialized{false};

	if (initialized == false)
	{
		if (gDevTACCore.initialize(kAppName, kProductVersion) == false)
			result = TACDEV_INIT_FAILED;
		else
			initialized = true;
	}

	return result;
}

static TAC_RESULT _getCommandState
(
	TAC_HANDLE tacHandle,
	const std::string& command,
	bool *state
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		try
		{
			*state = alpacaDevice->getCommandState(command);
		}
		catch (const TACException& e)
		{
			result = e.errorCode();
			gDevTACCore.setLastError(e.getMessage());
			AppCore::writeToApplicationLogLine("TACDev::_getCommandState failed for '" + command + "': " + e.getMessage() + " (code " + std::to_string(result) + ")");
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

static TAC_RESULT _setCommandState
(
	TAC_HANDLE tacHandle,
	const std::string& command,
	bool state
)
{
	TAC_RESULT result{NO_TAC_ERROR};
	bool valid{false};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		try
		{
			alpacaDevice->setWaitForCompletion();
			valid = alpacaDevice->sendCommand(command, state);

			if (valid == false)
			{
				result = TACDEV_BAD_TAC_HANDLE;
				gDevTACCore.setLastError(kTACDevHandleNotOpen);
				AppCore::writeToApplicationLogLine("TACDev::_setCommandState: device handle not open for '" + command + "'");
			}
		}
		catch (const TACException& e)
		{
			result = e.errorCode();
			gDevTACCore.setLastError(e.getMessage());
			AppCore::writeToApplicationLogLine("TACDev::_setCommandState failed for '" + command + "': " + e.getMessage() + " (code " + std::to_string(result) + ")");
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

static TAC_RESULT _quickCommand
(
	TAC_HANDLE tacHandle,
	const std::string& command
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		try
		{
			alpacaDevice->quickCommand(command);
		}
		catch (const TACException& e)
		{
			result = e.errorCode();
			gDevTACCore.setLastError(e.getMessage());
			AppCore::writeToApplicationLogLine("TACDev::_quickCommand failed for '" + command + "': " + e.getMessage() + " (code " + std::to_string(result) + ")");
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT GetAlpacaVersion
(
	char* alpacaVersion,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};
	std::string version{QTAC_VERSION};

	if (!copyStringToBuffer(version, alpacaVersion, bufferSize))
	{
		result = static_cast<TAC_RESULT>(version.size() + 1);
	}

	return result;
}

TAC_RESULT GetTACVersion(char* tacVersion, int bufferSize)
{
	TAC_RESULT result{NO_TAC_ERROR};
	std::string version{QTAC_VERSION};

	if (!copyStringToBuffer(version, tacVersion, bufferSize))
	{
		result = static_cast<TAC_RESULT>(version.size() + 1);
	}

	return result;
}

TAC_RESULT GetLastTACError
(
	char* lastError,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};
	std::string lastErr = gDevTACCore.lastError();

	if (!copyStringToBuffer(lastErr, lastError, bufferSize))
	{
		result = TACDEV_BUFFER_TOO_SMALL;
		gDevTACCore.setLastError(kTACDevBufferTooSmall);
	}

	return result;
}

TAC_RESULT GetLoggingState
(
	bool* loggingState
)
{
	if (loggingState)
		*loggingState = gDevTACCore.getLoggingState();

	return NO_TAC_ERROR;
}

TAC_RESULT SetLoggingState(bool loggingState)
{
	gDevTACCore.setLoggingState(loggingState);

	return NO_TAC_ERROR;
}

unsigned long GetDeviceCount
(
	int* deviceCount
)
{
	InitializeTACDev();

	return gDevTACCore.GetDeviceCount(deviceCount);
}

unsigned long GetPortData
(
	int deviceIndex,
	char* portData,
	int bufferSize
)
{
	int result{0};

	const AlpacaDevices& alpacaDevices = gDevTACCore.GetAlpacaDevices();
	if (deviceIndex >= 0 && deviceIndex < static_cast<int>(alpacaDevices.size()))
	{
		AlpacaDevice alpacaDevice = alpacaDevices.at(deviceIndex);
		std::string portData2 = alpacaDevice->portName() + ";" +
								alpacaDevice->description() + ";" +
								alpacaDevice->serialNumber() + ";" +
								std::to_string(deviceIndex);

		if (copyStringToBuffer(portData2, portData, bufferSize))
		{
			result = static_cast<int>(portData2.size());
		}
	}

	return result;
}

TAC_HANDLE OpenHandleByDescription
(
	const char* portName
)
{
	return gDevTACCore.OpenHandleByDescription(portName);
}

TAC_RESULT CloseTACHandle
(
	TAC_HANDLE tacHandle
)
{
	return gDevTACCore.CloseTACHandle(tacHandle);
}

TAC_RESULT GetName
(
	TAC_HANDLE tacHandle,
	char* deviceName,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		std::string name = alpacaDevice->name();
		if (!copyStringToBuffer(name, deviceName, bufferSize))
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTACCore.setLastError(kTACDevBufferTooSmall);
		}
	}
	else
	{
		if (deviceName && bufferSize > 0)
			my_memcpy(deviceName, bufferSize, "\0", 1);

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT GetFirmwareVersion
(
	TAC_HANDLE tacHandle,
	char* firmwareVersion,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		std::string firmwareVer = alpacaDevice->firmwareVersion();
		if (!copyStringToBuffer(firmwareVer, firmwareVersion, bufferSize))
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTACCore.setLastError(kTACDevBufferTooSmall);
		}
	}
	else
	{
		if (firmwareVersion && bufferSize > 0)
			my_memcpy(firmwareVersion, bufferSize, "\0", 1);

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT GetHardware
(
	TAC_HANDLE tacHandle,
	char* hardware,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		std::string hardwareType = alpacaDevice->debugBoardTypeString();
		if (!copyStringToBuffer(hardwareType, hardware, bufferSize))
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTACCore.setLastError(kTACDevBufferTooSmall);
		}
	}
	else
	{
		if (hardware && bufferSize > 0)
			my_memcpy(hardware, bufferSize, "\0", 1);

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT GetHardwareVersion
(
	TAC_HANDLE tacHandle,
	char* hardwareVersion,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		std::string hardwareVersionStr = alpacaDevice->hardwareVersionString();
		if (!copyStringToBuffer(hardwareVersionStr, hardwareVersion, bufferSize))
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTACCore.setLastError(kTACDevBufferTooSmall);
		}
	}
	else
	{
		if (hardwareVersion && bufferSize > 0)
			my_memcpy(hardwareVersion, bufferSize, "\0", 1);

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT GetUUID
(
	TAC_HANDLE tacHandle,
	char* uuid,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		std::string uuidStr = alpacaDevice->uuid();
		if (!copyStringToBuffer(uuidStr, uuid, bufferSize))
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTACCore.setLastError(kTACDevBufferTooSmall);
		}
	}
	else
	{
		if (uuid && bufferSize > 0)
			my_memcpy(uuid, bufferSize, "\0", 1);

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR SetExternalPowerControl
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		alpacaDevice->externalPowerControl(state);
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT SetBatteryState(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "battery", state);
}

TAC_RESULT GetBatteryState(TAC_HANDLE tacHandle, bool *state)
{
	return _getCommandState(tacHandle, "battery", state);
}

TAC_RESULT Usb0(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "usb0", state);
}

TAC_RESULT GetUsb0State(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "usb0", state);
}

TAC_RESULT Usb1(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "usb1", state);
}

TAC_RESULT GetUsb1State(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "usb1", state);
}

TAC_RESULT PowerKey(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "pkey", state);
}

TAC_RESULT GetPowerKeyState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "pkey", state);
}

TAC_RESULT VolumeUp(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "volup", state);
}

TAC_RESULT GetVolumeUpState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "volup", state);
}

TAC_RESULT VolumeDown(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "voldn", state);
}

TAC_RESULT GetVolumeDownState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "voldn", state);
}

TAC_RESULT DisconnectUIM1(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "uim1", state);
}

TAC_RESULT GetDisconnectUIM1State(TAC_HANDLE tacHandle, bool* state)
{	
	return _getCommandState(tacHandle, "uim1", state);
}

TAC_RESULT DisconnectUIM2(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "uim2", state);
}

TAC_RESULT GetDisconnectUIM2State(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "uim2", state);
}

TAC_RESULT DisconnectSDCard(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "sdcard", state);
}

TAC_RESULT GetDisconnectSDCardState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "sdcard", state);
}

TAC_RESULT PrimaryEDL(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "pedl", state);
}

TAC_RESULT GetPrimaryEDLState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "pedl", state);
}

TAC_RESULT ForcePSHoldHigh(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "pshold", state);
}

TAC_RESULT GetForcePSHoldHighState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "pshold", state);
}

TAC_RESULT SecondaryEDL(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "sedl", state);
}

TAC_RESULT GetSecondaryEDLState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "sedl", state);
}

TAC_RESULT SecondaryPmResinN(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "sresn", state);
}

TAC_RESULT GetSecondaryPmResinNState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "sresn", state);
}

TAC_RESULT Eud(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "eud", state);
}

TAC_RESULT GetEUDState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "eud", state);
}

TAC_RESULT HeadsetDisconnect(TAC_HANDLE tacHandle, bool state)
{
	return _setCommandState(tacHandle, "headset", state);
}

TAC_RESULT GetHeadsetDisconnectState(TAC_HANDLE tacHandle, bool* state)
{
	return _getCommandState(tacHandle, "headset", state);
}

TAC_RESULT SetName
(
	TAC_HANDLE tacHandle,
	const char* newName
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		alpacaDevice->setWaitForCompletion();
		alpacaDevice->setName(newName ? std::string(newName) : std::string());
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT GetResetCount
(
	TAC_HANDLE tacHandle,
	int* resetCount
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		alpacaDevice->setWaitForCompletion();
		if (resetCount)
			*resetCount = alpacaDevice->getResetCount();
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT ClearResetCount(TAC_HANDLE tacHandle)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		alpacaDevice->setWaitForCompletion();
		alpacaDevice->clearResetCount();
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT PowerOnButton(TAC_HANDLE tacHandle)
{
	return _quickCommand(tacHandle, "powerOn");
}

TAC_RESULT PowerOffButton(TAC_HANDLE tacHandle)
{
	return _quickCommand(tacHandle, "powerOff");
}

TAC_RESULT BootToFastBootButton(TAC_HANDLE tacHandle)
{
	return _quickCommand(tacHandle, "bootToFastboot");
}

TAC_RESULT BootToUEFIMenuButton(TAC_HANDLE tacHandle)
{
	return _quickCommand(tacHandle, "bootToUEFI");
}

TAC_RESULT BootToEDLButton(TAC_HANDLE tacHandle)
{
	return _quickCommand(tacHandle, "bootToEDL");
}

TAC_RESULT BootToSecondaryEDLButton(TAC_HANDLE tacHandle)
{
	return _quickCommand(tacHandle, "bootToSecondaryEDL");
}

TAC_ERROR SetPinState
(
	TAC_HANDLE tacHandle,
	int pin,
	bool state
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		alpacaDevice->setWaitForCompletion();
		alpacaDevice->setPinState(pin, state);
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR GetCommandCount
(
	TAC_HANDLE tacHandle,
	unsigned long* commandCount
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	if (commandCount)
		*commandCount = 0;

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		if (commandCount)
			*commandCount = alpacaDevice->commandCount();
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR GetCommand
(
	TAC_HANDLE tacHandle,
	unsigned long commandIndex,
	char *commandBuffer,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		TACCommand commandEntry = alpacaDevice->commandEntry(commandIndex);
		if (commandEntry._pin != 0)
		{
			std::string commandData = commandEntry._command + ";" +
									  commandEntry._helpText + ";" +
									  std::to_string(commandEntry._pin) + ";" +
									  commandEntry._tabName + ";" +
									  commandEntry._groupName + ";" +
									  commandEntry._cellLocation;

			if (!copyStringToBuffer(commandData, commandBuffer, bufferSize))
			{
				result = TACDEV_BUFFER_TOO_SMALL;
				gDevTACCore.setLastError(kTACDevBufferTooSmall);
			}
		}
		else
		{
			result = TACDEV_BAD_INDEX;
			gDevTACCore.setLastError(kTACBadIndex);
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR GetQuickCommandCount
(
	TAC_HANDLE tacHandle,
	unsigned long* commandCount
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	if (commandCount)
		*commandCount = 0;

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		if (commandCount)
			*commandCount = alpacaDevice->quickCommandCount();
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR GetQuickCommand
(
	TAC_HANDLE tacHandle,
	unsigned long commandIndex,
	char* commandBuffer,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		std::string commandData = alpacaDevice->getQuickCommand(commandIndex);
		if (!commandData.empty())
		{
			if (!copyStringToBuffer(commandData, commandBuffer, bufferSize))
			{
				result = TACDEV_BUFFER_TOO_SMALL;
				gDevTACCore.setLastError(kTACDevBufferTooSmall);
			}
		}
		else
		{
			result = TACDEV_BAD_INDEX;
			gDevTACCore.setLastError(kTACBadIndex);
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR GetCommandState
(
	TAC_HANDLE tacHandle,
	const char *command,
	bool* state
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	if (!command || !state)
		return TACDEV_BAD_INDEX;

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		try
		{
			*state = alpacaDevice->getCommandState(command);
		}
		catch (const TACException& e)
		{
			(void)e;
			result = TACDEV_COMMAND_NOT_FOUND;
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR SendCommand
(
	TAC_HANDLE tacHandle,
	const char* command,
	bool state
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	if (!command)
		return TACDEV_BAD_INDEX;

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		bool valid;

		try
		{
			valid = alpacaDevice->sendCommand(command, state);
			if (valid == false)
			{
				result = TACDEV_BAD_TAC_HANDLE; // DriveThread is NULL
				gDevTACCore.setLastError(kTACDevHandleNotOpen);
			}
		}
		catch (TACException& e)
		{
			result = e.errorCode();
			gDevTACCore.setLastError(e.getMessage());
			AppCore::writeToApplicationLogLine("TACDev::SendCommand failed for '" + std::string(command) + "': " + e.getMessage() + " (code " + std::to_string(result) + ")");
		}
		catch (const std::exception& e)
		{
			result = TACDEV_COMMAND_NOT_FOUND;
			gDevTACCore.setLastError(e.what());
			AppCore::writeToApplicationLogLine("TACDev::SendCommand exception for '" + std::string(command) + "': " + e.what());
		}
		catch (...)
		{
			result = TACDEV_COMMAND_NOT_FOUND;
			gDevTACCore.setLastError("Unknown exception");
			AppCore::writeToApplicationLogLine("TACDev::SendCommand unknown exception for '" + std::string(command) + "'");
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR GetHelpText
(
	TAC_HANDLE tacHandle,
	char* helpBuffer,
	int bufferSize,
	int* actualSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		std::string helpText = alpacaDevice->getHelp();
		if (actualSize)
			*actualSize = static_cast<int>(helpText.size());

		if (!copyStringToBuffer(helpText, helpBuffer, bufferSize))
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTACCore.setLastError(kTACDevBufferTooSmall);
		}
	}
	else
	{
		if (helpBuffer && bufferSize > 0)
			my_memcpy(helpBuffer, bufferSize, "\0", 1);
		if (actualSize)
			*actualSize = 0;

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR GetScriptVariableCount(TAC_HANDLE tacHandle, unsigned long *scriptVariableCount)
{
	TAC_RESULT result{NO_TAC_ERROR};
	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);

	if (alpacaDevice != nullptr)
	{
		if (scriptVariableCount)
			*scriptVariableCount = alpacaDevice->scriptVariableCount();
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR GetScriptVariable(TAC_HANDLE tacHandle, unsigned long scriptVariableIndex, char *scriptVariableBuffer, int bufferSize)
{
	TAC_RESULT result{NO_TAC_ERROR};
	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);

	if (alpacaDevice != nullptr)
	{
		std::string variableData = alpacaDevice->getScriptVariable(scriptVariableIndex);
		if (!copyStringToBuffer(variableData, scriptVariableBuffer, bufferSize))
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTACCore.setLastError(kTACDevBufferTooSmall);
		}
	}
	else
	{
		if (scriptVariableBuffer && bufferSize > 0)
			my_memcpy(scriptVariableBuffer, bufferSize, "\0", 1);

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
	}
	return result;
}

TAC_ERROR UpdateScriptVariableValue(TAC_HANDLE tacHandle, const char *scriptVariable, const char* scriptVariableValue)
{
	TAC_RESULT result{NO_TAC_ERROR};
	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);

	try
	{
		if (alpacaDevice != nullptr)
		{
			alpacaDevice->updateScriptVariableValue(scriptVariable ? scriptVariable : "", scriptVariableValue ? scriptVariableValue : "");
		}
		else
		{
			result = TACDEV_BAD_TAC_HANDLE;
			gDevTACCore.setLastError(kTACDevHandleNotOpen);
		}
	}
	catch (TACException& e)
	{
		result = e.errorCode();
		gDevTACCore.setLastError(e.getMessage());
	}

	return result;
}

TAC_ERROR IsCommandQueueClear(TAC_HANDLE tacHandle, bool* status)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTACCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		try
		{
			// if false, queue is clear
			if (status)
				*status = !alpacaDevice->isCommandQueueClear();

			// We're limited by the PIC32CX firmware.
			// The firmware does not actually send us an acknowledgement for the command execution.
			// It receives the commands and processes them later. Hardcoded delay can help some automation use-cases.
			if (alpacaDevice->debugBoardType() == ePIC32CXAuto)
				std::this_thread::sleep_for(std::chrono::milliseconds(10000));
		}
		catch (TACException& e)
		{
			result = e.errorCode();
			gDevTACCore.setLastError(e.getMessage());
			AppCore::writeToApplicationLogLine("TACDev::_quickCommandWithDelay failed: " + e.getMessage() + " (code " + std::to_string(result) + ")");
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTACCore.setLastError(kTACDevHandleNotOpen);
		AppCore::writeToApplicationLogLine("TACDev::_quickCommandWithDelay failed: " + std::string(kTACDevHandleNotOpen) + " (code " + std::to_string(result) + ")");
	}

	return result;
}
