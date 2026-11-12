#ifndef MYMEMCPY_H
#define MYMEMCPY_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstddef>

void QCOMMONCONSOLE_EXPORT my_memcpy(void* destination, size_t destinationSize, const void* source, size_t copyBytes);

#endif
