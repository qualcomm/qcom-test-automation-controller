#ifndef STRINGUTILITIES_H
#define STRINGUTILITIES_H
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
// Copyright 2013-2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
