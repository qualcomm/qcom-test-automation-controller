// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "QCommon.h"

#include "QCommonConsole.h"

// QT
#include <QtGlobal>

void InitializeQCommon()
{
	Q_INIT_RESOURCE(QCommon);

	InitializeQCommonConsole();
}
