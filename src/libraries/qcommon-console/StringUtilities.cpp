/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
