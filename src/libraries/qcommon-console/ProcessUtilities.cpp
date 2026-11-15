// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ProcessUtilities.h"
#include "StringUtilities.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

bool isPlatformProcessRunning(const std::string& applicationPath);
std::string getModulePath();

bool isProcessRunning
(
	const std::string& applicationName
)
{
	return isPlatformProcessRunning(applicationName);
}

bool startProcess(const std::string &applicationName)
{
	bool result{false};

	std::string modulePath = getModulePath();
	if (!modulePath.empty())
	{
		std::filesystem::path moduleDir = std::filesystem::path(modulePath).parent_path();
		std::string appPath = (moduleDir / applicationName).lexically_normal().string();

#if defined(_WIN32) || defined(_WIN64)
		STARTUPINFOA si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

		if (CreateProcessA(appPath.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, moduleDir.string().c_str(), &si, &pi))
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			result = true;
		}
#else
		pid_t pid = fork();
		if (pid == 0)
		{
			if (chdir(moduleDir.string().c_str()) != 0)
			{
				_exit(126);
			}
			char* const argv[] = { const_cast<char*>(appPath.c_str()), nullptr };
			execv(appPath.c_str(), argv);
			_exit(127);
		}
		else if (pid > 0)
		{
			result = true;
		}
#endif
	}

	return result;
}

#if defined(_WIN32) || defined(_WIN64)

bool isPlatformProcessRunning
(
	const std::string& applicationPath
)
{
	bool result{false};

	std::string appPathLower = toLower(applicationPath);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32W);

		if (Process32FirstW(hProcessSnap, &pe32))
		{
			do
			{
				int size_needed = WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, NULL, 0, NULL, NULL);
				std::string modulePathStr;
				if (size_needed > 0)
				{
					modulePathStr.resize(size_needed);
					WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, &modulePathStr[0], size_needed, NULL, NULL);
					if (!modulePathStr.empty() && modulePathStr.back() == '\0')
						modulePathStr.pop_back();
				}
				std::string modulePathLower = toLower(modulePathStr);

				if (modulePathLower.find(appPathLower) != std::string::npos)
				{
					result = true;
					break;
				}
			} while (Process32NextW(hProcessSnap, &pe32));
		}

		CloseHandle(hProcessSnap);
	}

	return result;
}

std::string getModulePath()
{
	std::string result;

	WCHAR modulePath[MAX_PATH];
	HMODULE hModule{nullptr};

	if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCWSTR) &getModulePath, &hModule) != FALSE)
	{
		if (GetModuleFileNameW(hModule, modulePath, sizeof(modulePath) / sizeof(WCHAR)) != FALSE)
		{
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, modulePath, -1, NULL, 0, NULL, NULL);
			if (size_needed > 0)
			{
				result.resize(size_needed);
				WideCharToMultiByte(CP_UTF8, 0, modulePath, -1, &result[0], size_needed, NULL, NULL);
				if (!result.empty() && result.back() == '\0')
					result.pop_back();
			}
		}
	}

	return result;
}

#else

std::string getModulePath()
{
	std::string result;
	char buffer[1024];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	if (len != -1)
	{
		buffer[len] = '\0';
		result = std::string(buffer);
	}

	return result;
}

bool isPlatformProcessRunning(const std::string& applicationPath)
{
	std::string appLower = toLower(applicationPath);

	std::error_code ec;
	std::filesystem::directory_iterator dirIt("/proc", ec);
	if (ec)
	{
		AppCore::writeToApplicationLogLine("isPlatformProcessRunning: failed to open /proc: " + ec.message());
		return false;
	}

	for (const auto& entry : dirIt)
	{
		if (entry.is_directory(ec))
		{
			std::filesystem::path cmdFilePath = entry.path() / "cmdline";
			if (std::filesystem::exists(cmdFilePath, ec))
			{
				std::ifstream cmdFile(cmdFilePath, std::ios::binary);
				if (cmdFile.is_open())
				{
					std::string contents;
					std::getline(cmdFile, contents, '\0');
					if (contents.empty())
					{
						cmdFile.clear();
						cmdFile.seekg(0);
						std::getline(cmdFile, contents);
					}

					size_t spacePos = contents.find(' ');
					std::string firstPart = (spacePos != std::string::npos) ? contents.substr(0, spacePos) : contents;

					if (appLower == toLower(firstPart))
						return true;
				}
			}
		}
	}
	return false;
}

#endif
