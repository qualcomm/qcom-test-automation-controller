// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef DEBUGBOARDTYPE_H
#define DEBUGBOARDTYPE_H

#include "QCommonConsoleGlobal.h"

#include <string>

enum DebugBoardType
{
	eUnknownDebugBoard,
	eSpiderBoard,
	ePSOC,
	eFTDI,
	ePIC32CXAuto,
	eFT232H,
	eSTM32,
};

enum PSOCVariant
{
	ePSOCUnknown,
	ePSOCGPIO,
	ePSOCGPIOIIC,
};

enum PSOCIICVariant
{
	ePSOCIICUnknown,
	eKTS1622EUAATR,
	eTCA9534APWR,
};

std::string QCOMMONCONSOLE_EXPORT debugBoardTypeToString(DebugBoardType);
DebugBoardType QCOMMONCONSOLE_EXPORT debugBoardTypeFromString(const std::string& boardString);

std::string QCOMMONCONSOLE_EXPORT psocVariantToString(PSOCVariant psocVariant);
PSOCVariant QCOMMONCONSOLE_EXPORT psocVariantFromString(const std::string& psocVariantString);

std::string QCOMMONCONSOLE_EXPORT psocSlaveToString(const PSOCIICVariant slave);

#endif // DEBUGBOARDTYPE_H
