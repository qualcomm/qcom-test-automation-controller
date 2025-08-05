// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "TACDev.h"

// libTAC
#include "AlpacaDevice.h"
#include "TACPreferences.h"

// QCommon
#include "AlpacaSharedLibrary.h"

// QT
#include <QMap>

using namespace std;

class DevTacCore :
	public QObject,
	public AlpacaSharedLibrary
{
	Q_OBJECT
public:
	DevTacCore()
	{
	}

	~DevTacCore()
	{
	}

	bool initialize(const QByteArray& appName, const QByteArray& appVersion);

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

			*deviceCount = _alpacaDevices.count();

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

private slots:
	void onErrorEvent(const QByteArray& message);

private:
	bool							_initialized{false};
	TACPreferences					_preferences;
	AlpacaDevices					_alpacaDevices;

	QMap<TAC_HANDLE, AlpacaDevice>	_openDevices;

};
