#ifndef PROCESSUTILITIES_H
#define PROCESSUTILITIES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QString>

bool QCOMMONCONSOLE_EXPORT isProcessRunning(const QString& applicationName);
bool QCOMMONCONSOLE_EXPORT startProcess(const QString& applicationName);

#endif // PROCESSUTILITIES_H
