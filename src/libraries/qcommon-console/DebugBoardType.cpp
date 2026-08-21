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
		case eFT232H: result = "FT232H"; break;
		default:
			break;
	}

	return result;
}

DebugBoardType debugBoardTypeFromString(const QString& boardString)
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
	else if (boardString.compare("FT232H", Qt::CaseInsensitive) == 0)
		result = eFT232H;

	return result;
}

QString psocVariantToString(PSOCVariant psocVariant)
{
	QString result("Unknown");

	switch (psocVariant)
	{
		case ePSOCUnknown: break;
		case ePSOCGPIO: result = "GPIO"; break;
		case ePSOCGPIOIIC: result = "GPIO with I2C"; break;
		default:
			break;
	}

	return result;
}

PSOCVariant psocVariantFromString(const QString &psocVariantString)
{
	PSOCVariant result{ePSOCUnknown};

	if (psocVariantString.compare("GPIO", Qt::CaseInsensitive) == 0)
		result = ePSOCGPIO;
	else if (psocVariantString.compare("GPIO with I2C", Qt::CaseInsensitive) == 0)
		result = ePSOCGPIOIIC;

	return result;
}
