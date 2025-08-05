// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
