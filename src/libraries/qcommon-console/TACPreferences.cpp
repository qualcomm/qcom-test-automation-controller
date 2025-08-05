/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

