#ifndef PROGRAMMERCOMMANDLINE_H
#define PROGRAMMERCOMMANDLINE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Qt
#include <QString>
#include <QStringList>

// QCommon
#include "CommandLineParser.h"

const QString kAppName{"LITEProgrammer"};

class ProgrammerCommandLine :
	public CommandLineParser
{
public:
	ProgrammerCommandLine(const QStringList& args);

	bool listSet();
	bool listPlatformSet();
	bool getProgramming();
	bool programSet();

	std::string serialNumber();
	std::string modelName();
	uint32_t platformID();

private:
	bool						_listSet{false};
	bool						_listPlatformSet{false};
	bool						_getProgramming{false};
	bool						_program{false};

	std::string					_serialNumber;
	std::string					_modelName;
	std::string					_platformID;
};

#endif // PROGRAMMERCOMMANDLINE_H
