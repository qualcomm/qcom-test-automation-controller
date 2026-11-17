#ifndef ALPACASETTINGS_H
#define ALPACASETTINGS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonConsoleGlobal.h"
#include "INISettings.h"

#include <string>

class QCOMMONCONSOLE_EXPORT AlpacaSettings :
	public INISettings
{
public:
	AlpacaSettings();
	explicit AlpacaSettings(const std::string& appName);
	AlpacaSettings(const std::string& organization, const std::string& appName);
	~AlpacaSettings() override;

private:
	bool _scopedGroup{false};
};

#endif // ALPACASETTINGS_H
