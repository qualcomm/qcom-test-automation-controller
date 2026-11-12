// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "StringUtilities.h"

// C++
#include <cctype>
#include <chrono>
#include <ctime>
#include <functional>
#include <sstream>

using namespace std;

std::string toCamelCase
(
	const std::string& camelCaseMe,
	char splitChar
)
{
	std::string result;
	bool capitalizeNext = true;
	for (char c : camelCaseMe)
	{
		if (c == splitChar)
		{
			capitalizeNext = true;
		}
		else
		{
			if (capitalizeNext)
			{
				result.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
				capitalizeNext = false;
			}
			else
			{
				result.push_back(c);
			}
		}
	}

	return result;
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

std::string fromPoint(const std::pair<int, int>& value)
{
	return fromQPoint(value);
}

std::pair<int, int> toPoint(const std::string& value)
{
	return toQPoint(value);
}

std::string currentDateTimeString()
{
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
#if defined(_WIN32)
	localtime_s(&tm, &t);
#else
	localtime_r(&t, &tm);
#endif
	char buf[64];
	std::strftime(buf, sizeof(buf), "%c", &tm);
	return std::string(buf);
}

std::string formatDateTime(const std::string& format)
{
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
#if defined(_WIN32)
	localtime_s(&tm, &t);
#else
	localtime_r(&t, &tm);
#endif
	char buf[128];
	std::strftime(buf, sizeof(buf), format.c_str(), &tm);
	return std::string(buf);
}
