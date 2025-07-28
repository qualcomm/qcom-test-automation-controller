// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "TACDev.h"
#include "TACDevCore.h"

// libTAC
#include "TACDefines.h"
#include "AlpacaDevice.h"
#include "TacException.h"

// QCommon
#include "mymemcpy.h"
#include "version.h"

// QT
#include <QMap>
#include <QThread>

DevTacCore gDevTacCore;

const QByteArray kTACDevHandleNotOpen("TAC device is not open. Please reopen the TAC device");
const QByteArray kTACDevBufferTooSmall("TACDev buffer is too small");
const QByteArray kTACBadIndex("User provided invalid index");


TAC_RESULT InitializeTACDev()
{
	TAC_RESULT result{NO_TAC_ERROR};
	static bool initialized{false};

	if (initialized == false)
	{
		if (gDevTacCore.initialize(kAppName.toLatin1(), kAppVersion.toLatin1()) == false)
			result = TACDEV_INIT_FAILED;
		else
			initialized = true;
	}

	return result;
}

TAC_RESULT _getCommandState
(
	TAC_HANDLE tacHandle,
	const QByteArray& command,
	bool *state
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		try
		{
			*state = alpacaDevice->getCommandState(command);
		}
		catch (const TacException& e)
		{
			result = e.errorCode();
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT _setCommandState
(
	TAC_HANDLE tacHandle,
	const QByteArray& command,
	bool state
)
{
	TAC_RESULT result{NO_TAC_ERROR};
	bool valid{false};

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		try
		{
			alpacaDevice->setWaitForCompletion();
			valid = alpacaDevice->sendCommand(command, state);

			if (valid == false)
			{
				result = TACDEV_BAD_TAC_HANDLE;
				gDevTacCore.setLastError(kTACDevHandleNotOpen);
			}
		}
		catch (const TacException& e)
		{
			result = e.errorCode();
			gDevTacCore.setLastError(e.getMessage());
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT _quickCommand
(
	TAC_HANDLE tacHandle,
	const QByteArray& command
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		try
		{
			alpacaDevice->quickCommand(command);
		}
		catch (const TacException& e)
		{
			result = e.errorCode();
			gDevTacCore.setLastError(e.getMessage());
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	QByteArray version{ALPACA_VERSION};

	if (version.size() < bufferSize)
	{
		my_memcpy(alpacaVersion, bufferSize, version.data(), version.size());
	}
	else
	{
		result = version.size();
	}

	return result;
}

TAC_RESULT GetTACVersion(char* tacVersion, int bufferSize)
{
	TAC_RESULT result{NO_TAC_ERROR};

	QByteArray version{TAC_VERSION};

	if (version.size() < bufferSize)
	{
		my_memcpy(tacVersion, bufferSize, version.data(), version.size());
	}
	else
	{
		result = version.size();
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

	QByteArray lastErrorBA = gDevTacCore.lastError();
	if (lastErrorBA.size() < bufferSize)
	{
		my_memcpy(lastError, bufferSize, lastErrorBA.data(), lastErrorBA.size());
	}
	else
	{
		result = TACDEV_BUFFER_TOO_SMALL;
		gDevTacCore.setLastError(kTACDevBufferTooSmall);
	}

	return result;
}

TAC_RESULT GetLoggingState
(
	bool* loggingState
)
{
	*loggingState = gDevTacCore.getLoggingState();

	return NO_TAC_ERROR;
}

TAC_RESULT SetLoggingState(bool loggingState)
{
	gDevTacCore.setLoggingState(loggingState);

	return NO_TAC_ERROR;
}

unsigned long GetDeviceCount
(
	int* deviceCount
)
{
	InitializeTACDev();

	return gDevTacCore.GetDeviceCount(deviceCount);
}

unsigned long GetPortData
(
	int deviceIndex,
	char* portData,
	int bufferSize
)
{
	int result{0};

	const AlpacaDevices& alpacaDevices = gDevTacCore.GetAlpacaDevices();
	if (deviceIndex < alpacaDevices.count())
	{
		AlpacaDevice alpacaDevice =  alpacaDevices.at(deviceIndex);
		QByteArray portData2;

		portData2 = alpacaDevice->portName();
		portData2 += ";";
		portData2 += alpacaDevice->description();
		portData2 += ";";
		portData2 += alpacaDevice->serialNumber();
		portData2 += ";";
		portData2 += QByteArray::number(deviceIndex);

		my_memcpy(portData, bufferSize, portData2.constData(), portData2.size());
		result = portData2.size();
	}

	return result;
}

TAC_HANDLE OpenHandleByDescription
(
	const char* portName
)
{
	return gDevTacCore.OpenHandleByDescription(portName);
}

TAC_RESULT CloseTACHandle
(
	TAC_HANDLE tacHandle
)
{
	return gDevTacCore.CloseTACHandle(tacHandle);
}

TAC_RESULT GetName
(
	TAC_HANDLE tacHandle,
	char* deviceName,
	int bufferSize
)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		QByteArray name = alpacaDevice->name();
		if (name.size() >= bufferSize)
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTacCore.setLastError(kTACDevBufferTooSmall);
		}
		else
			my_memcpy(deviceName, bufferSize, name, name.size());
	}
	else
	{
		my_memcpy(deviceName, bufferSize, "\0", 1);

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		QByteArray firmwareVer = alpacaDevice->firmwareVersion().toLatin1();
		if (firmwareVer.size() >= bufferSize)
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTacCore.setLastError(kTACDevBufferTooSmall);
		}
		else
			my_memcpy(firmwareVersion, bufferSize, firmwareVer.data(), firmwareVer.size());
	}
	else
	{
		my_memcpy(firmwareVersion, bufferSize, "\0", 1);

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		QByteArray hardwareType = alpacaDevice->debugBoardTypeString().toLatin1();
		if (hardwareType.size() >= bufferSize)
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTacCore.setLastError(kTACDevBufferTooSmall);
		}
		else
			my_memcpy(hardware, bufferSize, hardwareType.data(), hardwareType.size());
	}
	else
	{
		my_memcpy(hardware, bufferSize, "\0", 1);

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		QByteArray hardwareVersionStr = alpacaDevice->hardwareVersionString().toLatin1();
		if (hardwareVersionStr.size() >= bufferSize)
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTacCore.setLastError(kTACDevBufferTooSmall);
		}
		else
			my_memcpy(hardwareVersion, bufferSize, hardwareVersionStr.data(), hardwareVersionStr.size());
	}
	else
	{
		my_memcpy(hardwareVersion, bufferSize, "\0", 1);
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		QByteArray uuidStr = alpacaDevice->uuid().toLatin1();
		if (uuidStr.size() >= bufferSize)
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTacCore.setLastError(kTACDevBufferTooSmall);
		}
		else
			my_memcpy(uuid, bufferSize, uuidStr, uuidStr.size());
	}
	else
	{
		my_memcpy(uuid, bufferSize, "\0", 1);
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		alpacaDevice->externalPowerControl(state);
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT SetBatteryState
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "battery", state);
}

TAC_RESULT GetBatteryState(TAC_HANDLE tacHandle, bool *state)
{
	return _getCommandState(tacHandle, "battery", state);
}

TAC_RESULT Usb0
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "usb0", state);
}

TAC_RESULT GetUsb0State
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "usb0", state);
}

TAC_RESULT Usb1
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "usb1", state);
}

TAC_RESULT GetUsb1State
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "usb1", state);
}

TAC_RESULT PowerKey
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "pkey", state);
}

TAC_RESULT GetPowerKeyState
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "pkey", state);
}

TAC_RESULT VolumeUp
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "volup", state);
}

TAC_RESULT GetVolumeUpState
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "volup", state);
}

TAC_RESULT VolumeDown
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "voldn", state);
}

TAC_RESULT GetVolumeDownState
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "voldn", state);
}

TAC_RESULT DisconnectUIM1
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "uim1", state);
}

TAC_RESULT GetDisconnectUIM1State
(
	TAC_HANDLE tacHandle,
	bool* state
)
{	
	return _getCommandState(tacHandle, "uim1", state);
}

TAC_RESULT DisconnectUIM2
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "uim2", state);
}

TAC_RESULT GetDisconnectUIM2State
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "uim2", state);
}

TAC_RESULT DisconnectSDCard
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "sdcard", state);
}

TAC_RESULT GetDisconnectSDCardState
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "sdcard", state);
}

TAC_RESULT PrimaryEDL
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "pedl", state);
}

TAC_RESULT GetPrimaryEDLState
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "pedl", state);
}

TAC_RESULT ForcePSHoldHigh
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "pshold", state);
}

TAC_RESULT GetForcePSHoldHighState
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "pshold", state);
}

TAC_RESULT SecondaryEDL
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "sedl", state);
}

TAC_RESULT GetSecondaryEDLState
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "sedl", state);
}

TAC_RESULT SecondaryPmResinN
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "sresn", state);
}

TAC_RESULT GetSecondaryPmResinNState
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "sresn", state);
}

TAC_RESULT Eud
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "eud", state);
}

TAC_RESULT GetEUDState
(
	TAC_HANDLE tacHandle,
	bool* state
)
{
	return _getCommandState(tacHandle, "eud", state);
}

TAC_RESULT HeadsetDisconnect
(
	TAC_HANDLE tacHandle,
	bool state
)
{
	return _setCommandState(tacHandle, "headset", state);
}

TAC_RESULT GetHeadsetDisconnectState
(
	TAC_HANDLE tacHandle,
	bool* state
)
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		alpacaDevice->setWaitForCompletion();
		alpacaDevice->setName(QByteArray(newName));
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		alpacaDevice->setWaitForCompletion();
		*resetCount = alpacaDevice->getResetCount();
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT ClearResetCount(TAC_HANDLE tacHandle)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		alpacaDevice->setWaitForCompletion();
		alpacaDevice->clearResetCount();
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_RESULT PowerOnButton
(
	TAC_HANDLE tacHandle
)
{
	return _quickCommand(tacHandle, "powerOn");
}

TAC_RESULT PowerOffButton
(
	TAC_HANDLE tacHandle
)
{
	return _quickCommand(tacHandle, "powerOff");
}

TAC_RESULT BootToFastBootButton
(
	TAC_HANDLE tacHandle
)
{
	return _quickCommand(tacHandle, "bootToFastboot");
}

TAC_RESULT BootToUEFIMenuButton
(
	TAC_HANDLE tacHandle
)
{
	return _quickCommand(tacHandle, "bootToUEFI");
}

TAC_RESULT BootToEDLButton
(
	TAC_HANDLE tacHandle
)
{
	return _quickCommand(tacHandle, "bootToEDL");
}

TAC_RESULT BootToSecondaryEDLButton
(
	TAC_HANDLE tacHandle
)
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		alpacaDevice->setWaitForCompletion();
		alpacaDevice->setPinState(pin, state);
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	*commandCount = 0;
	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		*commandCount = alpacaDevice->commandCount();
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		TacCommand commandEntry = alpacaDevice->commandEntry(commandIndex);
		if (commandEntry._pin != 0)
		{
			QByteArray commandData;

			commandData = commandEntry._command.toLatin1();
			commandData += ";";
			commandData += commandEntry._helpText.toLatin1();
			commandData += ";";
			commandData += QByteArray::number(commandEntry._pin);
			commandData += ";";
			commandData += commandEntry._tabName;
			commandData += ";";
			commandData += commandEntry._groupName;
			commandData += ";";
			commandData += commandEntry._cellLocation;

			my_memcpy(commandBuffer, bufferSize, commandData.constData(), commandData.size());
		}
		else
		{
			result = TAC_BAD_INDEX;
			gDevTacCore.setLastError(kTACBadIndex);
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	*commandCount = 0;
	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		*commandCount = alpacaDevice->quickCommandCount();
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		QByteArray commandData = alpacaDevice->getQuickCommand(commandIndex);
		if (commandData.isEmpty() == false)
		{
			my_memcpy(commandBuffer, bufferSize, commandData.constData(), commandData.size());
		}
		else
		{
			result = TAC_BAD_INDEX;
			gDevTacCore.setLastError(kTACBadIndex);
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		try
		{
			*state = alpacaDevice->getCommandState(command);
		}
		catch (const TacException& e)
		{
			Q_UNUSED(e)

			result = TACDEV_COMMAND_NOT_FOUND;
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		bool valid;

		try
		{
			valid = alpacaDevice->sendCommand(command, state);
			if (valid == false)
			{
				result = TACDEV_BAD_TAC_HANDLE; // DriveThread is NULL
				gDevTacCore.setLastError(kTACDevHandleNotOpen);
			}
		}
		catch (TacException& e)
		{
			result = e.errorCode();
			gDevTacCore.setLastError(e.getMessage());
		}
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
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

	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		// todo
		QByteArray helpText = alpacaDevice->getHelp();
		if (helpText.size() >= bufferSize)
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTacCore.setLastError(kTACDevBufferTooSmall);
			*actualSize = helpText.size();
		}
		else
			my_memcpy(helpBuffer, bufferSize, helpText, helpText.size());
	}
	else
	{
		my_memcpy(helpBuffer, bufferSize, "\0", 1);
		*actualSize = 0;

		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}


TAC_ERROR GetScriptVariableCount(TAC_HANDLE tacHandle, unsigned long *scriptVariableCount)
{
	TAC_RESULT result{NO_TAC_ERROR};
	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);

	if (alpacaDevice.isNull() == false)
		*scriptVariableCount = alpacaDevice->scriptVariableCount();
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
	}

	return result;
}

TAC_ERROR GetScriptVariable(TAC_HANDLE tacHandle, unsigned long scriptVariableIndex, char *scriptVariableBuffer, int bufferSize)
{
	TAC_RESULT result{NO_TAC_ERROR};
	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);

	if (alpacaDevice.isNull() == false)
	{
		QByteArray variableData = alpacaDevice->getScriptVariable(scriptVariableIndex);
		if (variableData.size() >= bufferSize)
		{
			result = TACDEV_BUFFER_TOO_SMALL;
			gDevTacCore.setLastError(kTACDevBufferTooSmall);
		}
		else
			my_memcpy(scriptVariableBuffer, bufferSize, variableData, variableData.size());
	}
	else
	{
		my_memcpy(scriptVariableBuffer, bufferSize, "\0", 1);
		result = TACDEV_BAD_TAC_HANDLE;
		gDevTacCore.setLastError(kTACDevHandleNotOpen);
	}
	return result;
}

TAC_ERROR UpdateScriptVariableValue(TAC_HANDLE tacHandle, const char *scriptVariable, const char* scriptVariableValue)
{
	TAC_RESULT result{NO_TAC_ERROR};
	AlpacaDevice alpacaDevice = gDevTacCore.getAlpacaDevice(tacHandle);

	try
	{
		if (alpacaDevice.isNull() == false)
		{
			alpacaDevice->updateScriptVariableValue(scriptVariable, scriptVariableValue);
		}
		else
		{
			result = TACDEV_BAD_TAC_HANDLE;
			gDevTacCore.setLastError(kTACDevHandleNotOpen);
		}
	}
	catch (TacException& e)
	{
		result = e.errorCode();
		gDevTacCore.setLastError(e.getMessage());
	}

	return result;
}
