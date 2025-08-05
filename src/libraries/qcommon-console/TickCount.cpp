// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

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

quint64 tickCount()
{
	quint64 result(0);

#ifdef Q_OS_WIN
	result = (quint64) ::GetTickCount();
#endif

#ifdef Q_OS_LINUX
	timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
	{
		result = (quint64) ts.tv_sec * (quint64) 1000;
		result += (quint64) ts.tv_nsec / (quint64) 1000000;
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
	quint64 milliseconds
)
{
	quint64 then = tickCount() + milliseconds;

	while (then > tickCount())
	{
		if (test())
			return true;

		QThread::msleep(100);
	}

	return false;
}
