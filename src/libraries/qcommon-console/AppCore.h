#ifndef APPCORE_H
#define APPCORE_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

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
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
class LicenseManager;
class TelematicsManager;

#include "QCommonConsoleGlobal.h"
#include "PreferencesBase.h"
#include "ThreadedLog.h"

// QT
#include <QString>

class QCOMMONCONSOLE_EXPORT AppCore
{
friend class AppCoreDestructor;

public:
	~AppCore();

	static AppCore* getAppCore();

	QByteArray appName()
	{
		return _appName;
	}

	QByteArray appVersion()
	{
		return _appVersion;
	}

	quint32 daysSinceInstall();

	PreferencesBase* getPreferences()
	{
		return _preferences;
	}
	void setPreferences(PreferencesBase* preferences);

	void setAppLogging(bool loggingState);
	bool appLoggingActive();

	QString currentAppLog()
	{
		return _appThreadedLog->currentLogPath();
	}

	void setRunLogging(bool loggingState);
	bool runLoggingActive();
	QString currentRunLog()
	{
		return _runThreadedLog->currentLogPath();
	}

	QString loggingPath();

	bool checkLicense(const QByteArray& productID, const QByteArray& featureID);
	bool isLicenseManagerValid();

	void postStartEvent();
	void postAutomationEvent();
	void postMetric(const QByteArray& metricID, double metric);

	void writeToAppLog(const QString& writeMe);

	static void writeToApplicationLog(const QString& writeMe);
	static void writeToApplicationLogLine(const QString& writeMe);

	void writeToRunLog(const QString& writeMe);
	static void writeToRuntimeLog(const QString& writeMe);

private:
	AppCore();

	// This isn't a singleton.  It's just a convenience for saving and restoring appcore
	static AppCore*				_appCore;

	bool						_internalBuild{false};
	QByteArray					_appName;
	QByteArray					_appVersion;
	// LicenseManager*				_licenseManager{Q_NULLPTR};
	// TelematicsManager*			_telematicsManager{Q_NULLPTR};

	PreferencesBase*			_preferences{Q_NULLPTR};
	ThreadedLog					_appThreadedLog;
	ThreadedLog					_runThreadedLog;
};

#endif // APPCORE_H
