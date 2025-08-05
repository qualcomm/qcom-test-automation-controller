#ifndef FTDICHECKAPPLICATION_H
#define FTDICHECKAPPLICATION_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
