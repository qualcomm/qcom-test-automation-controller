#ifndef HEXIFY_H
#define HEXIFY_H

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

#include <QByteArray>
#include <QString>

inline QByteArray Hexify
(
	const QByteArray& hexMe
)
{
	QByteArray result;
	QByteArray temp(hexMe.toHex());
	bool even(false);

	QByteArray::iterator ch = temp.begin();
	while (ch != temp.end())
	{
		result += *ch;

		if (even)
			result += " ";

		even = !even;

		ch++;
	}

	return result.trimmed();
}

template<class T> QByteArray DecAndHexPrint(T value, int width)
{
	return QString("%1 (0x%2)").arg(value, 10, 10, QChar(' ')).arg(value, width, 16, QChar('0')).toLatin1();
}

template<class T> QString DecAndHexPrintString(T value, int width)
{
	return QString("%1 (0x%2)").arg(value, 10, 10, QChar(' ')).arg(value, width, 16, QChar('0'));
}
#endif // HEXIFY_H
