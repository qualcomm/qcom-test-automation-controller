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
// Copyright 2013-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "UDLCommandLine.h"
#include "version.h"

// Qt
#include <QCoreApplication>

UDLCommandLine::UDLCommandLine
(
	const QStringList& args
) :
	CommandLineParser(args)
{
	QCoreApplication* instance = QCoreApplication::instance();

	addHelpLine(instance->applicationName());
	addHelpLine("Update TAC Device Configurations");
	addHelpLine(QString("Version: %1").arg(UPDATE_DEVICE_LIST));

	addHelpLine("");
	addHelpLine();
	addHelpLine("");
	addHelpLine("Options");
	addHelpLine("   -v, verbose                   -- Write verbose diagnostics when run");
	addHelpLine("");
	addHelpLine("   dir=<text>                    -- Sets a user directory to process. (i.e.) dir=C:/pathtofiles");

	_verbose = isSet(QStringList{} << "-v" << "verbose");
	_dir = value("dir");
}
