#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <cstdint>
#include <string>
#include <utility>

#include <QString>

typedef uint64_t HashType;

QString QCOMMONCONSOLE_EXPORT toCamelCase(const QString&  camelCaseMe, const QChar splitChar = '_', Qt::SplitBehavior splitBehavior = Qt::SkipEmptyParts);
bool QCOMMONCONSOLE_EXPORT isAlphaNumeric(const std::string& testMe);
std::string QCOMMONCONSOLE_EXPORT fromBool(bool value);

std::string QCOMMONCONSOLE_EXPORT fromQPoint(const std::pair<int, int>& value);
std::pair<int, int> QCOMMONCONSOLE_EXPORT toQPoint(const std::string& value);

HashType QCOMMONCONSOLE_EXPORT strHash(const std::string& hashMe);
HashType QCOMMONCONSOLE_EXPORT arrayHash(const std::string& hashMe);

#endif // STRINGUTILITIES_H
