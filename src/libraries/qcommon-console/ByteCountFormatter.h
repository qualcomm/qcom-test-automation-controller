#ifndef BYTECOUNTFORMATTER_H
#define BYTECOUNTFORMATTER_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <QString>

#include <cstdint>

inline QString FormatBytes
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
	QString unitArray[] =
	{
		QString("B"),
		QString("KB"),
		QString("MB"),
		QString("GB"),
		QString("TB")
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

	if	(divisor ==	0.0)
		return QString("%1 %2").arg((double) byteCount,	0, 'f', precision).arg(unitArray[unitArrayIndex]);

	return QString("%1 %2").arg((double) byteCount / divisor, 0, 'f', precision).arg(unitArray[unitArrayIndex]);
}

#endif // BYTECOUNTFORMATTER_H
