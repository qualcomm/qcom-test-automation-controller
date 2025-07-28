#ifndef FTDICHECKAPPLICATION_H
#define FTDICHECKAPPLICATION_H

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
#include <QtGlobal>

#ifdef Q_OS_WIN
	#include <Windows.h>
#endif

// Qt
#include <QMap>
#include <QString>
#include <QStringList>

enum ArchType
{
	eUnknown,
	eX32,
	eX64,
	eARM
};

class FTDICheckApplication
{
public:
	FTDICheckApplication();
	~FTDICheckApplication();

	int defaultActivity();
	void showVersion();

	bool verifyInstalledDriver();
	bool uninstallDriver();
	bool installDriver();

private:
	ArchType getArchitecture();

	bool detectLibrary(const QString& libraryName);
	bool checkLibraryVersion(const QString& filePath);

	bool isExpectedProductVersion(const QString& fileName, const QString& version);
	bool isFTDIDriverInfFile(const QString& filePath);

	bool isWindowsFTDIDriverFilesPresent();
	bool isLinuxFTDIDriverFilesPresent();

	bool isDriverLatest();

	bool deleteWindowsDriver(const QString& infFile);
	bool addWindowsDriver(const QString& infFile);

	bool deleteLinuxDriver();
	bool addLinuxDriver();


	QStringList                    _detectedFiles;
	QMap<QString, QString>         _expectedProductVersionMap;
};

#endif // FTDICHECKAPPLICATION_H
