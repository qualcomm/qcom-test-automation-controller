#ifndef CONSOLEAPPLICATIONENHANCEMENTS_H
#define CONSOLEAPPLICATIONENHANCEMENTS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonConsoleGlobal.h"

// QT
#include <QSettings>
#include <QString>

QString QCOMMONCONSOLE_EXPORT applicationBinPath();
QString QCOMMONCONSOLE_EXPORT applicationDataPath();
// Returns the per user configuration directory, or an empty string when no
// writable location could be established. An empty result means configuration
// cannot be read or written and callers must fail rather than continue.
QString QCOMMONCONSOLE_EXPORT documentsDataPath(const QString& append);

// Convenience check for callers that only need to know whether configuration
// storage is usable before they begin work.
bool QCOMMONCONSOLE_EXPORT documentsDataPathAvailable();
QString QCOMMONCONSOLE_EXPORT defaultGlobalLoggingPath();
QString QCOMMONCONSOLE_EXPORT defaultLoggingPath(const QString& appName);

QString QCOMMONCONSOLE_EXPORT killOneDrive(const QString& testPath, const QString& revertPath);

QString QCOMMONCONSOLE_EXPORT createFilenameTimeStamp();

bool QCOMMONCONSOLE_EXPORT isNewInstall(QSettings& settings, const QString& currentInstallGuuid);
void QCOMMONCONSOLE_EXPORT renewInstallRegistry(QSettings& settings, const QString& newInstallGuid);

QString QCOMMONCONSOLE_EXPORT getModuleFilePath(const QString& moduleFileName);
QString QCOMMONCONSOLE_EXPORT expandPath(const QString& filePath);

QString QCOMMONCONSOLE_EXPORT tacConfigRoot();

// Logs the executable, data, configuration and documents folders once, so
// path problems in a given install layout can be diagnosed from the log.
void QCOMMONCONSOLE_EXPORT logInstallLayout();

void cleanIt(const QString& pathToClean);

bool QCOMMONCONSOLE_EXPORT isUserPrivileged();
bool QCOMMONCONSOLE_EXPORT executeBinaryAsAdministrator(const QString &binary, const QString &cmdArgs);

#endif
