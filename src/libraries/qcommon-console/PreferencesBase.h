#ifndef PREFERENCESBASE_H
#define PREFERENCESBASE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QT
#include <QVariant>

#include <string>

const QString kPreferences(QStringLiteral("Preferences"));

class QCOMMONCONSOLE_EXPORT PreferencesBase
{
public:
	PreferencesBase();

	virtual void setAppName(const std::string& appName, const std::string& appVersion);

	std::string appName();
	std::string appVersion();

	bool defaultLoggingState();
	bool loggingActive();
	void setLoggingActive(bool logActive);
	void saveLoggingActive(bool logActive);

	std::string defaultAppLogPath();
	std::string appLogPath();
	void setAppLogPath(const std::string& logPath);
	void saveAppLogPath(const std::string& loggingPath);

	std::string defaultRunLogPath();
	std::string runLogPath();
	void setRunLogPath(const std::string& logPath);
	void saveRunLogPath(const std::string& loggingPath);

	std::string defaultPlatformConfigLocation();
	std::string platformConfigLocation();
	void setPlatformConfigLocation(const std::string& platformConfigLocation);
	void savePlatformConfigLocation(const std::string& saveLocation);

protected:
	std::string					_appName;
	std::string					_appVersion;
	bool						_loggingActive{false};
	std::string					_appLoggingPath;
	std::string					_runLoggingPath;
	std::string					_platformConfigLocation;
};

#endif // PREFERENCESBASE_H
