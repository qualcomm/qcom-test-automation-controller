// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
            Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// Update Device List
#include "FTDITemplateCompiler.h"
#include "UDLCommandLine.h"
#include "UpdateDeviceList.h"

// qcommon-console
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

	preferencesBase.setAppName(kAppName.toLatin1(), UPDATE_DEVICE_LIST);
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();

	UDLCommandLine parser(a.arguments());
	UpdateDeviceList deviceList;
    FTDITemplateCompiler ftdiTemplateCompiler;

	bool verbose = parser.verbose();

	if (parser.helpRequested() == false)
	{
		QString fileDir = parser.userDir();
		deviceList.setDeviceListDir(fileDir);
		deviceList.setVerbosity(verbose);
		deviceList.write();

        ftdiTemplateCompiler.setDeviceListDir(fileDir);
        ftdiTemplateCompiler.setVerbosity(verbose);
        ftdiTemplateCompiler.write();
	}
	else
	{
		QString helpText = parser.helpText();
		std::cout << helpText.toLatin1().data() << std::endl;
	}

	return 0;
}
