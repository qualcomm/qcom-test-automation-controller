// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AppCore.h"

// QCommon
#include "AlpacaDefines.h"
#include "ConsoleApplicationEnhancements.h"

// Qt
#include <QCoreApplication>
#include <QDate>
#include <QDir>
#include <QFile>
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

	// The fixed locations above only match one product layout. Also accept
	// ib.conf next to the executable, which is correct for every layout.
	// applicationDirPath() needs a QCoreApplication, and AppCore can be
	// created by library hosts that have none.
	if (_internalBuild == false && QCoreApplication::instance() != Q_NULLPTR)
	{
		_internalBuild = QFile::exists(applicationBinPath() + "ib.conf");
	}
}

AppCore::~AppCore()
{
	ThreadedLog nullLog(Q_NULLPTR);

	// The logs are owned by QSharedPointer, so resetting them releases the
	// object. Calling deleteLater() as well would delete it a second time.
	// close() waits for the log thread to finish before the reset.
	if (_runThreadedLog.isNull() == false)
	{
		_runThreadedLog->close();
		_runThreadedLog = nullLog;
	}

	if (_appThreadedLog.isNull() == false)
	{
		_appThreadedLog->close();
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

quint32 AppCore::daysSinceInstall()
{
	quint32 result{0};

	QString buildDateStr = kCompileDate.trimmed();

	buildDateStr = buildDateStr.replace("  ", " ");

#ifdef _DEBUG
	//buildDateStr = "JAN 1 2021";
#endif

	QDate buildDateTime = QDate::fromString(buildDateStr,"MMM d yyyy");
	result = buildDateTime.daysTo(QDate::currentDate());

	if (result > 90)
	{
		QString message = "Current Date:" + QDate::currentDate().toString();
		writeToAppLog(message);

		message = "Build Date:" + buildDateTime.toString();
		writeToAppLog(message);

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
		QString logDirectory;

		if (_preferences != Q_NULLPTR)
		{
			logDirectory = _preferences->appLogPath();
		}
		else
		{
			logDirectory = defaultGlobalLoggingPath();
		}

		// An empty directory means no writable log location could be found.
		// Appending to it would produce a path at the filesystem root, so skip
		// logging rather than attempt a write that cannot succeed.
		if (logDirectory.isEmpty())
		{
			return;
		}

		logPath = QDir::cleanPath(logDirectory + QDir::separator() + _ThreadedLog::createLogName(_appName + "_"));

		_appThreadedLog = _ThreadedLog::createThreadedLog();
		if (_appThreadedLog.isNull())
		{
			return;
		}

		_appThreadedLog->open(logPath);

		writeToAppLog(_appName + " Application log started\n");
		writeToAppLog("QTAC Version: " + QString(QTAC_VERSION) + "\n");
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
		QString logDirectory;

		if (_preferences != Q_NULLPTR)
		{
			logDirectory = _preferences->runLogPath();
		}
		else
		{
			logDirectory = defaultLoggingPath(_appName);
		}

		// See setAppLogging(): without a writable directory there is nowhere to
		// log, and building a path anyway would target the filesystem root.
		if (logDirectory.isEmpty())
		{
			return;
		}

		logPath = QDir::cleanPath(logDirectory + QDir::separator() + _ThreadedLog::createLogName(_appName + "_"));

		_runThreadedLog = _ThreadedLog::createThreadedLog();
		if (_runThreadedLog.isNull())
		{
			return;
		}

		_runThreadedLog->open(logPath);
		_runThreadedLog->addLogEntry("Starting Log: " + QDateTime::currentDateTime().toString() + "\n");
	}
}

bool AppCore::runLoggingActive()
{
	return _runThreadedLog.isNull() == false;
}

QString AppCore::loggingPath()
{
	QString result{defaultLoggingPath(_appName)};

	if (_preferences != Q_NULLPTR)
		result = _preferences->runLogPath();

	return result;
}

bool AppCore::checkLicense
(
		const QByteArray& productID,
		const QByteArray& featureID
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
	const QByteArray& metricID,
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

void AppCore::writeToAppLog(const QString &writeMe)
{
	if (_appThreadedLog.isNull() == false)
		_appThreadedLog->addLogEntry("[" + QTime::currentTime().toString("hh:mm:ss:zzz") + "]: " + writeMe);
}

void AppCore::writeToApplicationLog(const QString& writeMe)
{
	AppCore* appCore = AppCore::getAppCore();
	if (appCore != Q_NULLPTR)
		appCore->writeToAppLog(writeMe);
}

void AppCore::writeToApplicationLogLine
(
	const QString& writeMe
)
{
	AppCore::writeToApplicationLog(writeMe + QString("\n"));
}

void AppCore::writeToRunLog(const QString& writeMe)
{
	if (_runThreadedLog.isNull() == false)
		_runThreadedLog->addLogEntry(writeMe);
}

void AppCore::writeToRuntimeLog(const QString& writeMe)
{
	AppCore* appCore = AppCore::getAppCore();
	if (appCore != Q_NULLPTR)
		appCore->writeToRunLog(writeMe);
}
