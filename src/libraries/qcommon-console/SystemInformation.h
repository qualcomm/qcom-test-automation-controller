#ifndef SYSTEMINFORMATION_H
#define SYSTEMINFORMATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include <string>

class QCOMMONCONSOLE_EXPORT SystemInformation
{
public:
	SystemInformation();

	std::string computerName();
	std::string userName();
	std::string osName();
	std::string osVersion();

	// memory
	std::string totalPhysicalMemory();
	std::string totalVirtualMemory();

private:
	std::string					_computerName;
	std::string					_userName;
	std::string					_osName;
	std::string					_osVersion;
	std::string					_totalPhysicalMemory;
};

#endif // SYSTEMINFORMATION_H
