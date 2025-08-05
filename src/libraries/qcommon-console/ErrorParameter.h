#ifndef ERRORPARAMETERS_H
#define ERRORPARAMETERS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include <QPair>
#include <QString>
#include <QVariant>

typedef QPair<QString, QVariant> ErrorParameter;
typedef QList<ErrorParameter> ErrorParameters;

#endif // ERRORPARAMETERS_H
