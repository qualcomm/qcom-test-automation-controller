// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "TACPreferences.h"

// TAC
#include "TACDefines.h"

// QCommon
#include "ConsoleApplicationEnhancements.h"
#include "AlpacaSettings.h"

const QString kButtonAssertTime(QStringLiteral("buttonAssertTime"));
const QString kPowerOnDelay(QStringLiteral("powerOnDelay"));
const QString kShutdownDelayEnable(QStringLiteral("shutdownDelayEnable"));
const QString kShutdownDelay(QStringLiteral("shutdownDelay"));
const QString kLastDevice(QStringLiteral("lastDevice"));
const QString kOpenLastDeviceOnStarup(QStringLiteral("openLastDeviceOnStartup"));
const QString kTACConfigurationPath(QStringLiteral("TACConfigLocations"));
const QString kTACEditorConfigurationPath(QStringLiteral("TACEditorConfigLocations"));

void TACPreferences::setAppName
(
	const QByteArray& appName,
	const QByteArray& appVersion
)
{
	PreferencesBase::setAppName(appName, appVersion);

	AlpacaSettings settings(_appName);

	settings.beginGroup(kPreferences);

	_powerDelay = settings.value(kPowerOnDelay, defaultPowerDelay()).toInt();
	_buttonAssertTime = settings.value(kButtonAssertTime, defaultButtonAssertTime()).toInt();
	_autoShutdown = settings.value(kShutdownDelayEnable, defaultAutoShutdown()).toBool();
	_autoShutdownDelay = settings.value(kShutdownDelay, defaultAutoShutdownDelay()).toReal();
	_openLast = settings.value(kOpenLastDeviceOnStarup, defaultOpenLastStart()).toBool();
	_tacEditorConfigLocation = settings.value(kTACEditorConfigurationPath, defaultTACEditorConfigLocation()).toString();

	settings.endGroup();
}

int TACPreferences::defaultPowerDelay()
{
	return 800;
}

int TACPreferences::powerDelay()
{
	return _powerDelay;
}

void TACPreferences::setPowerDelay
(
	int powerDelay
)
{
	_powerDelay = powerDelay;
}

void TACPreferences::savePowerDelay(int powerDelay)
{
	_powerDelay = powerDelay;

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kPowerOnDelay, powerDelay);
	settings.endGroup();
}

int TACPreferences::defaultButtonAssertTime()
{
	return 8;
}

int TACPreferences::buttonAssertTime()
{
	return _buttonAssertTime;
}

void TACPreferences::setButtonAssertTime
(
	int buttonAssertTime
)
{
	_buttonAssertTime = buttonAssertTime;
}

void TACPreferences::saveButtonAssertTime
(
	int buttonAssertTime
)
{
	_buttonAssertTime = buttonAssertTime;

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kButtonAssertTime, buttonAssertTime);
	settings.endGroup();
}

bool TACPreferences::defaultAutoShutdown()
{
	return false;
}

bool TACPreferences::autoShutdownActive()
{
	return _autoShutdown;
}

void TACPreferences::setAutoShutdownActive
(
	bool shutdownActive
)
{
	_autoShutdown = shutdownActive;
}

void TACPreferences::saveAutoShutdownActive
(
	bool shutdownActive
)
{
	_autoShutdown = shutdownActive;

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kShutdownDelayEnable, shutdownActive);

	settings.endGroup();
}

qreal TACPreferences::defaultAutoShutdownDelay()
{
	return 24.0;
}

qreal TACPreferences::autoShutdownDelay()
{
	return _autoShutdownDelay;
}

qint64 TACPreferences::autoShutdownDelayInMSecs()
{
	qreal result = autoShutdownDelay();

	result *= 1000. * 60. * 60.;

	return static_cast<qint64>(result);
}

void TACPreferences::setAutoShutdownDelay
(
	qreal delayInHours
)
{
	_autoShutdownDelay = delayInHours;
}

void TACPreferences::saveAutoShutdownDelay(qreal delayInHours)
{
	_autoShutdownDelay = delayInHours;

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kShutdownDelay, delayInHours);

	settings.endGroup();
}

bool TACPreferences::defaultOpenLastStart()
{
	return false;
}

bool TACPreferences::openLastStart()
{
	return _openLast;
}

void TACPreferences::setOpenLastStart(bool openLastOnStart)
{
	_openLast = openLastOnStart;
}

void TACPreferences::saveOpenLastStart(bool openLastOnStart)
{
	_openLast = openLastOnStart;

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kOpenLastDeviceOnStarup, openLastOnStart);

	settings.endGroup();
}

QString TACPreferences::lastDevice()
{
	QString result;

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
    result = settings.value(kLastDevice, QString()).toString();

	settings.endGroup();

	return result;
}

void TACPreferences::saveLastDevice
(
	const QString& lastDevice
)
{
	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kLastDevice, lastDevice);

	settings.endGroup();
}

QString TACPreferences::defaultTACEditorConfigLocation()
{
	return documentsDataPath("TAC Configurations");
}

QString TACPreferences::tacEditorConfigLocation()
{
	return _tacEditorConfigLocation;
}

void TACPreferences::setTACEditorConfigLocation(const QString &tacEditorConfigLocation)
{
	_tacEditorConfigLocation = killOneDrive(tacEditorConfigLocation, defaultTACEditorConfigLocation());
}

void TACPreferences::saveTACEditorConfigLocation(const QString &saveLocation)
{
	setTACEditorConfigLocation(saveLocation);

	AlpacaSettings settings(kAppName);

	settings.beginGroup(kPreferences);
	settings.setValue(kTACEditorConfigurationPath, _tacEditorConfigLocation);

	settings.endGroup();
}

