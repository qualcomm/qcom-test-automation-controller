#ifndef HEXIFY_H
#define HEXIFY_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
