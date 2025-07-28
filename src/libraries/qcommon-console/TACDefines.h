#ifndef TACDEFINES_H
#define TACDEFINES_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2018-2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
