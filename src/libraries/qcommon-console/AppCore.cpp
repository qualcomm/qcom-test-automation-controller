// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AppCore.h"

// QCommon
#include "AlpacaDefines.h"
#include "ConsoleApplicationEnhancements.h"

// Qt
#include <QDate>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

const QString kWindowsInternalBuildPath(QStringLiteral("C:\\Program Files (x86)\\Qualcomm\\QTAC\\ib.conf"));
const QString kLinuxInternalBuildPath(QStringLiteral("/opt/qcom/QTAC/bin/ib.conf"));

AppCore* AppCore::_appCore{Q_NULLPTR};

class AppCoreDestructor
{
public:
	AppCoreDestructor()
	{
	}

	~AppCoreDestructor()
	{
		if (AppCore::_appCore != Q_NULLPTR)
		{
			delete AppCore::_appCore;
			AppCore::_appCore = Q_NULLPTR;
		}
	}
} gAppCoreDestructor;

AppCore::AppCore()
{
	initializeStringProof();

	Q_ASSERT(AppCore::_appCore == Q_NULLPTR); // <--- Only functions in debug

	if (AppCore::_appCore == Q_NULLPTR) // <--- Check for release?
		AppCore::_appCore = this;

#ifdef Q_OS_WINDOWS
	_internalBuild = QFile::exists(kWindowsInternalBuildPath);
#endif
#ifdef Q_OS_LINUX
	_internalBuild = QFile::exists(kLinuxInternalBuildPath);
#endif
}

AppCore::~AppCore()
{
	ThreadedLog nullLog(Q_NULLPTR);

    if (_runThreadedLog.isNull() == false)
    {
        _runThreadedLog->close();
        _appThreadedLog->deleteLater();
        _runThreadedLog = nullLog;
    }

    if (_appThreadedLog.isNull() == false)
    {
        _appThreadedLog->close();
        _appThreadedLog->deleteLater();
        _appThreadedLog = nullLog;
    }
}

AppCore *AppCore::getAppCore()
{
	if (AppCore::_appCore == Q_NULLPTR)
	{
		AppCore::_appCore = new AppCore;
	}

	return AppCore::_appCore;
}

void AppCore::setPreferences
(
	PreferencesBase* preferences
)
{
	_preferences = preferences;
	_appName = _preferences->appName();
	_appVersion = _preferences->appVersion();

	setAppLogging(_preferences->loggingActive());
}

uint32_t AppCore::daysSinceInstall()
{
	uint32_t result{0};

	QString buildDateStr = QString::fromStdString(kCompileDate).trimmed();

	buildDateStr = buildDateStr.replace("  ", " ");

#ifdef _DEBUG
	//buildDateStr = "JAN 1 2021";
#endif

	QDate buildDateTime = QDate::fromString(buildDateStr,"MMM d yyyy");
	result = buildDateTime.daysTo(QDate::currentDate());

	if (result > 90)
	{
		QString message = "Current Date:" + QDate::currentDate().toString();
		writeToAppLog(message.toStdString());

		message = "Build Date:" + buildDateTime.toString();
		writeToAppLog(message.toStdString());

		QSettings alpacaSettings;
		alpacaSettings.setValue("dateCheckNotified", false);

		result = false;
	}

	return result;
}

void AppCore::setAppLogging
(
	bool loggingState
)
{
	if (_appThreadedLog.isNull() == false)
	{
		_appThreadedLog->close();
		_appThreadedLog = ThreadedLog(Q_NULLPTR);
	}

	if (loggingState == true)
	{
		QString logPath;

		if (_preferences != Q_NULLPTR)
		{
			logPath = QDir::cleanPath(QString::fromStdString(_preferences->appLogPath()) + QDir::separator() + _ThreadedLog::createLogName(QString::fromStdString(_appName + "_")));
		}
		else
		{
			logPath = QDir::cleanPath(QString::fromStdString(defaultGlobalLoggingPath()) + QDir::separator() + _ThreadedLog::createLogName(QString::fromStdString(_appName + "_")));
		}

		_appThreadedLog = _ThreadedLog::createThreadedLog();
		_appThreadedLog->open(logPath);

		writeToAppLog(_appName + " Application log started\n");
		writeToAppLog("QTAC Version: " + QString(QTAC_VERSION).toStdString() + "\n");
		writeToAppLog(_appName + " Version: " + _appVersion + "\n");
	}
}

bool AppCore::appLoggingActive()
{
	return _appThreadedLog.isNull() == false;
}

void AppCore::setRunLogging(bool loggingState)
{
	if (_runThreadedLog.isNull() == false)
	{
		_runThreadedLog->close();
		_runThreadedLog = ThreadedLog(Q_NULLPTR);
	}

	if (loggingState == true)
	{
		QString logPath;

		if (_preferences != Q_NULLPTR)
		{
			logPath = QDir::cleanPath(QString::fromStdString(_preferences->runLogPath()) + QDir::separator() + _ThreadedLog::createLogName(QString::fromStdString(_appName + "_")));
		}
		else
		{
			logPath = QDir::cleanPath(QString::fromStdString(defaultLoggingPath(_appName)) + QDir::separator() + _ThreadedLog::createLogName(QString::fromStdString(_appName + "_")));
		}

		_runThreadedLog = _ThreadedLog::createThreadedLog();
		_runThreadedLog->open(logPath);
		_runThreadedLog->addLogEntry("Starting Log: " + QDateTime::currentDateTime().toString() + "\n");
	}
}

bool AppCore::runLoggingActive()
{
	return _runThreadedLog.isNull() == false;
}

std::string AppCore::loggingPath()
{
	std::string result{defaultLoggingPath(_appName)};

	if (_preferences != Q_NULLPTR)
		result = _preferences->runLogPath();

	return result;
}

bool AppCore::checkLicense
(
		const std::string& productID,
		const std::string& featureID
)
{
	bool result{true};

	Q_UNUSED(productID)
	Q_UNUSED(featureID)

	return result;
}

bool AppCore::isLicenseManagerValid()
{
	return true;
}

void AppCore::postStartEvent()
{

}

void AppCore::postAutomationEvent()
{

}

void AppCore::postMetric
(
	const std::string& metricID,
	double metric
)
{
	Q_UNUSED(metricID)
	Q_UNUSED(metric)

	// if (_telematicsManager != Q_NULLPTR)
	// {
	//     _telematicsManager->trackMetric(metricID, metric);
	// }
}

void AppCore::writeToAppLog(const std::string &writeMe)
{
	if (_appThreadedLog.isNull() == false)
		_appThreadedLog->addLogEntry("[" + QTime::currentTime().toString("hh:mm:ss:zzz") + "]: " + QString::fromStdString(writeMe));
}

void AppCore::writeToApplicationLog(const std::string& writeMe)
{
	AppCore* appCore = AppCore::getAppCore();
	if (appCore != Q_NULLPTR)
		appCore->writeToAppLog(writeMe);
}

void AppCore::writeToApplicationLogLine
(
	const std::string& writeMe
)
{
	AppCore::writeToApplicationLog(writeMe + "\n");
}

void AppCore::writeToRunLog(const std::string& writeMe)
{
	if (_runThreadedLog.isNull() == false)
		_runThreadedLog->addLogEntry(QString::fromStdString(writeMe));
}

void AppCore::writeToRuntimeLog(const std::string& writeMe)
{
	AppCore* appCore = AppCore::getAppCore();
	if (appCore != Q_NULLPTR)
		appCore->writeToRunLog(writeMe);
}
