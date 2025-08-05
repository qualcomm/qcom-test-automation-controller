/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
