// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
