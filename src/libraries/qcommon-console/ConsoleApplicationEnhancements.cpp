// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ConsoleApplicationEnhancements.h"
#include "AppCore.h"
#include "AlpacaDefines.h"
#include "StringUtilities.h"

#if defined(_WIN32) || defined(_WIN64)
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
	#include <shellapi.h>
	#include <shlobj.h>
	#pragma comment(lib, "shell32")
#endif

#if defined(__linux__)
	#include <pwd.h>
	#include <unistd.h>
#endif

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>

// --- User Preferences & Default Behavior Enhancements ---

std::string killOneDrive(const std::string& testPath, const std::string& revertPath)
{
	if (contains(testPath, "onedrive -", false))
		return revertPath;

	return testPath;
}

std::string documentsDataPath(const std::string& append)
{
	const std::string rootDir = kProductName;

	std::filesystem::path basePath;

#if defined(_WIN32) || defined(_WIN64)
	char docPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_MYDOCUMENTS, NULL, 0, docPath)))
	{
		basePath = docPath;
	}
	else
	{
		basePath = "C:/Documents";
	}
	basePath = killOneDrive(basePath.generic_string(), "C:/Documents");
#elif defined(__linux__)
	const struct passwd* pw = getpwuid(getuid());
	if (pw != nullptr && pw->pw_dir != nullptr && pw->pw_dir[0] != '\0')
		basePath = std::filesystem::path(pw->pw_dir) / "Documents";
	else
		basePath = "/root/Documents";
#else
	basePath = "Documents";
#endif

	std::filesystem::path result = (basePath / rootDir).lexically_normal();
	if (!append.empty())
	{
		result = (result / append).lexically_normal();
	}

	std::error_code ec;
	if (!std::filesystem::exists(result, ec))
	{
		std::filesystem::create_directories(result, ec);
	}

	return result.generic_string();
}

std::string defaultGlobalLoggingPath()
{
	std::filesystem::path result = std::filesystem::path(documentsDataPath("global")).lexically_normal();
	std::error_code ec;
	if (!std::filesystem::exists(result, ec))
	{
		std::filesystem::create_directories(result, ec);
	}
	return result.generic_string();
}

std::string defaultLoggingPath(const std::string& appName)
{
	std::string name = appName;
	if (name.empty())
	{
		AppCore* appCore = AppCore::getAppCore();
		if (appCore != nullptr && !appCore->appName().empty())
			name = appCore->appName();
		else
			name = kProductName;
	}

	std::filesystem::path result = (std::filesystem::path(documentsDataPath(name)) / "logs").lexically_normal();
	std::error_code ec;
	if (!std::filesystem::exists(result, ec))
	{
		std::filesystem::create_directories(result, ec);
	}
	return result.generic_string();
}

std::string expandPath(const std::string& filePath)
{
	std::string result = filePath;

#if defined(__linux__)
	if (!result.empty() && result[0] == '~')
	{
		const struct passwd* pw = getpwuid(getuid());
		std::string homeStr = (pw != nullptr && pw->pw_dir != nullptr && pw->pw_dir[0] != '\0')
			? pw->pw_dir
			: "/root";
		result.replace(0, 1, homeStr);
	}
#endif

	return result;
}

std::string getModuleFilePath(const std::string& moduleFileName)
{
#if defined(_WIN32) || defined(_WIN64)
	char path[_MAX_PATH + 1];
	std::string platformModuleFileName = moduleFileName + ".dll";
	if (GetModuleFileNameA(GetModuleHandleA(platformModuleFileName.c_str()), path, sizeof(path)))
	{
		return std::filesystem::path(path).lexically_normal().generic_string();
	}
	return std::string();
#else
	(void)moduleFileName;
	return std::string();
#endif
}

// --- Configuration Root Query (Queries AppCore critical configurations path) ---

std::string tacConfigRoot()
{
	return AppCore::configurationsPath();
}

// --- Critical Path Forwarders for Console Enhancements Compatibility ---

std::string applicationBinPath()
{
	return AppCore::applicationBinPath();
}

std::string applicationDataPath()
{
	return AppCore::applicationDataPath();
}

std::string firmwarePath()
{
	return AppCore::firmwarePath();
}

std::string userConfigurationsPath()
{
	return AppCore::userConfigurationsPath();
}

// --- Process & Privilege Utilities ---

void cleanIt(const std::string &pathToClean)
{
#if defined(_WIN32) || defined(_WIN64)
	ShellExecuteA(NULL, "open", "LogCleanup.exe", pathToClean.c_str(), NULL, SW_HIDE);
#else
	(void)pathToClean;
#endif
}

bool isUserPrivileged()
{
	bool privileged{false};

#if defined(_WIN32) || defined(_WIN64)
	privileged = IsUserAnAdmin();
#elif defined(__linux__)
	privileged = (geteuid() == 0);
#endif

	return privileged;
}

bool executeBinaryAsAdministrator(const std::string &binary, const std::string &cmdArgs)
{
	bool result{false};

#if defined(_WIN32) || defined(_WIN64)
	SHELLEXECUTEINFOA shExecInfo{0};

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = "runas";
	shExecInfo.lpFile = binary.c_str();
	shExecInfo.lpParameters = cmdArgs.c_str();
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOW;
	shExecInfo.hInstApp = NULL;

	if (ShellExecuteExA(&shExecInfo))
	{
		WaitForSingleObject(shExecInfo.hProcess, INFINITE);

		DWORD exitCode(0);
		GetExitCodeProcess(shExecInfo.hProcess, &exitCode);

		if (exitCode == 0)
			result = true;

		CloseHandle(shExecInfo.hProcess);
	}

#elif defined(__linux__)
	std::string cmd = "sudo " + binary + " " + cmdArgs;
	int ret = std::system(cmd.c_str());
	if (ret == 0)
		result = true;
#endif

	return result;
}
