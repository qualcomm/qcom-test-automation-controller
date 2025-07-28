#ifndef BYTECOUNTFORMATTER_H
#define BYTECOUNTFORMATTER_H

// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include <QtGlobal>
#include <QString>

inline QString FormatBytes
(
	quint64 byteCount,
	int precision = -1
)
{
	double	divisor;

	const quint64 kKiloByte(1024);
	const quint64 kMegaByte(kKiloByte * 1024);
	const quint64 kGigaByte(kMegaByte * 1024);
	const quint64 kTeraByte(kGigaByte * 1024);

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
