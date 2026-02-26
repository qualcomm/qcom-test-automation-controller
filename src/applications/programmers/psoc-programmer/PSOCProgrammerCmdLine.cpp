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
