// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

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
	const QByteArray& appName,
	const QByteArray& appVersion
)
{
	_appName = appName;
	_appVersion = appVersion;

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);
	_loggingActive = settings.value(kLoggingEnabled, defaultLoggingState()).toBool();
	_appLoggingPath = killOneDrive(settings.value(kAppLogLocation, defaultAppLogPath()).toString(), defaultAppLogPath());
	_runLoggingPath = killOneDrive(settings.value(kRunLogLocation, defaultRunLogPath()).toString(), defaultRunLogPath());
	_platformConfigLocation = killOneDrive(defaultPlatformConfigLocation(), defaultPlatformConfigLocation());
	settings.endGroup();
}

QByteArray PreferencesBase::appName()
{
	return _appName;
}

QByteArray PreferencesBase::appVersion()
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

QString PreferencesBase::defaultAppLogPath()
{
	return defaultGlobalLoggingPath();
}

QString PreferencesBase::appLogPath()
{
	return _appLoggingPath;
}

void PreferencesBase::setAppLogPath
(
	const QString& loggingPath
)
{
	_appLoggingPath = loggingPath;
}

void PreferencesBase::saveAppLogPath
(
	const QString& loggingPath
)
{
	setAppLogPath(loggingPath);

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);
	settings.setValue(kAppLogLocation, _appLoggingPath);
	settings.endGroup();
}

QString PreferencesBase::defaultRunLogPath()
{
	return defaultLoggingPath(_appName);
}

QString PreferencesBase::runLogPath()
{
	return _runLoggingPath;
}

void PreferencesBase::setRunLogPath
(
	const QString& loggingPath
)
{
	_runLoggingPath = loggingPath;
}

void PreferencesBase::saveRunLogPath
(
	const QString& loggingPath
)
{
	setRunLogPath(loggingPath);

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);
	settings.setValue(kRunLogLocation, _runLoggingPath);
	settings.endGroup();
}

QString PreferencesBase::defaultPlatformConfigLocation()
{
	QString result;

#ifdef DEBUG
	result = "C:\\github\\AlpacaRepos\\AlpacaConfigs\\tac_configs\\";
#else
	result = tacConfigRoot();
#endif

	return result;
}

QString PreferencesBase::platformConfigLocation()
{
	return _platformConfigLocation;
}

void PreferencesBase::setPlatformConfigLocation(const QString& platformConfigLocation)
{
	Q_UNUSED(platformConfigLocation);
}

void PreferencesBase::savePlatformConfigLocation(const QString& saveLocation)
{
	Q_UNUSED(saveLocation);
}

