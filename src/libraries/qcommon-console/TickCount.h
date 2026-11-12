#ifndef TICKCOUNT_H
#define TICKCOUNT_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

// C++
#include <cstdint>
#include <functional>

uint64_t QCOMMONCONSOLE_EXPORT tickCount();

bool QCOMMONCONSOLE_EXPORT sleepUntil(std::function<bool(void)> test, uint64_t milliseconds);

#endif // TICKCOUNT_H
