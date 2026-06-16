// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "DebugBoardType.h"

QString debugBoardTypeToString(DebugBoardType debugBoardType)
{
	QString result("Unknown");

	switch (debugBoardType)
	{
		case ePSOC: result = "PSOC"; break;
		case eSpiderBoard: result = "Spider Board"; break;
		case eFTDI: result = "FTDI"; break;
		case ePIC32CXAuto: result = "PIC32CXAuto"; break;
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

	if (boardString.compare("PSOC", Qt::CaseInsensitive) == 0)
		result = ePSOC;
	else if (boardString.compare("FTDI", Qt::CaseInsensitive) == 0)
		result = eFTDI;
	else if (boardString.compare("PIC32CXAuto", Qt::CaseInsensitive) == 0)
		result = ePIC32CXAuto;
	else if (boardString.compare("Spider Board", Qt::CaseInsensitive) == 0)
		result = eSpiderBoard;

	return result;
}
