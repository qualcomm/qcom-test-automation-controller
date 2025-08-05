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
			Biswajit Roy (biswroy@qti.qualcomm.com)
			Anmol Jaiswal (anmojais@qti.qualcomm.com)
*/

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

	// _licenseManager->release();
	// _licenseManager = Q_NULLPTR;

	// _telematicsManager->release();
	// _telematicsManager = Q_NULLPTR;

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

	// _licenseManager = LicenseManager::getInstance(*this);
	// if (_licenseManager->valid() == false)
	// {
	//     _licenseManager = Q_NULLPTR;
	// }

	// _telematicsManager = TelematicsManager::getInstance(*this);
	// if (_telematicsManager)
	//     _telematicsManager->initialize(kProductID);
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

		if (_preferences != Q_NULLPTR)
		{
			logPath = QDir::cleanPath(_preferences->appLogPath() + QDir::separator() + _ThreadedLog::createLogName(_appName + "_"));
		}
		else
		{
			logPath = QDir::cleanPath(defaultGlobalLoggingPath() + QDir::separator() + _ThreadedLog::createLogName(_appName + "_"));
		}

		_appThreadedLog = _ThreadedLog::createThreadedLog();
		_appThreadedLog->open(logPath);

		writeToAppLog(_appName + " Application log started\n");
		writeToAppLog("QTAC Version: " + QString(ALPACA_VERSION) + "\n");
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
			logPath = QDir::cleanPath(_preferences->runLogPath() + QDir::separator() + _ThreadedLog::createLogName(_appName + "_"));
		}
		else
		{
			logPath = QDir::cleanPath(defaultLoggingPath(_appName) + QDir::separator() + _ThreadedLog::createLogName(_appName + "_"));
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

	// if (_licenseManager != Q_NULLPTR)
	// {
	//     if (_licenseManager->checkLicence(productID, featureID) == false)
	//     {
	//         eLimeReturnCode lastReturnCode = _licenseManager->lastLimeError();
	//         if (lastReturnCode != LIME_LICENSE_UNAVAILABLE)
	//         {
	//             QByteArray message = licenseCheckFailed() + "\n";
	//             writeToAppLog(message);
	//         }
	//     }
	//     else
	//     {
	//         result = true;
	//     }
	// }
	// else
	// {
	//     QByteArray message = licenseManagerFailed() + "\n";
	//     writeToAppLog(message);
	// }

	return result;
}

bool AppCore::isLicenseManagerValid()
{
	return true;
	// return _licenseManager != Q_NULLPTR;
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
