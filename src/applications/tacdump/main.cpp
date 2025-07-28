// Confidential and Proprietary Qualcomm Technologies, Inc.

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
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// tacLib
#include "AlpacaDevice.h"
#include "TACPreferences.h"

// QCommon
#include "AppCore.h"
#include "QCommonConsole.h"

// Qt
#include <QCoreApplication>

// C++
#include <iostream>

using namespace std;

class DevTacCore
{
public:
	DevTacCore()
	{
		_appCore = AppCore::getAppCore();

		_preferences.setAppName("TAC", "1.1.1");
		_appCore->setPreferences(&_preferences);
		_appCore->postAutomationEvent();
	}

	~DevTacCore()
	{
	}

	void setLoggingState(bool state)
	{
		_appCore->setAppLogging(state);
	}

	bool getLoggingState()
	{
		return _appCore->appLoggingActive();
	}

	AppCore* appCore()
	{
		return AppCore::getAppCore();
	}

private:
	TACPreferences				_preferences;
	AppCore*					_appCore{Q_NULLPTR};
};

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	DevTacCore tacCore;

	InitializeQCommonConsole();

	QString errorString;
	bool forQuts{false};

	if (argc > 1)
	{
		if (a.arguments().at(1) == "-quts")
			forQuts = true;
	}

	AlpacaDevices alpacaDevices;

	_AlpacaDevice::updateAlpacaDevices();
	_AlpacaDevice::getAlpacaDevices(alpacaDevices);

	if (alpacaDevices.count() > 0)
	{
		if (!forQuts)
			cout << "Devices:" << endl;

		for (const auto& alpacaDevice : std::as_const(alpacaDevices))
		{
			if (!forQuts)
			{
				cout << "   Device:" << alpacaDevice->portName().data() << " Serial:" << alpacaDevice->serialNumber().data() << endl;
			}
			else
			{
				cout << alpacaDevice->portName().data() << ";" << alpacaDevice->serialNumber().data()<< ";" << endl;
			}
		}
	}
	else
	{
		if (errorString.isEmpty())
		{
			cout << "No devices found." << endl;
		}
		else
		{
			cout << errorString.toLatin1().data() << endl;
		}
	}
}
