// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Update Device List
#include "UDLCommandLine.h"
#include "UpdateDeviceList.h"

// QCommonConsole
#include "AppCore.h"
#include "PreferencesBase.h"
#include "QCommonConsole.h"
#include "version.h"

// QT
#include <QCoreApplication>
#include <QDir>

// C++
#include <iostream>

int main(int argc, char *argv[])
{
	QString kAppName{"UpdateDeviceList"};

	QCoreApplication a(argc, argv);

	a.setApplicationName(kAppName);
	a.setApplicationVersion(UPDATE_DEVICE_LIST);

	AppCore* appCore = AppCore::getAppCore();
	PreferencesBase preferencesBase;

	preferencesBase.setAppName(kAppName.toStdString(), UPDATE_DEVICE_LIST);
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();

	UDLCommandLine parser(a.arguments());
	UpdateDeviceList deviceList;

	bool verbose = parser.verbose();

	if (parser.helpRequested() == false)
	{
		QString fileDir = parser.userDir();
		deviceList.setDeviceListDir(fileDir);
		deviceList.setVerbosity(verbose);
		deviceList.write();
	}
	else
	{
		std::cout << parser.helpText() << std::endl;
	}

	return 0;
}
