// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
