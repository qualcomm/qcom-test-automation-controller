// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <map>
#include <string>
#include <vector>

enum ArchType { eUnknown, eX32, eX64, eARM };

class FTDICheckApplication
{
public:
    FTDICheckApplication();

    int  defaultActivity();
    void showVersion();

    bool verifyInstalledDriver();
    bool uninstallDriver();
    bool installDriver();

private:
    ArchType getArchitecture();

    bool detectLibrary(const std::string& libraryName);
    bool checkLibraryVersion(const std::string& filePath);
    bool isExpectedProductVersion(const std::string& fileName, const std::string& version);
    bool isFTDIDriverInfFile(const std::string& filePath);
    bool isWindowsFTDIDriverFilesPresent();
    bool isLinuxFTDIDriverFilesPresent();
    bool isDriverLatest();
    bool deleteWindowsDriver(const std::string& infFile);
    bool addWindowsDriver(const std::string& infFile);
    bool deleteLinuxDriver();
    bool addLinuxDriver();

    std::vector<std::string>           _detectedFiles;
    std::map<std::string, std::string> _expectedProductVersionMap;
};
