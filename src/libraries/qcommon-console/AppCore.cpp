// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AppCore.h"

// QCommon
#include "AlpacaDefines.h"
#include "AlpacaSettings.h"
#include "ConsoleApplicationEnhancements.h"
#include "StringUtilities.h"

#include <cassert>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>

const std::string kWindowsInternalBuildPath("C:\\Program Files (x86)\\Qualcomm\\QTAC\\ib.conf");
const std::string kLinuxInternalBuildPath("/opt/qcom/QTAC/bin/ib.conf");

AppCore* AppCore::_appCore{nullptr};

class AppCoreDestructor
{
public:
	AppCoreDestructor()
	{
	}

	~AppCoreDestructor()
	{
		if (AppCore::_appCore != nullptr)
		{
			delete AppCore::_appCore;
			AppCore::_appCore = nullptr;
		}
	}
} gAppCoreDestructor;

AppCore::AppCore()
{
	initializeStringProof();

	assert(AppCore::_appCore == nullptr);

	if (AppCore::_appCore == nullptr)
		AppCore::_appCore = this;

#if defined(_WIN32) || defined(Q_OS_WINDOWS)
	_internalBuild = std::filesystem::exists(kWindowsInternalBuildPath);
#endif
#if defined(__linux__) || defined(Q_OS_LINUX)
	_internalBuild = std::filesystem::exists(kLinuxInternalBuildPath);
#endif
}

AppCore::~AppCore()
{
	if (_runThreadedLog)
	{
		_runThreadedLog->close();
		_runThreadedLog = nullptr;
	}

	if (_appThreadedLog)
	{
		_appThreadedLog->close();
		_appThreadedLog = nullptr;
	}
}

AppCore *AppCore::getAppCore()
{
	if (AppCore::_appCore == nullptr)
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

	std::istringstream iss(kCompileDate);
	std::string monthStr;
	int day = 0;
	int year = 0;
	if (iss >> monthStr >> day >> year)
	{
		static const char* const kMonths[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		int month = -1;
		for (int i = 0; i < 12; ++i)
		{
			if (monthStr == kMonths[i])
			{
				month = i;
				break;
			}
		}

		if (month >= 0)
		{
			std::tm buildTm = {};
			buildTm.tm_year = year - 1900;
			buildTm.tm_mon = month;
			buildTm.tm_mday = day;
			std::time_t buildTime = std::mktime(&buildTm);

			std::time_t now = std::time(nullptr);
			std::tm nowTm = *std::localtime(&now);
			nowTm.tm_hour = 0;
			nowTm.tm_min = 0;
			nowTm.tm_sec = 0;
			std::time_t nowDate = std::mktime(&nowTm);

			double diffSeconds = std::difftime(nowDate, buildTime);
			if (diffSeconds > 0)
			{
				result = static_cast<uint32_t>(diffSeconds / (60 * 60 * 24));
			}

			if (result > 90)
			{
				std::string message = "Current Date: " + formatDateTime("%a %b %d %Y");
				writeToAppLog(message);

				message = "Build Date: " + monthStr + " " + std::to_string(day) + " " + std::to_string(year);
				writeToAppLog(message);

				AlpacaSettings alpacaSettings;
				alpacaSettings.setValue("dateCheckNotified", false);

				result = 0;
			}
		}
	}

	return result;
}

void AppCore::setAppLogging
(
	bool loggingState
)
{
	if (_appThreadedLog)
	{
		_appThreadedLog->close();
		_appThreadedLog = nullptr;
	}

	if (loggingState == true)
	{
		std::string logPath;

		if (_preferences != nullptr)
		{
			std::filesystem::path p = std::filesystem::path(_preferences->appLogPath()) / _ThreadedLog::createLogName(_appName + "_");
			logPath = p.lexically_normal().string();
		}
		else
		{
			std::filesystem::path p = std::filesystem::path(defaultGlobalLoggingPath()) / _ThreadedLog::createLogName(_appName + "_");
			logPath = p.lexically_normal().string();
		}

		_appThreadedLog = _ThreadedLog::createThreadedLog();
		_appThreadedLog->open(logPath);

		writeToAppLog(_appName + " Application log started\n");
		writeToAppLog("QTAC Version: " + std::string(QTAC_VERSION) + "\n");
		writeToAppLog(_appName + " Version: " + _appVersion + "\n");
	}
}

bool AppCore::appLoggingActive()
{
	return _appThreadedLog != nullptr;
}

void AppCore::setRunLogging(bool loggingState)
{
	if (_runThreadedLog)
	{
		_runThreadedLog->close();
		_runThreadedLog = nullptr;
	}

	if (loggingState == true)
	{
		std::string logPath;

		if (_preferences != nullptr)
		{
			std::filesystem::path p = std::filesystem::path(_preferences->runLogPath()) / _ThreadedLog::createLogName(_appName + "_");
			logPath = p.lexically_normal().string();
		}
		else
		{
			std::filesystem::path p = std::filesystem::path(defaultLoggingPath(_appName)) / _ThreadedLog::createLogName(_appName + "_");
			logPath = p.lexically_normal().string();
		}

		_runThreadedLog = _ThreadedLog::createThreadedLog();
		_runThreadedLog->open(logPath);
		_runThreadedLog->addLogEntry("Starting Log: " + currentDateTimeString() + "\n");
	}
}

bool AppCore::runLoggingActive()
{
	return _runThreadedLog != nullptr;
}

std::string AppCore::loggingPath()
{
	std::string result{defaultLoggingPath(_appName)};

	if (_preferences != nullptr)
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

	(void)productID;
	(void)featureID;

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
	(void)metricID;
	(void)metric;
}

void AppCore::writeToAppLog(const std::string &writeMe)
{
	if (_appThreadedLog)
	{
		auto now = std::chrono::system_clock::now();
		auto timeT = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
		std::tm localTm = *std::localtime(&timeT);
		char timeBuf[32];
		std::snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d:%03d",
			localTm.tm_hour, localTm.tm_min, localTm.tm_sec, static_cast<int>(ms.count()));
		_appThreadedLog->addLogEntry(std::string("[") + timeBuf + "]: " + writeMe);
	}
}

void AppCore::writeToApplicationLog(const std::string& writeMe)
{
	AppCore* appCore = AppCore::getAppCore();
	if (appCore != nullptr)
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
	if (_runThreadedLog)
		_runThreadedLog->addLogEntry(writeMe);
}

void AppCore::writeToRuntimeLog(const std::string& writeMe)
{
	AppCore* appCore = AppCore::getAppCore();
	if (appCore != nullptr)
		appCore->writeToRunLog(writeMe);
}
