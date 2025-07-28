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
