#ifndef ALPACASHAREDLIBRARY_H
#define ALPACASHAREDLIBRARY_H
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
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
