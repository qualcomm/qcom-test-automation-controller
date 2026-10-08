// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Authors: Michael Simpson, Biswajit Roy

#ifndef QTAC_DEBUGBOARDTYPE_H
#define QTAC_DEBUGBOARDTYPE_H

#include <qtac/String.h>

enum DebugBoardType
{
	eUnknownDebugBoard,
	eSpiderBoard,
	ePSOC,
	eFTDI,
	ePIC32CXAuto,
	eFT232H,
	eSTM32
};

inline qtac::String debugBoardTypeToString(DebugBoardType type)
{
	switch (type)
	{
	case eSpiderBoard:  return "SpiderBoard";
	case ePSOC:         return "PSOC";
	case eFTDI:         return "FTDI";
	case ePIC32CXAuto:  return "PIC32CXAuto";
	case eFT232H:       return "FT232H";
	case eSTM32:        return "STM32";
	default:            return "Unknown";
	}
}

inline DebugBoardType debugBoardTypeFromString(const qtac::String& boardString)
{
	qtac::String lower = boardString.toLower();
	if (lower == "spiderboard") return eSpiderBoard;
	if (lower == "psoc")        return ePSOC;
	if (lower == "ftdi")        return eFTDI;
	if (lower == "pic32cxauto") return ePIC32CXAuto;
	if (lower == "ft232h")      return eFT232H;
	if (lower == "stm32")       return eSTM32;
	return eUnknownDebugBoard;
}

// ---------------------------------------------------------------------------
// PSoC variant enums (mirrors upstream qcommon-console DebugBoardType.h)
// ---------------------------------------------------------------------------

enum PSOCVariant
{
	ePSOCUnknown,
	ePSOCGPIO,
	ePSOCGPIOIIC
};

enum PSOCIICVariant
{
	ePSOCIICUnknown,
	eKTS1622EUAATR,
	eTCA9534APWR
};

inline qtac::String psocVariantToString(PSOCVariant v)
{
	switch (v)
	{
	case ePSOCGPIO:    return "GPIO";
	case ePSOCGPIOIIC: return "GPIOIIC";
	default:           return "Unknown";
	}
}

inline PSOCVariant psocVariantFromString(const qtac::String& s)
{
	if (s == "GPIO")    return ePSOCGPIO;
	if (s == "GPIOIIC") return ePSOCGPIOIIC;
	return ePSOCUnknown;
}

inline qtac::String psocSlaveToString(PSOCIICVariant v)
{
	switch (v)
	{
	case eKTS1622EUAATR: return "KTS1622EUAATR";
	case eTCA9534APWR:   return "TCA9534APWR";
	default:             return "Unknown";
	}
}

inline PSOCIICVariant psocSlaveFromString(const qtac::String& s)
{
	if (s == "KTS1622EUAATR") return eKTS1622EUAATR;
	if (s == "TCA9534APWR")   return eTCA9534APWR;
	return ePSOCIICUnknown;
}

#endif // QTAC_DEBUGBOARDTYPE_H
