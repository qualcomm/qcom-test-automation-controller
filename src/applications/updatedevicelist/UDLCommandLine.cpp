// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
