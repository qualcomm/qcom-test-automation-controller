#ifndef ERRORPARAMETERS_H
#define ERRORPARAMETERS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <QVariant>

#include <string>
#include <utility>
#include <vector>

typedef std::pair<std::string, QVariant> ErrorParameter;
typedef std::vector<ErrorParameter> ErrorParameters;

#endif // ERRORPARAMETERS_H
