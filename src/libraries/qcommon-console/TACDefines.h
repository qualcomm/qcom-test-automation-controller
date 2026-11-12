#ifndef TACDEFINES_H
#define TACDEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "version.h"

#include <string>

const std::string kAppName("TAC");
const std::string kAppVersion(QTAC_VERSION);

const std::string kTACConfigEditorApp("TAC Configuration Editor");
const std::string kTACConfigEditorVersion(TAC_CONFIG_VERSION);

const std::string kFTDICheckApp("FTDI Check");
const std::string kFTDICheckVersion(FTDI_CHECK_VERSION);

const std::string kInitialString(kAppName + ", version " + kAppVersion);

const std::string kTACConfigurationGroup(kAppName);

#endif // TACDEFINES_H
