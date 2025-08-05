#ifndef CONSOLEAPPLICATIONENHANCEMENTS_H
#define CONSOLEAPPLICATIONENHANCEMENTS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// QCommon
#include "QCommonConsoleGlobal.h"

// QT
#include <QSettings>
#include <QString>


QString QCOMMONCONSOLE_EXPORT applicationBinPath();
QString QCOMMONCONSOLE_EXPORT applicationDataPath();
QString QCOMMONCONSOLE_EXPORT documentsDataPath(const QString& append);
QString QCOMMONCONSOLE_EXPORT defaultGlobalLoggingPath();
QString QCOMMONCONSOLE_EXPORT defaultLoggingPath(const QString& appName);

QString QCOMMONCONSOLE_EXPORT killOneDrive(const QString& testPath, const QString& revertPath);

QString QCOMMONCONSOLE_EXPORT createFilenameTimeStamp();

bool QCOMMONCONSOLE_EXPORT isNewInstall(QSettings& settings, const QString& currentInstallGuuid);
void QCOMMONCONSOLE_EXPORT renewInstallRegistry(QSettings& settings, const QString& newInstallGuid);

QString QCOMMONCONSOLE_EXPORT getModuleFilePath(const QString& moduleFileName);
QString QCOMMONCONSOLE_EXPORT expandPath(const QString& filePath);

QString QCOMMONCONSOLE_EXPORT tacConfigRoot(bool expandPath = true);
QString QCOMMONCONSOLE_EXPORT epmConfigRoot();

void kickIt();
void cleanIt(const QString& pathToClean);

bool QCOMMONCONSOLE_EXPORT isUserPrivileged();
bool QCOMMONCONSOLE_EXPORT executeBinaryAsAdministrator(const QString &binary, const QString &cmdArgs);

#endif

