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
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "FTDICheckCommandLine.h"
#include "version.h"

// Qt
#include <QCoreApplication>

FTDICheckCommandLine::FTDICheckCommandLine(const QStringList& args) :
	CommandLineParser(args)
{
	QCoreApplication* instance = QCoreApplication::instance();

	addHelpLine(instance->applicationName());
	addHelpLine("ftdi-check verifies whether the appropriate FTDI drivers are installed on a machine and updates the FTDI driver to appropriate versions if required.");
	addHelpLine(QString("Version: %1").arg(FTDI_CHECK_VERSION));

	addHelpLine("");
	addHelpLine();
	addHelpLine("");
	addHelpLine( "Usage: validate | uninstall | install | version");
	addHelpLine("   validate:            Validates the currently installed driver");
	addHelpLine("   uninstall:           Uninstalls the current driver");
	addHelpLine("   install:             Installs the appropriate FTDI driver for QTAC");
	addHelpLine("   version:             Displays the version of ftdi-check");
}
