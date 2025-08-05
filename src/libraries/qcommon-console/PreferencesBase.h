#ifndef PREFERENCESBASE_H
#define PREFERENCESBASE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
