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
// Copyright 2013-2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
