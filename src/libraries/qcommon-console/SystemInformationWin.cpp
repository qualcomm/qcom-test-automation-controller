// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SystemInformation.h"
#include "SystemUsername.h"

#include <cstdio>
#include <string>

#if defined(_WIN32) || defined(Q_OS_WIN)
// windows
#include <windows.h>
#include <lmcons.h>

static std::string queryRegistryString(HKEY rootKey, const char* subKey, const char* valueName, const std::string& defaultValue = "")
{
	HKEY hKey;
	if (RegOpenKeyExA(rootKey, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		char buffer[256];
		DWORD bufferSize = sizeof(buffer);
		DWORD type = 0;
		if (RegQueryValueExA(hKey, valueName, nullptr, &type, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			if (type == REG_SZ)
			{
				return std::string(buffer);
			}
		}
		RegCloseKey(hKey);
	}
	return defaultValue;
}
#endif

SystemInformation::SystemInformation()
{

}

std::string GetLastErrorAsString()
{
	std::string result;

#if defined(_WIN32) || defined(Q_OS_WIN)
	DWORD errorMessageID = ::GetLastError();
	if(errorMessageID == 0) {
		return result;
	}

	LPSTR messageBuffer = nullptr;

	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	result = std::string(messageBuffer, size);

	LocalFree(messageBuffer);
#endif
	return result;
}

std::string SystemInformation::computerName()
{
	if (_computerName.empty())
	{
#if defined(_WIN32) || defined(Q_OS_WIN)
		char temp[CNLEN + 1];
		DWORD tempLen = sizeof(temp);
		GetComputerNameA(temp, &tempLen);

		_computerName = std::string(temp);
#endif
	}

	return _computerName;
}

std::string SystemInformation::userName()
{
	if (_userName.empty())
	{
		SystemUsername userInfo;
		_userName = userInfo.userName();
	}
	return _userName;
}

std::string SystemInformation::osName()
{
	if (_osName.empty())
	{
#if defined(_WIN32) || defined(Q_OS_WIN)
		_osName = queryRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", "unknown");
		_osVersion = queryRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ReleaseId", "");
#endif
	}

	return _osName;
}

std::string SystemInformation::osVersion()
{
	if (_osVersion.empty())
	{
#if defined(_WIN32) || defined(Q_OS_WIN)
		_osName = queryRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductName", "unknown");
		_osVersion = queryRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ReleaseId", "");
#endif
	}

	return _osVersion;
}

std::string SystemInformation::totalPhysicalMemory()
{
	if (_totalPhysicalMemory.empty())
	{
#if defined(_WIN32) || defined(Q_OS_WIN)
		ULONGLONG memoryInKilobytes;

		if (::GetPhysicallyInstalledSystemMemory(&memoryInKilobytes) != FALSE)
		{
			double kiloBytes = (static_cast<double>(memoryInKilobytes) / 1024.0 / 1024.0);
			char memBuf[64];
			std::snprintf(memBuf, sizeof(memBuf), "%.2f", kiloBytes);
			_totalPhysicalMemory = memBuf;
		}
		else
		{
			_totalPhysicalMemory = "unknown";
		}
#endif
	}

	return _totalPhysicalMemory;
}

std::string SystemInformation::totalVirtualMemory()
{
	std::string result("Unknown");

#if defined(_WIN32) || defined(Q_OS_WIN)
	ULONGLONG memoryInKilobytes;

	if (::GetPhysicallyInstalledSystemMemory(&memoryInKilobytes) != FALSE)
	{
		double kiloBytes = (static_cast<double>(memoryInKilobytes) / 1024.0 / 1024.0);
		char memBuf[64];
		std::snprintf(memBuf, sizeof(memBuf), "%.2f", kiloBytes);
		result = memBuf;
	}
#endif
	return result;
}
