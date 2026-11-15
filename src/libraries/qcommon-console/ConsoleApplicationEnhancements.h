#ifndef CONSOLEAPPLICATIONENHANCEMENTS_H
#define CONSOLEAPPLICATIONENHANCEMENTS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonConsoleGlobal.h"
#include "AppCore.h"

#include <string>

class INISettings;
using IniSettings = INISettings;

// --- User Preferences & Default Behavior Enhancements ---
std::string QCOMMONCONSOLE_EXPORT documentsDataPath(const std::string& append = "");
std::string QCOMMONCONSOLE_EXPORT defaultGlobalLoggingPath();
std::string QCOMMONCONSOLE_EXPORT defaultLoggingPath(const std::string& appName = "");
std::string QCOMMONCONSOLE_EXPORT killOneDrive(const std::string& testPath, const std::string& revertPath);
std::string QCOMMONCONSOLE_EXPORT getModuleFilePath(const std::string& moduleFileName);
std::string QCOMMONCONSOLE_EXPORT expandPath(const std::string& filePath);

// --- Configuration Root Query (Queries AppCore critical configurations path) ---
std::string QCOMMONCONSOLE_EXPORT tacConfigRoot();

// --- Critical Path Forwarders for Console Enhancements Compatibility ---
std::string QCOMMONCONSOLE_EXPORT applicationBinPath();
std::string QCOMMONCONSOLE_EXPORT applicationDataPath();
std::string QCOMMONCONSOLE_EXPORT userConfigurationsPath();

void cleanIt(const std::string& pathToClean);

bool QCOMMONCONSOLE_EXPORT isUserPrivileged();
bool QCOMMONCONSOLE_EXPORT executeBinaryAsAdministrator(const std::string &binary, const std::string &cmdArgs);

#endif // CONSOLEAPPLICATIONENHANCEMENTS_H
