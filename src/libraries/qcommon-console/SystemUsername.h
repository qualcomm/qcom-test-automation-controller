#ifndef SYSTEMUSERNAME_H
#define SYSTEMUSERNAME_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include <QString>

#ifdef Q_OS_WIN
#include <windows.h>
#include <winevt.h>
#pragma comment(lib, "wevtapi.lib")
#define ARRAY_SIZE 1
#define TIMEOUT 1000  // 1 second; Set and use in place of INFINITE in EvtNext call
#endif

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

/**
 * @brief The SystemUsername class - The base class to determine the logged in username of the user across all platforms.
 */
class SystemUsername
{
public:
	SystemUsername();
	QString userName();
private:
	void getUserNameWin();
	void getUserNameLinux();

#ifdef Q_OS_WINDOWS
	DWORD fetchLatestEvent(EVT_HANDLE hResults);
	DWORD parseEvent(EVT_HANDLE hEvent);
#endif
	QString                     _userName;
};

#endif // SYSTEMUSERNAME_H
