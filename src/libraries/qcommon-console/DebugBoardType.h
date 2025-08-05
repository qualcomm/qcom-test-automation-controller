#ifndef DEBUGBOARDTYPE_H
#define DEBUGBOARDTYPE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author:	Michael Simpson <msimpson@qti.qualcomm.com>
//			Biswajit Roy <biswroy@qti.qualcomm.com>

#include "QCommonConsoleGlobal.h"

#include <QString>

enum DebugBoardType
{
	eUnknownDebugBoard,
	eFTDI
};

QString QCOMMONCONSOLE_EXPORT debugBoardTypeToString(DebugBoardType);
DebugBoardType QCOMMONCONSOLE_EXPORT debugBoardTypeFromString(const QString& boardString);

#endif // DEBUGBOARDTYPE_H
