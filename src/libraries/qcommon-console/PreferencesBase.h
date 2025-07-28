#ifndef PREFERENCESBASE_H
#define PREFERENCESBASE_H
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
// Copyright 2013-2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QT
#include <QByteArray>
#include <QString>
#include <QVariant>

const QString kPreferences(QStringLiteral("Preferences"));

class QCOMMONCONSOLE_EXPORT PreferencesBase
{
public:
	PreferencesBase();

	virtual void setAppName(const QByteArray& appName, const QByteArray& appVersion);

	QByteArray appName();
	QByteArray appVersion();

	bool defaultLoggingState();
	bool loggingActive();
	void setLoggingActive(bool logActive);
	void saveLoggingActive(bool logActive);

	QString defaultAppLogPath();
	QString appLogPath();
	void setAppLogPath(const QString& logPath);
	void saveAppLogPath(const QString& loggingPath);

	QString defaultRunLogPath();
	QString runLogPath();
	void setRunLogPath(const QString& logPath);
	void saveRunLogPath(const QString& loggingPath);

	QString defaultPlatformConfigLocation();
	QString platformConfigLocation();
	void setPlatformConfigLocation(const QString& platformConfigLocation);
	void savePlatformConfigLocation(const QString& saveLocation);

protected:
	QByteArray					_appName;
	QByteArray					_appVersion;
	bool						_loggingActive{false};
	QString						_appLoggingPath;
	QString						_runLoggingPath;
	QString						_platformConfigLocation;
};

#endif // PREFERENCESBASE_H
