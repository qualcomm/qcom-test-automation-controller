#ifndef CONSOLEAPPLICATIONENHANCEMENTS_H
#define CONSOLEAPPLICATIONENHANCEMENTS_H
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
// Copyright 2013-2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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

