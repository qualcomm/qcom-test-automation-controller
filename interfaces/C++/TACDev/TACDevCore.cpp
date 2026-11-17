// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACDevCore.h"
#include "AlpacaDefines.h"

// QCommon
#include "QCommonConsole.h"
#include "StringUtilities.h"

bool DevTACCore::initialize(const std::string& appName, const std::string& appVersion)
{
	if (_initialized == false)
	{
		AppCore* appCore = DevTACCore::appCore();

		_preferences.setAppName(appName, appVersion);
		if (appCore != nullptr)
		{
			appCore->setPreferences(&_preferences);
		}

		InitializeQCommonConsole();

		_initialized = true;
	}

	return _initialized;
}

void DevTACCore::setLoggingState(bool state)
{
	AppCore* appCore = DevTACCore::appCore();
	if (appCore != nullptr)
	{
		_preferences.setLoggingActive(state);
		_preferences.saveLoggingActive(state);
		appCore->setAppLogging(state);
	}
}

bool DevTACCore::getLoggingState()
{
	bool result{false};

	AppCore* appCore = DevTACCore::appCore();
	if (appCore != nullptr)
	{
		result = _preferences.loggingActive();
	}

	return result;
}

AppCore* DevTACCore::appCore()
{
	return AppCore::getAppCore();
}

AlpacaDevice DevTACCore::getAlpacaDevice(TAC_HANDLE tacHandle)
{
	AlpacaDevice alpacaDevice;

	auto it = _openDevices.find(tacHandle);
	if (it != _openDevices.end())
	{
		alpacaDevice = it->second;

		if (alpacaDevice != nullptr)
			setLastError(alpacaDevice->getLastError());
	}
	else
	{
		setLastError("Bad TAC Handle");
		AppCore::writeToApplicationLogLine("DevTACCore::getAlpacaDevice: Bad TAC Handle (" + std::to_string(tacHandle) + ")");
	}

	return alpacaDevice;
}

TAC_HANDLE DevTACCore::OpenHandleByDescription(const char* portName)
{
	TAC_HANDLE result{kBadHandle};

	if (portName == nullptr)
	{
		setLastError("Failed to open device: invalid port name (null)");
		return result;
	}

	for (const auto& [tacHandle, openDevice] : _openDevices)
	{
		if (openDevice->portName() == portName)
			return tacHandle;

		if (contains(openDevice->serialNumber(), portName))
			return tacHandle;
	}

	AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(portName);
	if (alpacaDevice == nullptr)
	{
		_AlpacaDevice::updateAlpacaDevices();
		alpacaDevice = _AlpacaDevice::findAlpacaDevice(portName);
	}
	if (alpacaDevice == nullptr)
	{
		alpacaDevice = _AlpacaDevice::findAlpacaDeviceBySerialNumber(portName, true);
		if (alpacaDevice == nullptr)
			alpacaDevice = _AlpacaDevice::findAlpacaDeviceByDescription(portName, true);
	}

	if (alpacaDevice != nullptr)
	{
		alpacaDevice->onError = [this](const std::string& message) {
			onErrorEvent(message);
		};

		if (alpacaDevice->open() == true)
		{
			result = alpacaDevice->hash();
			_openDevices[result] = alpacaDevice;
		}
		else
		{
			std::string lastError = alpacaDevice->getLastError();
			if (lastError.empty())
				setLastError("Failed to open device: " + std::string(portName));
			else
				setLastError("Failed to open device: " + std::string(portName) + " (" + lastError + ")");

			AppCore::writeToApplicationLogLine("DevTACCore::OpenHandleByDescription failed for " + std::string(portName) + ": " + _lastError);
			alpacaDevice->close();
		}
	}
	else
	{
		setLastError("Failed to open device: " + std::string(portName));
		AppCore::writeToApplicationLogLine("DevTACCore::OpenHandleByDescription device not found: " + std::string(portName));
	}

	return result;
}

TAC_RESULT DevTACCore::CloseTACHandle(TAC_HANDLE tacHandle)
{
	TAC_RESULT result{NO_TAC_ERROR};

	AlpacaDevice alpacaDevice = getAlpacaDevice(tacHandle);
	if (alpacaDevice != nullptr)
	{
		alpacaDevice->close();
		_openDevices.erase(tacHandle);
	}
	else
	{
		result = TACDEV_BAD_TAC_HANDLE;
		AppCore::writeToApplicationLogLine("DevTACCore::CloseTACHandle: Bad TAC Handle (" + std::to_string(tacHandle) + ")");
	}

	return result;
}

void DevTACCore::onErrorEvent(const std::string& message)
{
	setLastError(message);
}
