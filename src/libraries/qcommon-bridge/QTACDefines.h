#ifndef QTACDEFINES_H
#define QTACDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonBridgeGlobal.h"
#include "version.h"

// Qt
#include <QString>

const QString kAppName{QStringLiteral("TAC")};
const QString kAppVersion{QStringLiteral(QTAC_VERSION)};

const QString kProductName{QStringLiteral("QTAC")};
const QString kProductVersion{QStringLiteral(QTAC_VERSION)};
const QString kOrganizationName{QStringLiteral("Qualcomm, Inc.")};
const QString kCompileDate{QStringLiteral(__DATE__)};
const QString kCompileTime{QStringLiteral(__TIME__)};
const QString kBuildTime{kCompileDate + " " + kCompileTime};

const QString kTACConfigEditorApp{QStringLiteral("TAC Configuration Editor")};
const QString kTACConfigEditorVersion{QStringLiteral(TAC_CONFIG_VERSION)};

const QString kFTDICheckApp{QStringLiteral("FTDI Check")};
const QString kFTDICheckVersion{QStringLiteral(FTDI_CHECK_VERSION)};

const QString kInitialString{kAppName + ", version " + kAppVersion};
const QString kTACConfigurationGroup{kAppName};

#endif // QTACDEFINES_H
