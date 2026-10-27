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
#include <QCommandLineOption>
#include <QCommandLineParser>
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

// Exit status used when the command line cannot be parsed.
const int kBadCommandLine{1};

} // namespace

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	// Parse the command line before touching configuration or devices, so
	// --help never enumerates hardware.
	QCommandLineParser parser;
	parser.setApplicationDescription("Lists the connected TAC devices with their port and serial number.");
	// Keep the historical single-dash "-quts" option working.
	parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
	const QCommandLineOption helpOption = parser.addHelpOption();
	const QCommandLineOption qutsOption(QStringList() << "quts", "Print one \"port;serial;\" line per device, without headers.");
	parser.addOption(qutsOption);

	if (parser.parse(a.arguments()) == false)
	{
		cerr << parser.errorText().toLatin1().data() << endl << endl;
		cerr << parser.helpText().toLatin1().data();
		return kBadCommandLine;
	}

	if (parser.isSet(helpOption))
	{
		cout << parser.helpText().toLatin1().data();
		return 0;
	}

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
	const bool forQuts = parser.isSet(qutsOption);

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
