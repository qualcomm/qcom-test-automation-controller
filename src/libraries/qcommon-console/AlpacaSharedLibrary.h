#ifndef ALPACASHAREDLIBRARY_H
#define ALPACASHAREDLIBRARY_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// QCommon
#include "AppCore.h"
#include "PreferencesBase.h"

#include <string>

class QCOMMONCONSOLE_EXPORT AlpacaSharedLibrary
{
public:
	explicit AlpacaSharedLibrary();
	~AlpacaSharedLibrary();

	bool initialize(const std::string& appName, const std::string& appVersion, PreferencesBase* preferencesBase);

	static AppCore* getAppCore();

	bool licenseIsValid();

	void setLoggingState(bool state);
	bool getLoggingState();

	std::string lastError()
	{
		std::string result{_lastError};

		_lastError.clear();
		return result;
	}

	void setLastError(const std::string& lastError)
	{
		_lastError = lastError;
	}
protected:
	std::string					_appName;
	std::string					_appVersion;
	std::string					_lastError;
	static AppCore*				_appCore;
	bool						_validLicense{true};
};

#endif // ALPACASHAREDLIBRARY_H
