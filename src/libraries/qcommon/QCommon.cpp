// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommon.h"

#include "QCommonConsole.h"

// QCommon
#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"

// QT
#include <QDebug>
#include <QtGlobal>

void InitializeQCommon()
{
	Q_INIT_RESOURCE(QCResource);
	InitializeQCommonConsole();

	// Every GUI application calls this right after creating its application
	// object, so this is the one place that records where the install was
	// found. That makes a wrong path visible in the log of any install layout.
	logInstallLayout();
	qInfo().noquote() << "Install layout: documentation     =" << docsRoot();
}
