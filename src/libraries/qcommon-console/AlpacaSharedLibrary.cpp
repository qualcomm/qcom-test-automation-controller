// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AlpacaSharedLibrary.h"

// QCommon
#include "QCommonConsole.h"

AppCore* AlpacaSharedLibrary::_appCore{nullptr};

AlpacaSharedLibrary::AlpacaSharedLibrary()
{
}

AlpacaSharedLibrary::~AlpacaSharedLibrary()
{
}

AppCore* AlpacaSharedLibrary::getAppCore()
{
	if (AlpacaSharedLibrary::_appCore == nullptr)
	{
		AlpacaSharedLibrary::_appCore = AppCore::getAppCore();
	}

	return AlpacaSharedLibrary::_appCore;
}

bool AlpacaSharedLibrary::initialize
(
	const std::string &appName,
	const std::string &appVersion,
	PreferencesBase* preferencesBase
)
{
	bool result{false};

	_appName = appName;
	_appVersion = appVersion;

	AppCore* appCore = getAppCore();
	if (appCore != nullptr)
	{
		appCore->setPreferences(preferencesBase);

		InitializeQCommonConsole();
	}

	return result;
}

bool AlpacaSharedLibrary::licenseIsValid()
{
	// disallow license validation for open-source
	return _validLicense;
}

void AlpacaSharedLibrary::setLoggingState(bool state)
{
	AppCore* appCore = getAppCore();
	if (appCore != nullptr)
	{
		appCore->setAppLogging(state);
	}
}

bool AlpacaSharedLibrary::getLoggingState()
{
	bool result{false};

	AppCore* appCore = getAppCore();
	if (appCore != nullptr)
	{
		result = appCore->appLoggingActive();
	}

	return result;
}
