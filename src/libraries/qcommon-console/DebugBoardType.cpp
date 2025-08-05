// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author:	Michael Simpson <msimpson@qti.qualcomm.com>
//			Biswajit Roy <biswroy@qti.qualcomm.com>

#include "DebugBoardType.h"

QString debugBoardTypeToString(DebugBoardType debugBoardType)
{
	QString result("Unknown");

	switch (debugBoardType)
	{
		case eFTDI: result = "FTDI"; break;
		default:
			break;
	}

	return result;
}

DebugBoardType debugBoardTypeFromString
(
	const QString& boardString
)
{
	DebugBoardType result{eUnknownDebugBoard};

	if (boardString.compare("FTDI", Qt::CaseInsensitive) == 0)
		result = eFTDI;

	return result;
}
