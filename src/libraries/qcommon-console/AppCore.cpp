// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AppCore.h"

// QCommon
#include "AlpacaDefines.h"
#include "AlpacaSettings.h"
#include "ConsoleApplicationEnhancements.h"
#include "StringUtilities.h"

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#if defined(__linux__)
#include <unistd.h>
#endif

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>

static const std::string kDefaultAppName("QTAC");

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

void AppCore::writeToAppLog(const std::string &writeMe)
{
	if (_appThreadedLog)
	{
		auto now = std::chrono::system_clock::now();
		auto timeT = std::chrono::system_clock::to_time_t(now);
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
		std::tm localTm{};
#if defined(_WIN32)
		localtime_s(&localTm, &timeT);
#else
		localtime_r(&timeT, &localTm);
#endif
		char timeBuf[64];
		std::snprintf(timeBuf, sizeof(timeBuf), "%04d-%02d-%02d %02d:%02d:%02d:%03d",
			localTm.tm_year + 1900, localTm.tm_mon + 1, localTm.tm_mday,
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

// --- Central Path Management (Single Source of Truth) ---

std::string AppCore::applicationBinPath()
{
	std::string binDir;
#if defined(_WIN32) || defined(_WIN64)
	char buffer[MAX_PATH];
	DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
	if (len > 0)
	{
		binDir = std::filesystem::path(buffer).parent_path().generic_string();
	}
#elif defined(__linux__)
	char buffer[1024];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (len != -1)
	{
		buffer[len] = '\0';
		binDir = std::filesystem::path(buffer).parent_path().generic_string();
	}
#endif
	if (binDir.empty())
		binDir = ".";

	std::filesystem::path p = std::filesystem::path(binDir).lexically_normal();
	std::string result = p.generic_string();
	if (result.empty() || result.back() != '/')
		result += '/';
	return result;
}

std::string AppCore::applicationDataPath()
{
	std::error_code ec;

	// Installed production data path
#if defined(_WIN32) || defined(_WIN64)
	const std::string installedPath = "C:/ProgramData/Qualcomm/" + kDefaultAppName;
#elif defined(__linux__)
	const std::string installedPath = "/var/lib/qcom/data/" + kDefaultAppName;
#else
	const std::string installedPath = "./";
#endif

	// 1. If running as installed software (e.g. Program Files / /opt)
	std::string binPath = applicationBinPath();
	std::string lowerBin = toLower(binPath);
	bool isInstalledBin = (lowerBin.find("program files") != std::string::npos ||
	                       lowerBin.find("/opt/qcom") != std::string::npos);

	if (isInstalledBin && std::filesystem::exists(installedPath + "/configurations/devicelist.json", ec) && !ec)
	{
		return installedPath;
	}

	// 2. Dev builds (Debug or Release compiled from source):
	// Check parent directories of the executable for project configurations
	std::filesystem::path current = std::filesystem::path(binPath).lexically_normal();
	for (int depth = 0; depth < 5 && !current.empty() && current != current.parent_path(); ++depth, current = current.parent_path())
	{
		if (std::filesystem::exists(current / "configurations" / "devicelist.json", ec) && !ec)
		{
			return current.lexically_normal().generic_string();
		}
	}

	// 3. Fallback: If installed configurations exist, use installed path
	if (std::filesystem::exists(installedPath + "/configurations/devicelist.json", ec) && !ec)
	{
		return installedPath;
	}

	return installedPath;
}

std::string AppCore::configurationsPath()
{
	std::string dataPath = applicationDataPath();
	std::string result = (std::filesystem::path(dataPath) / "configurations").lexically_normal().generic_string();
	if (result.empty() || result.back() != '/')
		result += '/';
	return result;
}

std::string AppCore::userConfigurationsPath()
{
	return documentsDataPath("TAC Configurations");
}

std::string AppCore::configurationSavePath(const std::string& currentPath)
{
	if (currentPath.empty())
	{
		return userConfigurationsPath();
	}

	std::error_code ec;
	std::filesystem::path currentP = std::filesystem::absolute(currentPath, ec).lexically_normal();
	std::string currentNorm = toLower(currentP.generic_string());

	// Check against system configurations path (e.g. C:/ProgramData/Qualcomm/QTAC/configurations or repo/configurations)
	std::filesystem::path sysP = std::filesystem::absolute(configurationsPath(), ec).lexically_normal();
	std::string sysNorm = toLower(sysP.generic_string());
	if (!sysNorm.empty() && sysNorm.back() == '/')
		sysNorm.pop_back();

	// Check against installed production data root as well
#if defined(_WIN32) || defined(_WIN64)
	std::string prodNorm = "c:/programdata/qualcomm/" + toLower(kDefaultAppName);
#elif defined(__linux__)
	std::string prodNorm = "/var/lib/qcom/data/" + toLower(kDefaultAppName);
#else
	std::string prodNorm = "./";
#endif

	if (currentNorm.rfind(sysNorm, 0) == 0 || currentNorm.rfind(prodNorm, 0) == 0)
	{
		// Configurations should be saved in the user's documents location irrespective of configuration version
		return userConfigurationsPath();
	}

	return std::filesystem::path(currentPath).lexically_normal().generic_string();
}

