// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AlpacaSettings.h"

AlpacaSettings::AlpacaSettings()
	: INISettings("Alpaca")
{
}

AlpacaSettings::AlpacaSettings
(
	const std::string& appName
) :
	INISettings(appName)
{
	if (!appName.empty())
	{
		beginGroup(appName);
		_scopedGroup = true;
	}
}

AlpacaSettings::AlpacaSettings
(
	const std::string& organization,
	const std::string& appName
) :
	INISettings(organization, appName)
{
}

AlpacaSettings::~AlpacaSettings()
{
	if (_scopedGroup)
	{
		endGroup();
	}
	sync();
}
