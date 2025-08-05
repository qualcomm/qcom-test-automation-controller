#ifndef MYMEMCPY_H
#define MYMEMCPY_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QByteArray>

void QCOMMONCONSOLE_EXPORT my_memcpy(void* destination, quint64 destinationSize, const void* source, quint64 copyBytes);

#endif
