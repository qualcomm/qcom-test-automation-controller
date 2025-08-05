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

