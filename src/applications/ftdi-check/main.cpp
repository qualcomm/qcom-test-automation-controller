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
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
