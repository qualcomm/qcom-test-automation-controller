// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
