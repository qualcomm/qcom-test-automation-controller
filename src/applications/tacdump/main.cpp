// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// tacLib
#include "AlpacaDevice.h"
#include "TACPreferences.h"

// QCommon
#include "AppCore.h"
#include "ConsoleApplicationEnhancements.h"
#include "QCommonConsole.h"

// Qt
#include <QCoreApplication>

// C++
#include <iostream>

using namespace std;

class DevTACCore
{
public:
	DevTACCore()
	{
		_appCore = AppCore::getAppCore();

		_preferences.setAppName("TAC", "1.1.1");
		_appCore->setPreferences(&_preferences);
		_appCore->postAutomationEvent();
	}

	~DevTACCore()
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

namespace
{

// Exit status used when configuration storage is unusable. Any nonzero value
// would do; a distinct one lets callers tell this apart from "no devices".
const int kConfigurationUnavailable{2};

} // namespace

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	DevTACCore tacCore;

	InitializeQCommonConsole();

	// Device to port mappings live in the per user configuration directory.
	// Without it, enumeration cannot report which port belongs to which
	// device, so callers could act on the wrong one. Fail here instead of
	// printing a list that may be wrong.
	if (documentsDataPathAvailable() == false)
	{
		cerr << "Error: the Alpaca configuration directory is not available or not writable."
			 << endl
			 << "Device information cannot be reported reliably. Check that the home"
			 << " directory exists, is writable, and is not out of space." << endl;

		return kConfigurationUnavailable;
	}

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

	return 0;
}
