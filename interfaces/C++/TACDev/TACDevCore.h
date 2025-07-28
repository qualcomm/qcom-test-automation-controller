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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
