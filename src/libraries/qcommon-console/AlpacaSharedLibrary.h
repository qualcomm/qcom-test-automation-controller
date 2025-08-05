#ifndef ALPACASHAREDLIBRARY_H
#define ALPACASHAREDLIBRARY_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// QCommon
#include "AppCore.h"
#include "PreferencesBase.h"

class QCOMMONCONSOLE_EXPORT AlpacaSharedLibrary
{
public:
	explicit AlpacaSharedLibrary();
	~AlpacaSharedLibrary();

	bool initialize(const QByteArray& appName, const QByteArray& appVersion, PreferencesBase* preferencesBase);

	static AppCore* getAppCore();

	bool licenseIsValid();

	void setLoggingState(bool state);
	bool getLoggingState();

	QByteArray lastError()
	{
		QByteArray result{_lastError};

		_lastError.clear();
		return result;
	}

	void setLastError(const QByteArray& lastError)
	{
		_lastError = lastError;
	}
protected:
	QString						_appName;
	QString						_appVersion;
	QByteArray					_lastError;
	static AppCore*				_appCore;
	bool						_validLicense{true};
};

#endif // ALPACASHAREDLIBRARY_H
