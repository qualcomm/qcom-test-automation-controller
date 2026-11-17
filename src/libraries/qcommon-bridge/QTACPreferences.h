#ifndef QTACPREFERENCES_H
#define QTACPREFERENCES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"
#include "TACPreferences.h"

// Qt
#include <QString>

class QCOMMONBRIDGE_EXPORT QTACPreferences : public TACPreferences
{
public:
	QTACPreferences() = default;
	virtual ~QTACPreferences() = default;

	using TACPreferences::setAppName;
	void setAppName(const QString& appName, const QString& appVersion)
	{
		TACPreferences::setAppName(appName.toStdString(), appVersion.toStdString());
	}

	void setAppName(const QByteArray& appName, const QByteArray& appVersion)
	{
		TACPreferences::setAppName(appName.toStdString(), appVersion.toStdString());
	}

	QString getAppName()
	{
		return QString::fromStdString(TACPreferences::appName());
	}

	QString getAppVersion()
	{
		return QString::fromStdString(TACPreferences::appVersion());
	}

	QString defaultAppLogPath()
	{
		return QString::fromStdString(TACPreferences::defaultAppLogPath());
	}

	QString appLogPath()
	{
		return QString::fromStdString(TACPreferences::appLogPath());
	}

	using TACPreferences::setAppLogPath;
	void setAppLogPath(const QString& logPath)
	{
		TACPreferences::setAppLogPath(logPath.toStdString());
	}

	using TACPreferences::saveAppLogPath;
	void saveAppLogPath(const QString& loggingPath)
	{
		TACPreferences::saveAppLogPath(loggingPath.toStdString());
	}

	QString defaultRunLogPath()
	{
		return QString::fromStdString(TACPreferences::defaultRunLogPath());
	}

	QString runLogPath()
	{
		return QString::fromStdString(TACPreferences::runLogPath());
	}

	using TACPreferences::setRunLogPath;
	void setRunLogPath(const QString& logPath)
	{
		TACPreferences::setRunLogPath(logPath.toStdString());
	}

	using TACPreferences::saveRunLogPath;
	void saveRunLogPath(const QString& loggingPath)
	{
		TACPreferences::saveRunLogPath(loggingPath.toStdString());
	}

	QString lastDevice()
	{
		return QString::fromStdString(TACPreferences::lastDevice());
	}

	using TACPreferences::saveLastDevice;
	void saveLastDevice(const QString& lastDevice)
	{
		TACPreferences::saveLastDevice(lastDevice.toStdString());
	}
};

#endif // QTACPREFERENCES_H
