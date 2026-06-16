// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommon.h"

#include "QCommonConsole.h"

// QT
#include <QtGlobal>

void InitializeQCommon()
{
	Q_INIT_RESOURCE(QCResource);
	InitializeQCommonConsole();
}
