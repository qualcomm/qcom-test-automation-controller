// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "StringUtilities.h"

// QT
#include <QStringList>

// C++
#include <cctype>
#include <functional>

using namespace std;

QString toCamelCase
(
	const QString& camelCaseMe,
	const QChar splitChar,
	Qt::SplitBehavior splitBehavior
)
{
	QStringList parts = camelCaseMe.split(splitChar, splitBehavior);
	for (auto& part: parts)
	{
		part.replace(0, 1, (part)[0].toUpper());
	}

	return parts.join("");
}

bool isAlphaNumeric
(
	const QByteArray &testMe
)
{
	bool result{true};

	for (const auto& aChar: testMe)
	{
		if (isalnum(aChar) == 0)
		{
			result = false;

			break;
		}
	}

	return result;
}

HashType strHash(const QString& hashMe)
{
	return ::arrayHash(hashMe.toLatin1());
}

HashType arrayHash
(
	const QByteArray& hashMe
)
{
	quint64 result{0};

	const quint64 p = 257;
	const quint64 m = 1e9 + 9;
	quint64 p_pow = 1;
	for (const auto c: hashMe)
	{
		result = (result + (c - 'a' + 1) * p_pow) % m;
		p_pow = (p_pow * p) % m;
	}

	return result;
}


QString fromBool(bool value)
{
	QString result;

	result = value ? QString("true") : QString("false");

	return result;
}

QString fromQPoint(const QPoint& value)
{
	QString result = QString("%1,%2").arg(value.x()).arg(value.y());

	return result;
}

QPoint toQPoint(const QString& value)
{
	QPoint result;

	QStringList temp = value.split(",");

	if (temp.size() >= 2)
	{
		int x = temp[0].toInt();
		int y = temp[1].toInt();

		result = QPoint(x,y);
	}

	return result;
}
