#ifndef PSOCCOMMANDLINE_H
#define PSOCCOMMANDLINE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "CommandLineParser.h"

class PSOCCommandLine :
	public CommandLineParser
{
public:
	PSOCCommandLine(const QStringList& args);

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

#endif // PSOCCOMMANDLINE_H
