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
// Copyright 2019-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// TAC
#include "TACApplication.h"

// libTAC
#include "QCommonConsole.h"

// QCommon
#include "QCommon.h"

// Qt
#include <QCoreApplication>
#include <QDir>
#include <QFont>

int main
(
	int argc,
	char* argv[]
)
{
	int result{0};
    QStringList libPaths;

	TACApplication a(argc, argv);

#ifdef Q_OS_LINUX
    QFont font("Tahoma", 8);
    QApplication::setFont(font);
#endif

    QString appPath = QDir::cleanPath(a.applicationDirPath() + "/platforms");
    libPaths.append(appPath);
	QCoreApplication::setLibraryPaths(libPaths);

	InitializeQCommon();

	if (a.openLastDevice() == false)
		a.createTACWindow();

	result = a.exec();

	return result;
}
