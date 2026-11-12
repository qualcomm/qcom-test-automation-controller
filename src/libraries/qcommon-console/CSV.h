#ifndef CSV_H
#define CSV_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <ostream>
#include <string>
#include <vector>

void QCOMMONCONSOLE_EXPORT writeCSVLine(std::ostream& stream, const std::vector<std::string>& values);

#endif // CSV_H
