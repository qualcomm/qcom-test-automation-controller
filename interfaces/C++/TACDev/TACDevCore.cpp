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
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Anmol Jaiswal (anmojais@qti.qualcomm.com)
*/

#include "TACDevCore.h"
#include "AlpacaDefines.h"

// QCommon
#include "QCommonConsole.h"
#include "RangedContainer.h"

bool DevTacCore::initialize(const QByteArray &appName, const QByteArray &appVersion)
{
	QCoreApplication::setApplicationName(kProductName);
	QCoreApplication::setOrganizationName("Qualcomm, Inc.");

	if (_initialized == false)
	{
		AppCore* appCore = DevTacCore::appCore();

		_preferences.setAppName(appName, appVersion);
		appCore->setPreferences(&_preferences);

		InitializeQCommonConsole();

		_initialized = true;
	}

	return _initialized;
}

void DevTacCore::setLoggingState(bool state)
{
	AppCore* appCore = DevTacCore::appCore();
	if (appCore != Q_NULLPTR)
	{
		_preferences.setLoggingActive(state);
		_preferences.saveLoggingActive(state);
	}
}

bool DevTacCore::getLoggingState()
{
	bool result{false};

	AppCore* appCore = DevTacCore::appCore();
	if (appCore != Q_NULLPTR)
	{
		result = _preferences.loggingActive();
	}

	return result;
}

AppCore *DevTacCore::appCore()
{
	return AppCore::getAppCore();
}

AlpacaDevice DevTacCore::getAlpacaDevice(TAC_HANDLE tacHandle)
{
	AlpacaDevice alpacaDevice;

	if (licenseIsValid())
	{
		if (_openDevices.find(tacHandle) != _openDevices.end())
		{
			alpacaDevice = _openDevices[tacHandle];

			if (alpacaDevice != Q_NULLPTR)
				setLastError(alpacaDevice->getLastError());
		}
		else
		{
			setLastError("Bad TAC Handle");
		}
	}
	else
	{
		setLastError("Invalid License");
	}

	return alpacaDevice;
}

TAC_HANDLE DevTacCore::OpenHandleByDescription(const char *portName)
{
	TAC_HANDLE result{kBadHandle};

	if (licenseIsValid() == true)
	{
		for (auto [tacHandle, openDevice]: RangedContainer(_openDevices))
		{
			if (openDevice->portName() == portName)
				return tacHandle;

			if (openDevice->serialNumber().contains(portName))
				return tacHandle;
		}

		AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(portName);
		if (alpacaDevice.isNull() == false)
		{
			connect(alpacaDevice.data(), &_AlpacaDevice::errorEvent, this, &DevTacCore::onErrorEvent);

			if (alpacaDevice->open() == true)
			{
				result = alpacaDevice->hash();
				_openDevices[result] =  alpacaDevice;
			}
			else
			{
				QByteArray lastError = alpacaDevice->getLastError();
				if (lastError.isEmpty())
					setLastError(QByteArray(portName) + " can't be opened.");
				else
					_lastError = lastError;

				alpacaDevice->close();
			}
		}
		else
		{
			setLastError(QByteArray(portName) + " can't be opened.");
		}
	}
	else
	{
		setLastError("Invalid License");
	}

	return result;
}

TAC_RESULT DevTacCore::CloseTACHandle(TAC_HANDLE tacHandle)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = getAlpacaDevice(tacHandle);
	if (alpacaDevice.isNull() == false)
	{
		alpacaDevice->close();
		_openDevices.remove(tacHandle);
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
	}

	return result;
}

void DevTacCore::onErrorEvent(const QByteArray &message)
{
	setLastError(message);
}
