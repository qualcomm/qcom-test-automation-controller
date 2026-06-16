// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsole.h"

#include "PlatformID.h"

// QT
#include <QtGlobal>

void InitializeQCommonConsole()
{
	Q_INIT_RESOURCE(QCCResource);
	PlatformContainer::initialize();
}
