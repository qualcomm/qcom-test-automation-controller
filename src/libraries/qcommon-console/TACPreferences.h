#ifndef TACPREFERENCES_H
#define TACPREFERENCES_H
// Confidential and Proprietary â€“ Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2021-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
