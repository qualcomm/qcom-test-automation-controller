// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "DebugBoardType.h"

#include <cctype>

static bool equalsCaseInsensitive(const std::string& left, const std::string& right)
{
	if (left.size() != right.size())
		return false;

	for (size_t index = 0; index < left.size(); index++)
	{
		if (std::tolower(static_cast<unsigned char>(left[index])) != std::tolower(static_cast<unsigned char>(right[index])))
			return false;
	}

	return true;
}

std::string debugBoardTypeToString(DebugBoardType debugBoardType)
{
	std::string result("Unknown");

	switch (debugBoardType)
	{
		case ePSOC: result = "PSOC"; break;
		case eSpiderBoard: result = "Spider Board"; break;
		case eFTDI: result = "FTDI"; break;
		case ePIC32CXAuto: result = "PIC32CXAuto"; break;
		case eFT232H: result = "FT232H"; break;
		case eSTM32: result = "STM32"; break;
		default:
			break;
	}

	return result;
}

DebugBoardType debugBoardTypeFromString(const std::string& boardString)
{
	DebugBoardType result{eUnknownDebugBoard};

	if (equalsCaseInsensitive(boardString, "PSOC"))
		result = ePSOC;
	else if (equalsCaseInsensitive(boardString, "FTDI"))
		result = eFTDI;
	else if (equalsCaseInsensitive(boardString, "PIC32CXAuto"))
		result = ePIC32CXAuto;
	else if (equalsCaseInsensitive(boardString, "Spider Board"))
		result = eSpiderBoard;
	else if (equalsCaseInsensitive(boardString, "FT232H"))
		result = eFT232H;
	else if (equalsCaseInsensitive(boardString, "STM32"))
		result = eSTM32;

	return result;
}

std::string psocVariantToString(PSOCVariant psocVariant)
{
	std::string result("Unknown");

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

PSOCVariant psocVariantFromString(const std::string &psocVariantString)
{
	PSOCVariant result{ePSOCUnknown};

	if (equalsCaseInsensitive(psocVariantString, "GPIO"))
		result = ePSOCGPIO;
	else if (equalsCaseInsensitive(psocVariantString, "GPIO with I2C"))
		result = ePSOCGPIOIIC;

	return result;
}

std::string psocSlaveToString(const PSOCIICVariant slave)
{
	std::string result("Unknown");

	switch (slave)
	{
	case ePSOCIICUnknown: break;
	case eKTS1622EUAATR: result = "KTS1622EUAATR"; break;
	case eTCA9534APWR: result = "TCA9534APWR"; break;
	default:
		break;
	}

	return result;
}
