// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "QCommonConsole.h"

#include "PlatformID.h"

// QT
#include <QtGlobal>

void InitializeQCommonConsole()
{
	Q_INIT_RESOURCE(qcommon_console);

	PlatformContainer::initialize();
}
