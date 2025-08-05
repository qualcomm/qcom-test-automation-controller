// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "ProgrammerCommandLine.h"

// QCommon
#include "version.h"

// Qt
#include <QCoreApplication>

ProgrammerCommandLine::ProgrammerCommandLine
(
	const QStringList& args
) :
	CommandLineParser(args)
{
	QCoreApplication* instance = QCoreApplication::instance();

	addHelpLine(instance->applicationName());
	addHelpLine("FTDI Platform Programmer");
	addHelpLine(QString("Version: %1").arg(LITE_PGR_COMMAND));

	setVersion(LITE_PGR_COMMAND);

	addHelpLine("");
	addHelpLine();
	addHelpLine("");
	addHelpLine("Options");
	addHelpLine("   -l, list                         -- List Lite capable devices");
	addHelpLine("   -lp, listplatforms               -- List the known lite platforms");
	addHelpLine("   -gp, getprogram                  -- Get the programming for the connected device");
	addHelpLine("   -p, program                      -- Program the connected device");

	addHelpLine("");
	addHelpLine("Parameters (optional), the parameters are required for the -p option");
	addHelpLine("   platformID=<numeric>             -- Set the platform id. (i.e.) platformID=8");
	addHelpLine("   serial=<text>                    -- The device with this serial number will be programmed. (i.e.) serial=foo");

	_listSet = isSet(QStringList{} << "-l" << "list");
	_listPlatformSet = isSet(QStringList{} << "-lp" << "listplatforms");
	_getProgramming = isSet(QStringList{} << "-gp" << "getprogram");
	_program = isSet(QStringList{} << "-p" << "program");

	_platformID = value("platformID");
	_serialNumber = value("serial");
}

bool ProgrammerCommandLine::listSet()
{
	return _listSet;
}

bool ProgrammerCommandLine::listPlatformSet()
{
	return _listPlatformSet;
}

bool ProgrammerCommandLine::getProgramming()
{
	return _getProgramming;
}

bool ProgrammerCommandLine::programSet()
{
	return _program;
}

QByteArray ProgrammerCommandLine::serialNumber()
{
	return _serialNumber.toLatin1();
}

QByteArray ProgrammerCommandLine::modelName()
{
	return _modelName.toLatin1();
}

quint32 ProgrammerCommandLine::platformID()
{
	quint32 result{0};
	bool okay;
	quint32 temp;

	if (_platformID.isEmpty() == false)
	{
		temp = _platformID.toUInt(&okay);
		if (okay)
			result = temp;
	}

	return result;
}
