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

#include "AlpacaSharedLibrary.h"

// QCommon
#include "QCommonConsole.h"

// Qt
#include <QDateTime>
#include <QDir>

#include <QTextStream>

AppCore* AlpacaSharedLibrary::_appCore{Q_NULLPTR};

AlpacaSharedLibrary::AlpacaSharedLibrary()
{
}

AlpacaSharedLibrary::~AlpacaSharedLibrary()
{
}

AppCore* AlpacaSharedLibrary::getAppCore()
{
	if (AlpacaSharedLibrary::_appCore == Q_NULLPTR)
	{
		AlpacaSharedLibrary::_appCore = AppCore::getAppCore();
	}

	return AlpacaSharedLibrary::_appCore;
}

bool AlpacaSharedLibrary::initialize
(
	const QByteArray &appName,
	const QByteArray &appVersion,
	PreferencesBase* preferencesBase
)
{
	bool result{false};

	_appName = appName;
	_appVersion = appVersion;

	AppCore* appCore = getAppCore();
	if (appCore != Q_NULLPTR)
	{
		appCore->setPreferences(preferencesBase);

		InitializeQCommonConsole();
	}

	return result;
}

bool AlpacaSharedLibrary::licenseIsValid()
{
	/* let me know when LIME works
		if (_validLicense == false)
			_validLicense = _appCore.checkLicense(kProductID, kCoreFeature);

		*/

	return _validLicense;
}

void AlpacaSharedLibrary::setLoggingState(bool state)
{
	AppCore* appCore = getAppCore();
	if (appCore != Q_NULLPTR)
	{
		appCore->setAppLogging(state);
	}
}

bool AlpacaSharedLibrary::getLoggingState()
{
	bool result{false};

	AppCore* appCore = getAppCore();
	if (appCore != Q_NULLPTR)
	{
		result = appCore->appLoggingActive();
	}

	return result;
}
