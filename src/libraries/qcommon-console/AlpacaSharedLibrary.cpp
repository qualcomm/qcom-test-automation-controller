// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
