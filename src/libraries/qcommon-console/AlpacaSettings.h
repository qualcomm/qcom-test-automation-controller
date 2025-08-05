#ifndef ALPACASETTINGS_H
#define ALPACASETTINGS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "QCommonConsoleGlobal.h"

// Qt
#include <QSettings>

class QCOMMONCONSOLE_EXPORT AlpacaSettings :
	public QSettings
{
	Q_OBJECT

public:
	AlpacaSettings(const QString& appName);
	~AlpacaSettings();
};

#endif // ALPACASETTINGS_H
