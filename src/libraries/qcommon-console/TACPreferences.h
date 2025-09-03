#ifndef TACPREFERENCES_H
#define TACPREFERENCES_H
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

// Author: msimpson

#include "QCommonConsoleGlobal.h"

#include <QStandardPaths>
#include <QString>

// QCommonConsole
#include "PreferencesBase.h"

class QCOMMONCONSOLE_EXPORT TACPreferences :
	public PreferencesBase
{
public:
	TACPreferences() = default;

	virtual void setAppName(const QByteArray& appName, const QByteArray& appVersion);

	int defaultPowerDelay();
	int powerDelay();
	void setPowerDelay(int powerDelay);
	void savePowerDelay(int powerDelay);

	int defaultButtonAssertTime();
	int buttonAssertTime();
	void setButtonAssertTime(int buttonAssertTime);
	void saveButtonAssertTime(int buttonAssertTime);

	bool defaultAutoShutdown();
	bool autoShutdownActive();
	void setAutoShutdownActive(bool shutdownActive);
	void saveAutoShutdownActive(bool shutdownActive);

	qreal defaultAutoShutdownDelay();
	qreal autoShutdownDelay();
	void setAutoShutdownDelay(qreal delayInHours);
	void saveAutoShutdownDelay(qreal delayInHours);
	qint64 autoShutdownDelayInMSecs();

	bool defaultOpenLastStart();
	bool openLastStart();
	void setOpenLastStart(bool openLastOnStart);
	void saveOpenLastStart(bool openLastOnStart);

	QString lastDevice();
	void saveLastDevice(const QString& lastDevice);

	QString defaultTACEditorConfigLocation();
	QString tacEditorConfigLocation();
	void setTACEditorConfigLocation(const QString& tacConfigLocation);
	void saveTACEditorConfigLocation(const QString& saveLocation);

private:
	int							_powerDelay{0};
	int							_buttonAssertTime{0};
	bool						_autoShutdown{false};
	qreal						_autoShutdownDelay{0.};
	bool						_openLast{false};
	QString                     _tacEditorConfigLocation;
};

#endif // TACPREFERENCES_H
