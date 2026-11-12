// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TickCount.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_LINUX
#include <time.h>
#endif

#ifdef Q_OS_DARWIN
#include <mach/mach_time.h>
#endif

#include <QThread>

uint64_t tickCount()
{
	uint64_t result(0);

#ifdef Q_OS_WIN
	result = (uint64_t) ::GetTickCount();
#endif

#ifdef Q_OS_LINUX
	timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
	{
		result = (uint64_t) ts.tv_sec * (uint64_t) 1000;
		result += (uint64_t) ts.tv_nsec / (uint64_t) 1000000;
	}
#endif

#ifdef Q_OS_DARWIN
	result = mach_absolute_time() / NSEC_PER_MSEC;
#endif

	return result;
}

bool sleepUntil
(
	std::function<bool ()> test,
	uint64_t milliseconds
)
{
	uint64_t then = tickCount() + milliseconds;

	while (then > tickCount())
	{
		if (test())
			return true;

		QThread::msleep(100);
	}

	return false;
}
