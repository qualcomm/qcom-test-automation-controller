#ifndef ALPACASETTINGS_H
#define ALPACASETTINGS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonConsoleGlobal.h"

// Qt
#include <QSettings>

#include <string>

class QCOMMONCONSOLE_EXPORT AlpacaSettings :
	public QSettings
{
	Q_OBJECT

public:
	AlpacaSettings(const std::string& appName);
	~AlpacaSettings();
};

#endif // ALPACASETTINGS_H
