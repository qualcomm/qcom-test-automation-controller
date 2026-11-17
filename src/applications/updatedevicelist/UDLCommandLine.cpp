// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "UDLCommandLine.h"
#include "version.h"
#include "QCommonTypes.h"

// Qt
#include <QCoreApplication>

UDLCommandLine::UDLCommandLine
(
	const QStringList& args
) :
	CommandLineParser(toStdVector(args))
{
	QCoreApplication* instance = QCoreApplication::instance();

	if (instance)
		addHelpLine(instance->applicationName().toStdString());
	addHelpLine("Update TAC Device Configurations");
	addHelpLine(std::string("Version: ") + UPDATE_DEVICE_LIST);

	addHelpLine("");
	addHelpLine();
	addHelpLine("");
	addHelpLine("Options");
	addHelpLine("   -v, verbose                   -- Write verbose diagnostics when run");
	addHelpLine("");
	addHelpLine("   dir=<text>                    -- Sets a user directory to process. (i.e.) dir=C:/pathtofiles");

	_verbose = isSet({"-v", "verbose"});
	_dir = toQString(value("dir"));
}
