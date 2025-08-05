#ifndef UDLCOMMANDLINE_H
#define UDLCOMMANDLINE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "CommandLineParser.h"

class UDLCommandLine :
	public CommandLineParser
{
public:
	UDLCommandLine(const QStringList& args);

	QString userDir()
	{
		return _dir;
	}

	bool verbose()
	{
		return _verbose;
	}

private:
	bool						_verbose{false};
	QString						_dir;
};

#endif // UDLCOMMANDLINE_H
