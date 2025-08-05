#ifndef TACDEFINES_H
#define TACDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// QCommon
#include "version.h"

// QT
#include <QString>

const QString kAppName(QStringLiteral("TAC"));
const QString kAppVersion(QStringLiteral(TAC_VERSION));

const QString kTerminalApp(QStringLiteral("TACTerminal"));
const QString kTerminalAppVersion(QStringLiteral(TAC_TERMINAL_VERSION));

const QString kTACConfigEditorApp(QStringLiteral("TAC Configuration Editor"));
const QString kTACConfigEditorVersion(QStringLiteral(TAC_CONFIG_VERSION));

const QString kFTDICheckApp(QStringLiteral("FTDI Check"));
const QString kFTDICheckVersion(QStringLiteral(FTDI_CHECK_VERSION));

const QString kInitialString(kAppName + ", version " + kAppVersion);

const QString kTACConfigurationGroup(kAppName);

#endif // TACDEFINES_H
