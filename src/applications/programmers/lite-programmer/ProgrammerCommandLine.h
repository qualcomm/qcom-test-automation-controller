#ifndef PROGRAMMERCOMMANDLINE_H
#define PROGRAMMERCOMMANDLINE_H
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
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "CommandLineParser.h"

const QString kAppName{"lite-programmer"};

class ProgrammerCommandLine :
	public CommandLineParser
{
public:
	ProgrammerCommandLine(const QStringList& args);

	bool listSet();
	bool listPlatformSet();
	bool getProgramming();
	bool programSet();

	QByteArray serialNumber();
	QByteArray modelName();
	quint32 platformID();

private:
	bool						_listSet{false};
	bool						_listPlatformSet{false};
	bool						_getProgramming{false};
	bool						_program{false};

	QString						_serialNumber;
	QString						_modelName;
	QString						_platformID;
};

#endif // PROGRAMMERCOMMANDLINE_H
