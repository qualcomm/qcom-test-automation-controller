// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TACDEVCORE_H
#define TACDEVCORE_H

#include "TACDev.h"

#include "AlpacaDevice.h"
#include "TACPreferences.h"
#include "AlpacaSharedLibrary.h"

#include <map>
#include <string>

class DevTACCore : public AlpacaSharedLibrary
{
public:
	DevTACCore() = default;
	~DevTACCore() = default;

	bool initialize(const std::string& appName, const std::string& appVersion);

	void setLoggingState(bool state);
	bool getLoggingState();

	AppCore* appCore();

	AlpacaDevice getAlpacaDevice(TAC_HANDLE tacHandle);

	TAC_RESULT GetDeviceCount(int* deviceCount)
	{
		TAC_RESULT result{TACDEV_INIT_FAILED};

		if (_initialized == true)
		{
			*deviceCount = 0;
			_AlpacaDevice::updateAlpacaDevices();
			_AlpacaDevice::getAlpacaDevices(_alpacaDevices);

			*deviceCount = static_cast<int>(_alpacaDevices.size());

			result = NO_TAC_ERROR;
		}

		return result;
	}

	const AlpacaDevices& GetAlpacaDevices()
	{
		return _alpacaDevices;
	}

	TAC_HANDLE OpenHandleByDescription(const char* portName);
	TAC_RESULT CloseTACHandle(TAC_HANDLE tacHandle);

	void onErrorEvent(const std::string& message);

private:
	bool								_initialized{false};
	TACPreferences						_preferences;
	AlpacaDevices						_alpacaDevices;
	std::map<TAC_HANDLE, AlpacaDevice>	_openDevices;
};

#endif // TACDEVCORE_H
