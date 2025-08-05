#ifndef PROGRAMMERCOMMANDLINE_H
#define PROGRAMMERCOMMANDLINE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "CommandLineParser.h"

const QString kAppName{"lite-programmer"};

class ProgrammerCommandLine :
	public CommandLineParser
{
public:
	ProgrammerCommandLine(const QStringList& args);

	bool listSet();
	bool listPlatformSet();
	bool getProgramming();
	bool programSet();

	QByteArray serialNumber();
	QByteArray modelName();
	quint32 platformID();

private:
	bool						_listSet{false};
	bool						_listPlatformSet{false};
	bool						_getProgramming{false};
	bool						_program{false};

	QString						_serialNumber;
	QString						_modelName;
	QString						_platformID;
};

#endif // PROGRAMMERCOMMANDLINE_H
