// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AlpacaSettings.h"

AlpacaSettings::AlpacaSettings()
	: IniSettings("Alpaca")
{
}

AlpacaSettings::AlpacaSettings
(
	const std::string& appName
) :
	IniSettings(appName)
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
	IniSettings(organization, appName)
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
