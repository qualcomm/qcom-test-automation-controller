// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ProgrammerCommandLine.h"

// QCommon
#include "AppCore.h"
#include "QCommonTypes.h"
#include "StringUtilities.h"
#include "version.h"

// Qt
#include <QCoreApplication>

ProgrammerCommandLine::ProgrammerCommandLine
(
	const QStringList& args
) :
	CommandLineParser(toStdVector(args))
{
	QCoreApplication* instance = QCoreApplication::instance();

	if (instance)
		addHelpLine(instance->applicationName().toStdString());
	addHelpLine("FTDI Platform Programmer");
	addHelpLine(std::string("Version: ") + LITE_PGR_COMMAND);

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

	_listSet = isSet({"-l", "list"});
	_listPlatformSet = isSet({"-lp", "listplatforms"});
	_getProgramming = isSet({"-gp", "getprogram"});
	_program = isSet({"-p", "program"});

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

std::string ProgrammerCommandLine::serialNumber()
{
	return _serialNumber;
}

std::string ProgrammerCommandLine::modelName()
{
	return _modelName;
}

uint32_t ProgrammerCommandLine::platformID()
{
	uint32_t result{0};
	if (!_platformID.empty())
	{
		try
		{
			result = static_cast<uint32_t>(std::stoul(_platformID));
		}
		catch (const std::exception& e)
		{
			AppCore::writeToApplicationLogLine("ProgrammerCommandLine::platformID: failed parsing '" + _platformID + "': " + e.what());
			result = 0;
		}
	}
	return result;
}
