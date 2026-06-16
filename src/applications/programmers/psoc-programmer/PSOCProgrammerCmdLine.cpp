// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PSOCProgrammerCmdLine.h"

// QCommon
#include "version.h"

// Qt
#include <QCoreApplication>

PSOCCommandLine::PSOCCommandLine
(
	const QStringList& args
) :
	CommandLineParser(args)
{
	QCoreApplication* instance = QCoreApplication::instance();

	addHelpLine(instance->applicationName());
	addHelpLine("Eeprom value programmer");
	addHelpLine(QString("Version: %1").arg(EEPROM_COMMAND));

	setVersion(EEPROM_COMMAND);

	addHelpLine("");
	addHelpLine();
	addHelpLine("");
	addHelpLine("Options");
	addHelpLine("   -l, list                         -- List Eeprom capable devices");
	addHelpLine("   -lp, listplatforms               -- List the known platforms");
	addHelpLine("   -gp, getprogram                  -- Get the programming the connected device");
	addHelpLine("   -p, program                      -- Program the connected device");

	addHelpLine("");
	addHelpLine("Parameters (optional), the program functions will uses these parameters");
	addHelpLine("   platformID=<numeric>             -- Set the platform id. (i.e.) platformID=8");
	addHelpLine("   serial=<text>                    -- Set the serial number. (i.e.) serial=foo");
	addHelpLine("   model=<text>                     -- Set the model number. (i.e.) model=bar");
	addHelpLine("");
	addHelpLine("   Enclose the parameter in a string if you want spaces in the value");

	_listSet = isSet(QStringList{} << "-l" << "list");
	_listPlatformSet = isSet(QStringList{} << "-lp" << "listplatforms");
	_getProgramming = isSet(QStringList{} << "-gp" << "getprogram");
	_program = isSet(QStringList{} << "-p" << "program");

	_platformID = value("platformID");
	_serialNumber = value("serial");
	_modelName = value("model");
}

bool PSOCCommandLine::listSet()
{
	return _listSet;
}

bool PSOCCommandLine::listPlatformSet()
{
	return _listPlatformSet;
}

bool PSOCCommandLine::getProgramming()
{
	return _getProgramming;
}

bool PSOCCommandLine::programSet()
{
	return _program;
}

QByteArray PSOCCommandLine::serialNumber()
{
	return _serialNumber.toLatin1();
}

QByteArray PSOCCommandLine::modelName()
{
	return _modelName.toLatin1();
}

quint32 PSOCCommandLine::platformID()
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
