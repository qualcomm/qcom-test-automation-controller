#ifndef CONSOLEINTERFACE_H
#define CONSOLEINTERFACE_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "FramePackage.h"
#include "QCommonConsoleGlobal.h"

class QCOMMONCONSOLE_EXPORT ConsoleInterface
{
public:
	virtual ~ConsoleInterface() {}

	virtual void addConsoleText(const QByteArray& consoleText) = 0;
	virtual void handleConsoleResponse(const FramePackage& framePackage) = 0;
};

#endif // CONSOLEINTERFACE_H
