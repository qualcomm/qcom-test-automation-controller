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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "PineCommandLine.h"

// Windows
#ifdef Q_OS_WIN
	#include <windows.h>
#endif

#include "ConsoleApplicationEnhancements.h"

PineCommandLine::PineCommandLine()
{

}

void PineCommandLine::invokeCli()
{
#ifdef Q_OS_WIN
	QByteArray cmdPath = "C:\\Windows\\System32\\cmd.exe";
	QByteArray workingDir = documentsDataPath("TAC Configurations").toLatin1();
	QByteArray cmdArgs = "/K title PINE Client & echo The terminal can be used to execute Packet Interface Editor (PINE) CLI commands. & echo "
					"PINE can be installed from the Qualcomm Package Manager (QPM). If you encounter issues while using the PINE CLI, & echo "
					"please reachout to the PINE support at pine.support@qti.qualcomm.com. & echo ----------------------- & echo USAGE: pine -helpalpaca";


	ShellExecuteA(NULL, NULL, cmdPath, cmdArgs, workingDir, 1);
#endif
}
