#ifndef ALPACADEFINES_H
#define ALPACADEFINES_H
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
// Copyright 2013-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"
#include "StringProof.h"
#include "version.h"

#include <QString>

const QString kProductName(QStringLiteral("QTAC"));
const char kProductID[] =	"1e48f695-c109-11ec-aebb-063166a9270b"; // QTAC External
const char kCoreFeature[] = "1e71efc7-c109-11ec-aebb-063166a9270b";

const QString kOrganizationName(QStringLiteral("Qualcomm, Inc."));
const QString kProductVersion(QStringLiteral(ALPACA_VERSION));
const QString kCompileDate(QStringLiteral(__DATE__));
const QString kCompileTime(QStringLiteral(__TIME__));
const QString kBuildTime(kCompileDate + QStringLiteral(" ") + kCompileTime);
const QString kVersionGUID(QStringLiteral("{6F01E0AB-1962-4054-C061-3CA7CA4397E0}")); // produced by GUID Maker

quint32 QCOMMONCONSOLE_EXPORT makeFirmwareVersion(quint32 hw, quint32 major, quint32 minor);

#endif // ALPACADEFINES_H
