#ifndef TICKCOUNT_H
#define TICKCOUNT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// C++
#include <functional>

// Qt
#include <QtGlobal>

quint64 QCOMMONCONSOLE_EXPORT tickCount();

bool QCOMMONCONSOLE_EXPORT sleepUntil(std::function<bool(void)> test, quint64 milliseconds);

#endif // TICKCOUNT_H
