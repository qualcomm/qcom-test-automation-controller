#ifndef TACPREFERENCES_H
#define TACPREFERENCES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "QCommonConsoleGlobal.h"

#include <QStandardPaths>
#include <QString>

// qcommon-console
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
