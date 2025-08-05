// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "mymemcpy.h"

#include <string.h>

// memcpy is decprected on Windows, memcpy_s doesn't exist on GCC 10
void my_memcpy
(
	void* destination,
	quint64 destinationSize,
	const void* source,
	quint64 copyBytes
)
{
	memset(destination, 0, destinationSize);

#ifdef Q_OS_UNIX
	memcpy(destination, source, copyBytes);
#endif

#ifdef Q_OS_WIN
	memcpy_s(destination, destinationSize, source, copyBytes);
#endif
}

