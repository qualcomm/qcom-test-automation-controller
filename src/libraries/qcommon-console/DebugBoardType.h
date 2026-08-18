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
	ePIC32CXAuto,
	eFT232H
};

enum PSOCVariant
{
	ePSOCUnknown,
	ePSOCGPIO,
	ePSOCGPIOIIC,
};

QString QCOMMONCONSOLE_EXPORT debugBoardTypeToString(DebugBoardType);
DebugBoardType QCOMMONCONSOLE_EXPORT debugBoardTypeFromString(const QString& boardString);

QString QCOMMONCONSOLE_EXPORT psocVariantToString(PSOCVariant psocVariant);
PSOCVariant QCOMMONCONSOLE_EXPORT psocVariantFromString(const QString& psocVariantString);

#endif // DEBUGBOARDTYPE_H
