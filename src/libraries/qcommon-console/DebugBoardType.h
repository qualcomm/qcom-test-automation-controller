// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DEBUGBOARDTYPE_H
#define DEBUGBOARDTYPE_H

#include "QCommonConsoleGlobal.h"

#include <QString>

enum DebugBoardType
{
	eUnknownDebugBoard,
	eSpiderBoard,
	ePSOC,
	eFTDI,
	ePIC32CXAuto
};

QString QCOMMONCONSOLE_EXPORT debugBoardTypeToString(DebugBoardType);
DebugBoardType QCOMMONCONSOLE_EXPORT debugBoardTypeFromString(const QString& boardString);

#endif // DEBUGBOARDTYPE_H
