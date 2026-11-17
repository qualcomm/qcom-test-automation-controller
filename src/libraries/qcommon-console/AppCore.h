#ifndef APPCORE_H
#define APPCORE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonConsoleGlobal.h"
#include "PreferencesBase.h"
#include "ThreadedLog.h"

#include <cstdint>
#include <string>

class QCOMMONCONSOLE_EXPORT AppCore
{
friend class AppCoreDestructor;

public:
	~AppCore();

	static AppCore* getAppCore();

	std::string appName()
	{
		return _appName;
	}

	std::string appVersion()
	{
		return _appVersion;
	}

	PreferencesBase* getPreferences()
	{
		return _preferences;
	}
	void setPreferences(PreferencesBase* preferences);

	void setAppLogging(bool loggingState);
	bool appLoggingActive();

	std::string currentAppLog()
	{
		return _appThreadedLog ? _appThreadedLog->currentLogPath() : std::string();
	}

	void setRunLogging(bool loggingState);
	bool runLoggingActive();
	std::string currentRunLog()
	{
		return _runThreadedLog ? _runThreadedLog->currentLogPath() : std::string();
	}

	std::string loggingPath();

	// --- Critical Application Paths (Cannot be configured per user preferences) ---
	static std::string applicationBinPath();
	static std::string applicationDataPath();
	static std::string firmwarePath();
	static std::string configurationsPath();
	static std::string userConfigurationsPath();
	static std::string configurationSavePath(const std::string& currentPath = "");

	void writeToAppLog(const std::string& writeMe);

	static void writeToApplicationLog(const std::string& writeMe);
	static void writeToApplicationLogLine(const std::string& writeMe);

	void writeToRunLog(const std::string& writeMe);
	static void writeToRuntimeLog(const std::string& writeMe);

private:
	AppCore();

	// This isn't a singleton.  It's just a convenience for saving and restoring appcore
	static AppCore*				_appCore;

	bool						_internalBuild{false};
	std::string					_appName;
	std::string					_appVersion;

	PreferencesBase*			_preferences{nullptr};
	ThreadedLog					_appThreadedLog;
	ThreadedLog					_runThreadedLog;
};

#endif // APPCORE_H
