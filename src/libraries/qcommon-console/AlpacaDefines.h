#ifndef ALPACADEFINES_H
#define ALPACADEFINES_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"
#include "StringProof.h"
#include "version.h"

#include <cstdint>
#include <string>

const std::string kProductName("QTAC");
const char kProductID[] =	"1e48f695-c109-11ec-aebb-063166a9270b"; // QTAC External
const char kCoreFeature[] = "1e71efc7-c109-11ec-aebb-063166a9270b";

const std::string kOrganizationName("Qualcomm, Inc.");
const std::string kProductVersion(QTAC_VERSION);
const std::string kCompileDate(__DATE__);
const std::string kCompileTime(__TIME__);
const std::string kBuildTime(kCompileDate + " " + kCompileTime);
const std::string kVersionGUID("{6F01E0AB-1962-4054-C061-3CA7CA4397E0}"); // produced by GUID Maker

uint32_t QCOMMONCONSOLE_EXPORT makeFirmwareVersion(uint32_t hw, uint32_t major, uint32_t minor);

#endif // ALPACADEFINES_H
