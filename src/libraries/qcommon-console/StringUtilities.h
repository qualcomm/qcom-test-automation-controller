#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include <QPoint>
#include <QString>

typedef quint64 HashType;

QString QCOMMONCONSOLE_EXPORT toCamelCase(const QString&  camelCaseMe, const QChar splitChar = '_', Qt::SplitBehavior splitBehavior = Qt::SkipEmptyParts);
bool QCOMMONCONSOLE_EXPORT isAlphaNumeric(const QByteArray& testMe);
QString QCOMMONCONSOLE_EXPORT fromBool(bool value);

QString QCOMMONCONSOLE_EXPORT fromQPoint(const QPoint& value);
QPoint QCOMMONCONSOLE_EXPORT toQPoint(const QString& value);

HashType QCOMMONCONSOLE_EXPORT strHash(const QString& hashMe);
HashType QCOMMONCONSOLE_EXPORT arrayHash(const QByteArray& hashMe);

#endif // STRINGUTILITIES_H
