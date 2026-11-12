#ifndef BYTECOUNTFORMATTER_H
#define BYTECOUNTFORMATTER_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <cstdint>
#include <cstdio>
#include <string>

inline std::string FormatBytes
(
	uint64_t byteCount,
	int precision = -1
)
{
	double	divisor;

	const uint64_t kKiloByte(1024);
	const uint64_t kMegaByte(kKiloByte * 1024);
	const uint64_t kGigaByte(kMegaByte * 1024);
	const uint64_t kTeraByte(kGigaByte * 1024);

	int unitArrayIndex;
	const char* unitArray[] =
	{
		"B",
		"KB",
		"MB",
		"GB",
		"TB"
	};

	if (byteCount < kKiloByte)
	{
		unitArrayIndex = 0;
		divisor	=	0;
	}
	else if (byteCount < kMegaByte)
	{
		unitArrayIndex = 1;
		divisor	= kKiloByte;
	}
	else if (byteCount < kGigaByte)
	{
		unitArrayIndex = 2;
		divisor = kMegaByte;
	}
	else if (byteCount < kTeraByte)
	{
		unitArrayIndex = 3;
		divisor	= kGigaByte;
	}
	else
	{
		unitArrayIndex = 4;
		divisor = kTeraByte;
	}

	char buffer[64];
	double displayValue = (divisor == 0.0) ? static_cast<double>(byteCount) : (static_cast<double>(byteCount) / divisor);
	if (precision < 0)
	{
		std::snprintf(buffer, sizeof(buffer), "%.0f %s", displayValue, unitArray[unitArrayIndex]);
	}
	else
	{
		std::snprintf(buffer, sizeof(buffer), "%.*f %s", precision, displayValue, unitArray[unitArrayIndex]);
	}

	return std::string(buffer);
}

#endif // BYTECOUNTFORMATTER_H
