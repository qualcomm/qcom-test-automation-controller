#ifndef FTDICHECKCOMMANDLINE_H
#define FTDICHECKCOMMANDLINE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "CommandLineParser.h"

class FTDICheckCommandLine : public CommandLineParser
{
public:
	FTDICheckCommandLine(const QStringList& args);
};

#endif // FTDICHECKCOMMANDLINE_H
