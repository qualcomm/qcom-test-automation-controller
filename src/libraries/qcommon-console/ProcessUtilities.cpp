// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ProcessUtilities.h"

// QT
#include <QCoreApplication>
#include <QProcess>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

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

		QStringList arguments;
		result = QProcess::startDetached(QString::fromStdString(appPath), arguments, QString::fromStdString(moduleDir.string()));
	}

	return result;
}

#ifdef Q_OS_WIN

#include <windows.h>
#include <tlhelp32.h>

bool isPlatformProcessRunning
(
	const std::string& applicationPath
)
{
	bool result{false};

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32W pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32W);

		if (Process32FirstW(hProcessSnap, &pe32))
		{
			std::vector<std::string> processes;

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
				for (char &c : modulePathStr)
					c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

				if (std::find(processes.begin(), processes.end(), modulePathStr) == processes.end())
					processes.push_back(modulePathStr);
			} while (Process32NextW(hProcessSnap, &pe32));

			std::string appPathLower = applicationPath;
			for (char &c : appPathLower)
				c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

			for (const auto& processName : processes)
			{
				if (processName.find(appPathLower) != std::string::npos)
				{
					result = true;
					break;
				}
			}
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

	QCoreApplication* instance = QCoreApplication::instance();
	if (instance != nullptr)
		result = instance->applicationDirPath().toStdString();

	return result;
}

bool isPlatformProcessRunning(const std::string& applicationPath)
{
	std::string appLower = applicationPath;
	for (char &c : appLower)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	std::error_code ec;
	std::filesystem::directory_iterator dirIt("/proc", ec);
	if (ec)
		return false;

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
					for (char &c : firstPart)
						c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

					if (appLower == firstPart)
						return true;
				}
			}
		}
	}
	return false;
}

#endif
