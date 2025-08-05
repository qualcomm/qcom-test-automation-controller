#ifndef CSV_H
#define CSV_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// Qt
#include <QString>
#include <QTextStream>

void QCOMMONCONSOLE_EXPORT writeCSVLine(QTextStream& textStream, const QStringList& values);

#endif // CSV_H
