#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstdint>
#include <string>
#include <vector>
#include <utility>

typedef uint64_t HashType;

// --- Case conversions ---
std::string QCOMMONCONSOLE_EXPORT toLower(const std::string& str);
inline std::string toLowerCase(const std::string& str) { return toLower(str); }

std::string QCOMMONCONSOLE_EXPORT toUpper(const std::string& str);
inline std::string toUpperCase(const std::string& str) { return toUpper(str); }

std::string QCOMMONCONSOLE_EXPORT toCamelCase(const std::string& camelCaseMe, char splitChar = '_');

// --- Whitespace trimming ---
std::string QCOMMONCONSOLE_EXPORT trim(const std::string& str, const std::string& whitespace = " \t\r\n");
std::string QCOMMONCONSOLE_EXPORT trimLeft(const std::string& str, const std::string& whitespace = " \t\r\n");
std::string QCOMMONCONSOLE_EXPORT trimRight(const std::string& str, const std::string& whitespace = " \t\r\n");

// --- Substring, prefix, suffix & predicate checks ---
bool QCOMMONCONSOLE_EXPORT startsWith(const std::string& str, const std::string& prefix, bool caseSensitive = true);
bool QCOMMONCONSOLE_EXPORT endsWith(const std::string& str, const std::string& suffix, bool caseSensitive = true);
bool QCOMMONCONSOLE_EXPORT contains(const std::string& str, const std::string& substr, bool caseSensitive = true);
bool QCOMMONCONSOLE_EXPORT equalsIgnoreCase(const std::string& a, const std::string& b);
inline bool equalsCaseInsensitive(const std::string& a, const std::string& b) { return equalsIgnoreCase(a, b); }
bool QCOMMONCONSOLE_EXPORT isAlphaNumeric(const std::string& testMe);

// --- Splitting & Joining ---
std::vector<std::string> QCOMMONCONSOLE_EXPORT split(const std::string& str, char delimiter, bool skipEmpty = false);
std::vector<std::string> QCOMMONCONSOLE_EXPORT split(const std::string& str, const std::string& delimiter, bool skipEmpty = false);
std::vector<std::string> QCOMMONCONSOLE_EXPORT splitWords(const std::string& str);
std::string QCOMMONCONSOLE_EXPORT join(const std::vector<std::string>& elements, const std::string& delimiter);

// --- Numeric Parsing ---
bool QCOMMONCONSOLE_EXPORT toUInt16(const std::string& value, uint16_t& result);
bool QCOMMONCONSOLE_EXPORT toUInt32(const std::string& value, uint32_t& result);
bool QCOMMONCONSOLE_EXPORT toInt32(const std::string& value, int32_t& result);
bool QCOMMONCONSOLE_EXPORT isIntegerString(const std::string& s);
bool QCOMMONCONSOLE_EXPORT toBool(const std::string& str, bool defaultValue = false);

// --- Compression & Case-Insensitive Search ---
std::string QCOMMONCONSOLE_EXPORT compressString(const std::string& str);
int QCOMMONCONSOLE_EXPORT indexOfCaseInsensitive(const std::vector<std::string>& list, const std::string& item);

// --- Buffer Operations ---
bool QCOMMONCONSOLE_EXPORT copyStringToBuffer(const std::string& str, char* buffer, int bufferSize);

// --- Replacement ---
std::string QCOMMONCONSOLE_EXPORT replace(const std::string& str, const std::string& from, const std::string& to);
void QCOMMONCONSOLE_EXPORT replaceAll(std::string& str, const std::string& from, const std::string& to);

// --- Hex and Byte Formatting ---
std::string QCOMMONCONSOLE_EXPORT toHexByte(uint8_t value, bool upperCase = true);
std::string QCOMMONCONSOLE_EXPORT toHexString(const void* data, size_t length, bool upperCase = true);
inline std::string toHexString(const std::string& str, bool upperCase = true) { return toHexString(str.data(), str.size(), upperCase); }
std::string QCOMMONCONSOLE_EXPORT fromHexString(const std::string& hexString);
std::string QCOMMONCONSOLE_EXPORT formatBytes(uint64_t byteCount, int precision = -1);
inline std::string FormatBytes(uint64_t byteCount, int precision = -1) { return formatBytes(byteCount, precision); }

// --- Point and Boolean Conversions ---
std::string QCOMMONCONSOLE_EXPORT fromBool(bool value);
std::string QCOMMONCONSOLE_EXPORT fromPoint(const std::pair<int, int>& value);
std::pair<int, int> QCOMMONCONSOLE_EXPORT toPoint(const std::string& value);

// Backward compatibility inline forwarding
inline std::string fromQPoint(const std::pair<int, int>& value) { return fromPoint(value); }
inline std::pair<int, int> toQPoint(const std::string& value) { return toPoint(value); }

// --- Hashing ---
HashType QCOMMONCONSOLE_EXPORT strHash(const std::string& hashMe);
HashType QCOMMONCONSOLE_EXPORT arrayHash(const std::string& hashMe);

// --- Date / Time Formatting ---
std::string QCOMMONCONSOLE_EXPORT currentDateTimeString();
std::string QCOMMONCONSOLE_EXPORT formatDateTime(const std::string& format);
std::string QCOMMONCONSOLE_EXPORT createFilenameTimeStamp();

#endif // STRINGUTILITIES_H
