// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
