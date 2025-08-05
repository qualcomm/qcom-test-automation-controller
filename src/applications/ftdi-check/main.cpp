// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "FTDICheckCommandLine.h"
#include "FTDICheckApplication.h"

// qcommon-console
#include "AppCore.h"
#include "PreferencesBase.h"
#include "QCommonConsole.h"
#include "version.h"

// Qt
#include <QCoreApplication>

// C++
#include <iostream>

int main(int argc, char *argv[])
{
	int result{0};
	QString kAppName{"ftdi-check"};

	QCoreApplication a(argc, argv);

	a.setApplicationName(kAppName);
	a.setApplicationVersion(FTDI_CHECK_VERSION);

	AppCore* appCore = AppCore::getAppCore();
	PreferencesBase preferencesBase;

	preferencesBase.setAppName(kAppName.toLatin1(), UPDATE_DEVICE_LIST);
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();

	FTDICheckCommandLine parser(a.arguments());
	FTDICheckApplication ftdiApp;

	if (parser.helpRequested())
	{
		QString helpText = parser.helpText();
		std::cout << helpText.toLatin1().data() << std::endl;
	}
	else if (parser.value("validate") == "true")
		result = ftdiApp.verifyInstalledDriver();
	else if (parser.value("uninstall") == "true")
		result = ftdiApp.uninstallDriver();
	else if (parser.value("install") == "true")
		result = ftdiApp.installDriver();
	else if (parser.value("version") == "true")
		ftdiApp.showVersion();
	else
		result = ftdiApp.defaultActivity();

	return result;
}
