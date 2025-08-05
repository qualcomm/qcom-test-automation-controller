// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "AlpacaSettings.h"

// Qt
#include <QCoreApplication>

AlpacaSettings::AlpacaSettings
(
	const QString& appName
)
{
	beginGroup(appName);
}

AlpacaSettings::~AlpacaSettings()
{
	endGroup(); // QTAC
}
