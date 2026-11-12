// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ConsoleApplicationEnhancements.h"
#include "AppCore.h"
#include "IniSettings.h"
#include "StringUtilities.h"

// Qt
#include <QCoreApplication>
#include <QLibrary>
#include <QProcess>

#ifdef Q_OS_WINDOWS
	#include <windows.h>
	#include <shellapi.h>
	#include <shlobj.h>
	#pragma comment(lib, "shell32")
#endif
#ifdef Q_OS_LINUX
	#include <unistd.h>
#endif

#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <string>

const std::string kAppName("QTAC");

std::string applicationBinPath()
{
	std::filesystem::path p = std::filesystem::path(QCoreApplication::applicationDirPath().toStdString()).lexically_normal();
	std::string result = (p / "").string();
	return result;
}

std::string applicationDataPath()
{
	std::string result;

	#if defined(_WIN32) || defined(Q_OS_WIN)
		result = "C:/ProgramData/Qualcomm/" + kAppName;
	#endif

	#if defined(__linux__) || defined(Q_OS_LINUX)
		result = "/var/lib/qcom/data/" + kAppName;
	#endif

	return result;
}

std::string documentsDataPath
(
	const std::string& append
)
{
	std::string appName = QCoreApplication::applicationName().toStdString();
	if (appName.empty())
		appName = kAppName;

	std::filesystem::path basePath;

#if defined(_WIN32) || defined(Q_OS_WIN)
	const char* userProfile = std::getenv("USERPROFILE");
	if (userProfile != nullptr && *userProfile != '\0')
	{
		basePath = std::filesystem::path(userProfile) / "Documents";
	}
	else
	{
		basePath = "C:/Documents";
	}
#else
	const char* home = std::getenv("HOME");
	if (home != nullptr && *home != '\0')
	{
		basePath = std::filesystem::path(home) / "Documents";
	}
	else
	{
		basePath = "/root/Documents";
	}
#endif

	std::filesystem::path result = (basePath / appName).lexically_normal();
	if (!append.empty())
	{
		result = (result / append).lexically_normal();
	}

	std::error_code ec;
	if (!std::filesystem::exists(result, ec))
	{
		std::filesystem::create_directories(result, ec);
	}

	return result.string();
}

std::string defaultGlobalLoggingPath()
{
	std::filesystem::path result = std::filesystem::path(documentsDataPath("global")).lexically_normal();
	std::error_code ec;
	if (!std::filesystem::exists(result, ec))
	{
		std::filesystem::create_directories(result, ec);
	}

	return result.string();
}

std::string defaultLoggingPath
(
	const std::string& appName
)
{
	std::filesystem::path result = (std::filesystem::path(documentsDataPath(appName)) / "logs").lexically_normal();
	std::error_code ec;
	if (!std::filesystem::exists(result, ec))
	{
		std::filesystem::create_directories(result, ec);
	}

	return result.string();
}

void renewInstallRegistry
(
	IniSettings& settings,
	const std::string& newInstallGuid
)
{
	QLibrary cleanLib("InstallFinalize");

	if (cleanLib.load())
	{
		typedef void (*InstallProc)(const char* registryPath);

		InstallProc installProc = reinterpret_cast<InstallProc>(cleanLib.resolve("installProc"));
		if (installProc)
		{
			installProc("HKEY_CURRENT_USER\\Software\\Qualcomm, Inc.");
		}

		cleanLib.unload();
	}

	settings.setValue("Install UUID", newInstallGuid);
	settings.setValue("Install Path", applicationBinPath());
}

bool isNewInstall
(
	IniSettings& settings,
	const std::string& currentInstallGuid
)
{
	std::string uuid = settings.value("Install UUID", "xxxx").toString();
	return uuid != currentInstallGuid;
}

std::string createFilenameTimeStamp()
{
	return formatDateTime("_%Y_%d_%m_%H_%M_%S");
}

std::string killOneDrive(const std::string& testPath, const std::string& revertPath)
{
	std::string lowerTest = testPath;
	for (char &c : lowerTest)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	if (lowerTest.find("onedrive -") != std::string::npos) // We don't allow that abomination, OneDrive.
		return revertPath;

	return testPath;
}

std::string getModuleFilePath(const std::string &moduleFileName)
{
#if defined(_WIN32) || defined(Q_OS_WIN)
	char path[_MAX_PATH + 1];
	std::string platformModuleFileName = moduleFileName + ".dll";
	if (GetModuleFileNameA(GetModuleHandleA(platformModuleFileName.c_str()), path, sizeof(path)))
	{
		return std::string(path);
	}
	return std::string();
#else
	(void)moduleFileName;
	return std::string();
#endif
}

void cleanIt(const std::string &pathToClean)
{
	QProcess* cleanItProc = new QProcess(nullptr);

	QStringList arguments;

	arguments << QString::fromStdString(pathToClean);

	if (cleanItProc->startDetached("LogCleanup.exe", arguments))
	{
		cleanItProc->waitForStarted();
	}

	cleanItProc->deleteLater();
}

bool isUserPrivileged()
{
	bool privileged{false};

#if defined(_WIN32) || defined(Q_OS_WINDOWS)
	privileged = IsUserAnAdmin();
#endif
#if defined(__linux__) || defined(Q_OS_LINUX)
	privileged = (geteuid() == 0);
#endif

	return privileged;
}

bool executeBinaryAsAdministrator(const std::string &binary, const std::string &cmdArgs)
{
	bool result{false};

#if defined(_WIN32) || defined(Q_OS_WINDOWS)
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

		if(exitCode == 0)
			result = true;

		CloseHandle(shExecInfo.hProcess);
	}

#else
	QProcess process;
	QString qBinary = QString::fromStdString(binary);
	QString qCmdArgs = QString::fromStdString(cmdArgs);

	process.start("sudo", {qBinary, qCmdArgs});
	if (process.waitForFinished())
	{
		if(process.exitCode() == 0)
			result = true;
	}
#endif

	return result;
}

std::string expandPath(const std::string &filePath)
{
	std::string result = filePath;

#if defined(__linux__) || defined(Q_OS_LINUX)
	// On Linux machines, home directory
	if (!result.empty() && result[0] == '~')
	{
		const char* home = std::getenv("HOME");
		std::string homeStr = (home != nullptr && *home != '\0') ? home : "/root";
		result.replace(0, 1, homeStr);
	}
#endif

	return result;
}

std::string tacConfigRoot()
{
	std::string result = applicationDataPath();

	std::string deviceListJsonPath = result + "/configurations/devicelist.json";
	bool isdeviceListJsonPathPresent = std::filesystem::exists(deviceListJsonPath);
	if (!isdeviceListJsonPathPresent)
	{
		result = "../../../../configurations/";
	}
	else
	{
		result += "/configurations/";
	}

	return result;
}
