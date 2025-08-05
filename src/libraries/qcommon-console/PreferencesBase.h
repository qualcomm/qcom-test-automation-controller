#ifndef PREFERENCESBASE_H
#define PREFERENCESBASE_H
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
