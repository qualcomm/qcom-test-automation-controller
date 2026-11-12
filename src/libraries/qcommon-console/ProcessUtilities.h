#ifndef PROCESSUTILITIES_H
#define PROCESSUTILITIES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <string>

bool QCOMMONCONSOLE_EXPORT isProcessRunning(const std::string& applicationName);
bool QCOMMONCONSOLE_EXPORT startProcess(const std::string& applicationName);

#endif // PROCESSUTILITIES_H
