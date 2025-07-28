#ifndef SYSTEMUSERNAME_H
#define SYSTEMUSERNAME_H

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
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
