// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PreferencesBase.h"

// QCommon
#include "ConsoleApplicationEnhancements.h"
#include "AlpacaSettings.h"

const QString kLoggingEnabled(QStringLiteral("logEnabled"));
const QString kAppLogLocation(QStringLiteral("appLogLocation"));
const QString kRunLogLocation(QStringLiteral("runLogLocation"));
const QString kPlatformConfigLocation(QStringLiteral("platformConfigLocation"));

PreferencesBase::PreferencesBase()
{

}

void PreferencesBase::setAppName
(
	const std::string& appName,
	const std::string& appVersion
)
{
	_appName = appName;
	_appVersion = appVersion;

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);
	_loggingActive = settings.value(kLoggingEnabled, defaultLoggingState()).toBool();
	_appLoggingPath = killOneDrive(settings.value(kAppLogLocation, QString::fromStdString(defaultAppLogPath())).toString().toStdString(), defaultAppLogPath());
	_runLoggingPath = killOneDrive(settings.value(kRunLogLocation, QString::fromStdString(defaultRunLogPath())).toString().toStdString(), defaultRunLogPath());
	_platformConfigLocation = killOneDrive(defaultPlatformConfigLocation(), defaultPlatformConfigLocation());
	settings.endGroup();
}

std::string PreferencesBase::appName()
{
	return _appName;
}

std::string PreferencesBase::appVersion()
{
	return _appVersion;
}

bool PreferencesBase::defaultLoggingState()
{
	return false;
}

bool PreferencesBase::loggingActive()
{
	return _loggingActive;
}

void PreferencesBase::setLoggingActive
(
	bool loggingActive
)
{
	_loggingActive = loggingActive;
}

void PreferencesBase::saveLoggingActive(bool loggingActive)
{
	_loggingActive = loggingActive;

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);
	settings.setValue(kLoggingEnabled, _loggingActive);
	settings.endGroup();
}

std::string PreferencesBase::defaultAppLogPath()
{
	return defaultGlobalLoggingPath();
}

std::string PreferencesBase::appLogPath()
{
	return _appLoggingPath;
}

void PreferencesBase::setAppLogPath
(
	const std::string& loggingPath
)
{
	_appLoggingPath = loggingPath;
}

void PreferencesBase::saveAppLogPath
(
	const std::string& loggingPath
)
{
	setAppLogPath(loggingPath);

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);
	settings.setValue(kAppLogLocation, QString::fromStdString(_appLoggingPath));
	settings.endGroup();
}

std::string PreferencesBase::defaultRunLogPath()
{
	return defaultLoggingPath(_appName);
}

std::string PreferencesBase::runLogPath()
{
	return _runLoggingPath;
}

void PreferencesBase::setRunLogPath
(
	const std::string& loggingPath
)
{
	_runLoggingPath = loggingPath;
}

void PreferencesBase::saveRunLogPath
(
	const std::string& loggingPath
)
{
	setRunLogPath(loggingPath);

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);
	settings.setValue(kRunLogLocation, QString::fromStdString(_runLoggingPath));
	settings.endGroup();
}

std::string PreferencesBase::defaultPlatformConfigLocation()
{
	return tacConfigRoot();
}

std::string PreferencesBase::platformConfigLocation()
{
	return _platformConfigLocation;
}

void PreferencesBase::setPlatformConfigLocation(const std::string& platformConfigLocation)
{
	Q_UNUSED(platformConfigLocation);
}

void PreferencesBase::savePlatformConfigLocation(const std::string& saveLocation)
{
	Q_UNUSED(saveLocation);
}

