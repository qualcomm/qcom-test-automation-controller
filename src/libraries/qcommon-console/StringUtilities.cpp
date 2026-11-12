// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
	const std::string &testMe
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

HashType strHash(const std::string& hashMe)
{
	return ::arrayHash(hashMe);
}

HashType arrayHash
(
	const std::string& hashMe
)
{
	uint64_t result{0};

	const uint64_t p = 257;
	const uint64_t m = 1e9 + 9;
	uint64_t p_pow = 1;
	for (const auto c: hashMe)
	{
		result = (result + (c - 'a' + 1) * p_pow) % m;
		p_pow = (p_pow * p) % m;
	}

	return result;
}

std::string fromBool(bool value)
{
	return value ? "true" : "false";
}

std::string fromQPoint(const std::pair<int, int>& value)
{
	return std::to_string(value.first) + "," + std::to_string(value.second);
}

std::pair<int, int> toQPoint(const std::string& value)
{
	std::pair<int, int> result{0, 0};

	size_t commaPos = value.find(',');

	if (commaPos != std::string::npos)
	{
		result.first = std::stoi(value.substr(0, commaPos));
		result.second = std::stoi(value.substr(commaPos + 1));
	}

	return result;
}
