// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "StringUtilities.h"

// C++
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <functional>
#include <sstream>

std::string toLower(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});
	return result;
}

std::string toUpper(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
		return static_cast<char>(std::toupper(c));
	});
	return result;
}

std::string toCamelCase(const std::string& camelCaseMe, char splitChar)
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

std::string trim(const std::string& str, const std::string& whitespace)
{
	const size_t start = str.find_first_not_of(whitespace);
	if (start == std::string::npos)
		return "";
	const size_t end = str.find_last_not_of(whitespace);
	return str.substr(start, end - start + 1);
}

std::string trimLeft(const std::string& str, const std::string& whitespace)
{
	const size_t start = str.find_first_not_of(whitespace);
	if (start == std::string::npos)
		return "";
	return str.substr(start);
}

std::string trimRight(const std::string& str, const std::string& whitespace)
{
	const size_t end = str.find_last_not_of(whitespace);
	if (end == std::string::npos)
		return "";
	return str.substr(0, end + 1);
}

bool startsWith(const std::string& str, const std::string& prefix, bool caseSensitive)
{
	if (prefix.size() > str.size())
		return false;

	if (caseSensitive)
	{
		return str.compare(0, prefix.size(), prefix) == 0;
	}

	for (size_t i = 0; i < prefix.size(); ++i)
	{
		if (std::tolower(static_cast<unsigned char>(str[i])) !=
			std::tolower(static_cast<unsigned char>(prefix[i])))
		{
			return false;
		}
	}
	return true;
}

bool endsWith(const std::string& str, const std::string& suffix, bool caseSensitive)
{
	if (suffix.size() > str.size())
		return false;

	const size_t offset = str.size() - suffix.size();
	if (caseSensitive)
	{
		return str.compare(offset, suffix.size(), suffix) == 0;
	}

	for (size_t i = 0; i < suffix.size(); ++i)
	{
		if (std::tolower(static_cast<unsigned char>(str[offset + i])) !=
			std::tolower(static_cast<unsigned char>(suffix[i])))
		{
			return false;
		}
	}
	return true;
}

bool contains(const std::string& str, const std::string& substr, bool caseSensitive)
{
	if (substr.empty())
		return true;

	if (caseSensitive)
	{
		return str.find(substr) != std::string::npos;
	}

	auto it = std::search(
		str.begin(), str.end(),
		substr.begin(), substr.end(),
		[](unsigned char ch1, unsigned char ch2) {
			return std::tolower(ch1) == std::tolower(ch2);
		}
	);
	return it != str.end();
}

bool equalsIgnoreCase(const std::string& a, const std::string& b)
{
	if (a.size() != b.size())
		return false;

	for (size_t i = 0; i < a.size(); ++i)
	{
		if (std::tolower(static_cast<unsigned char>(a[i])) !=
			std::tolower(static_cast<unsigned char>(b[i])))
		{
			return false;
		}
	}
	return true;
}

bool isAlphaNumeric(const std::string& testMe)
{
	if (testMe.empty())
		return false;

	for (const auto& aChar : testMe)
	{
		if (std::isalnum(static_cast<unsigned char>(aChar)) == 0)
		{
			return false;
		}
	}
	return true;
}

std::vector<std::string> split(const std::string& str, char delimiter, bool skipEmpty)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(str);
	while (std::getline(tokenStream, token, delimiter))
	{
		if (!skipEmpty || !token.empty())
		{
			tokens.push_back(token);
		}
	}
	return tokens;
}

std::vector<std::string> split(const std::string& str, const std::string& delimiter, bool skipEmpty)
{
	std::vector<std::string> tokens;
	if (delimiter.empty())
	{
		tokens.push_back(str);
		return tokens;
	}

	size_t start = 0;
	size_t end = str.find(delimiter);
	while (end != std::string::npos)
	{
		std::string token = str.substr(start, end - start);
		if (!skipEmpty || !token.empty())
		{
			tokens.push_back(token);
		}
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}
	std::string lastToken = str.substr(start);
	if (!skipEmpty || !lastToken.empty())
	{
		tokens.push_back(lastToken);
	}
	return tokens;
}

std::vector<std::string> splitWords(const std::string& str)
{
	std::vector<std::string> words;
	std::istringstream iss(str);
	std::string word;
	while (iss >> word)
	{
		words.push_back(word);
	}
	return words;
}

std::string join(const std::vector<std::string>& elements, const std::string& delimiter)
{
	if (elements.empty())
		return "";

	std::string result = elements[0];
	for (size_t i = 1; i < elements.size(); ++i)
	{
		result += delimiter;
		result += elements[i];
	}
	return result;
}

std::string replace(const std::string& str, const std::string& from, const std::string& to)
{
	std::string result = str;
	replaceAll(result, from, to);
	return result;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
	if (from.empty())
		return;

	size_t startPos = 0;
	while ((startPos = str.find(from, startPos)) != std::string::npos)
	{
		str.replace(startPos, from.length(), to);
		startPos += to.length();
	}
}

std::string toHexByte(uint8_t value, bool upperCase)
{
	static const char hexUpper[] = "0123456789ABCDEF";
	static const char hexLower[] = "0123456789abcdef";
	const char* hex = upperCase ? hexUpper : hexLower;

	std::string result(2, '0');
	result[0] = hex[(value >> 4) & 0x0F];
	result[1] = hex[value & 0x0F];
	return result;
}

std::string toHexString(const void* data, size_t length, bool upperCase)
{
	if (!data || length == 0)
		return "";

	const uint8_t* bytePtr = static_cast<const uint8_t*>(data);
	std::string result;
	result.reserve(length * 2);
	for (size_t i = 0; i < length; ++i)
	{
		result += toHexByte(bytePtr[i], upperCase);
	}
	return result;
}

std::string fromHexString(const std::string& hexString)
{
	std::string result;
	for (size_t index = 0; index + 1 < hexString.size(); index += 2)
	{
		std::string byteString = hexString.substr(index, 2);
		result.push_back(static_cast<char>(std::strtoul(byteString.c_str(), nullptr, 16)));
	}
	return result;
}

std::string formatBytes(uint64_t byteCount, int precision)
{
	const uint64_t kKiloByte = 1024ULL;
	const uint64_t kMegaByte = kKiloByte * 1024ULL;
	const uint64_t kGigaByte = kMegaByte * 1024ULL;
	const uint64_t kTeraByte = kGigaByte * 1024ULL;

	int unitIndex = 0;
	double divisor = 0.0;
	static const char* const units[] = { "B", "KB", "MB", "GB", "TB" };

	if (byteCount < kKiloByte)
	{
		unitIndex = 0;
		divisor = 0.0;
	}
	else if (byteCount < kMegaByte)
	{
		unitIndex = 1;
		divisor = static_cast<double>(kKiloByte);
	}
	else if (byteCount < kGigaByte)
	{
		unitIndex = 2;
		divisor = static_cast<double>(kMegaByte);
	}
	else if (byteCount < kTeraByte)
	{
		unitIndex = 3;
		divisor = static_cast<double>(kGigaByte);
	}
	else
	{
		unitIndex = 4;
		divisor = static_cast<double>(kTeraByte);
	}

	char buffer[64];
	double displayValue = (divisor == 0.0) ? static_cast<double>(byteCount) : (static_cast<double>(byteCount) / divisor);
	if (precision < 0)
	{
		std::snprintf(buffer, sizeof(buffer), "%.0f %s", displayValue, units[unitIndex]);
	}
	else
	{
		std::snprintf(buffer, sizeof(buffer), "%.*f %s", precision, displayValue, units[unitIndex]);
	}
	return std::string(buffer);
}

HashType strHash(const std::string& hashMe)
{
	return ::arrayHash(hashMe);
}

HashType arrayHash(const std::string& hashMe)
{
	uint64_t result{0};

	const uint64_t p = 257;
	const uint64_t m = 1000000009ULL;
	uint64_t p_pow = 1;
	for (const auto c : hashMe)
	{
		result = (result + (static_cast<uint8_t>(c) - 'a' + 1) * p_pow) % m;
		p_pow = (p_pow * p) % m;
	}

	return result;
}

std::string fromBool(bool value)
{
	return value ? "true" : "false";
}

std::string fromPoint(const std::pair<int, int>& value)
{
	return std::to_string(value.first) + "," + std::to_string(value.second);
}

std::pair<int, int> toPoint(const std::string& value)
{
	std::pair<int, int> result{0, 0};

	size_t commaPos = value.find(',');

	if (commaPos != std::string::npos)
	{
		try
		{
			result.first = std::stoi(value.substr(0, commaPos));
			result.second = std::stoi(value.substr(commaPos + 1));
		}
		catch (...)
		{
			result = {0, 0};
		}
	}

	return result;
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

std::string createFilenameTimeStamp()
{
	return formatDateTime("_%Y_%d_%m_%H_%M_%S");
}

bool toUInt16(const std::string& value, uint16_t& result)
{
	if (value.empty())
		return false;

	if (value.find('-') != std::string::npos)
		return false;

	try
	{
		size_t pos = 0;
		unsigned long parsed = std::stoul(value, &pos, 0);
		if (pos != value.size() || parsed > 0xFFFF)
			return false;
		result = static_cast<uint16_t>(parsed);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool toUInt32(const std::string& value, uint32_t& result)
{
	if (value.empty())
		return false;

	if (value.find('-') != std::string::npos)
		return false;

	try
	{
		size_t pos = 0;
		unsigned long long parsed = std::stoull(value, &pos, 0);
		if (pos != value.size() || parsed > 0xFFFFFFFFULL)
			return false;
		result = static_cast<uint32_t>(parsed);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool toInt32(const std::string& value, int32_t& result)
{
	if (value.empty())
		return false;

	try
	{
		size_t pos = 0;
		long parsed = std::stol(value, &pos, 0);
		if (pos != value.size())
			return false;
		result = static_cast<int32_t>(parsed);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool isIntegerString(const std::string& s)
{
	if (s.empty())
		return false;
	size_t start = 0;
	if (s[0] == '-' || s[0] == '+')
	{
		if (s.length() == 1)
			return false;
		start = 1;
	}
	for (size_t i = start; i < s.length(); ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(s[i])))
			return false;
	}
	return true;
}

bool toBool(const std::string& str, bool defaultValue)
{
	if (str.empty())
		return defaultValue;

	std::string s = toLower(trim(str));
	if (s == "true" || s == "1" || s == "yes" || s == "on")
		return true;
	if (s == "false" || s == "0" || s == "no" || s == "off")
		return false;

	return defaultValue;
}

std::string compressString(const std::string& str)
{
	std::string result;
	result.reserve(str.size());
	for (char ch : str)
	{
		if (!std::isspace(static_cast<unsigned char>(ch)))
		{
			result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
		}
	}
	return result;
}

int indexOfCaseInsensitive(const std::vector<std::string>& list, const std::string& item)
{
	for (size_t index = 0; index < list.size(); ++index)
	{
		if (equalsIgnoreCase(list[index], item))
			return static_cast<int>(index);
	}
	return -1;
}

bool copyStringToBuffer(const std::string& str, char* buffer, int bufferSize)
{
	if (!buffer || bufferSize <= 0)
		return false;

	if (static_cast<int>(str.size()) >= bufferSize)
	{
		buffer[0] = '\0';
		return false;
	}

	std::memcpy(buffer, str.c_str(), str.size() + 1);
	return true;
}
