#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstdint>
#include <string>
#include <utility>

typedef uint64_t HashType;

std::string QCOMMONCONSOLE_EXPORT toCamelCase(const std::string& camelCaseMe, char splitChar = '_');
bool QCOMMONCONSOLE_EXPORT isAlphaNumeric(const std::string& testMe);
std::string QCOMMONCONSOLE_EXPORT fromBool(bool value);

std::string QCOMMONCONSOLE_EXPORT fromQPoint(const std::pair<int, int>& value);
std::pair<int, int> QCOMMONCONSOLE_EXPORT toQPoint(const std::string& value);
std::string QCOMMONCONSOLE_EXPORT fromPoint(const std::pair<int, int>& value);
std::pair<int, int> QCOMMONCONSOLE_EXPORT toPoint(const std::string& value);

HashType QCOMMONCONSOLE_EXPORT strHash(const std::string& hashMe);
HashType QCOMMONCONSOLE_EXPORT arrayHash(const std::string& hashMe);

std::string QCOMMONCONSOLE_EXPORT currentDateTimeString();
std::string QCOMMONCONSOLE_EXPORT formatDateTime(const std::string& format);

#endif // STRINGUTILITIES_H
