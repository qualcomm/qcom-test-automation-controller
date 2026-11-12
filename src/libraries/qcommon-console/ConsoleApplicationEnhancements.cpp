// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ConsoleApplicationEnhancements.h"
#include "AppCore.h"
#include "Range.h"

// Qt
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>

#ifdef Q_OS_WINDOWS
	#include <windows.h>
	#include <shellapi.h>
	#include <shlobj.h>
	#pragma comment(lib, "shell32")
#endif
#ifdef Q_OS_LINUX
	#include <unistd.h>
#endif

#include <string>

const std::string kAppName("QTAC");

std::string applicationBinPath()
{
	const QString result = QDir::cleanPath(QCoreApplication::applicationDirPath())
	                       + QDir::separator();
	return result.toStdString();
}

std::string applicationDataPath()
{
	QString result;

	#ifdef Q_OS_WIN
		result = "C:/ProgramData/Qualcomm/" + QString::fromStdString(kAppName);
	#endif

	#ifdef Q_OS_LINUX
		result = "/var/lib/qcom/data/" + QString::fromStdString(kAppName);
	#endif

	return result.toStdString();
}

std::string documentsDataPath
(
	const std::string& append
)
{
	QString result;
	QString appName;

	appName = QCoreApplication::applicationName();
	if (appName.isEmpty())
		appName = QString::fromStdString(kAppName);

#ifdef Q_OS_WIN
	// QStandardPaths would return the "One Drive" location. Excel documents don't like living here
	result = QDir::homePath() + QDir::separator() + "Documents";
#else
	result = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif

	result = QDir::cleanPath(result + QDir::separator() + appName);
	if (append.empty() == false)
	{
		result = QDir::cleanPath(result + QDir::separator() + QString::fromStdString(append));
	}

	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result.toStdString();
}

QString toCamelCase
(
	const QString& s
)
{
	QStringList parts = s.split('_', Qt::SkipEmptyParts);
	for (auto i: basic_range<int>(1, parts.size()))
		parts[i].replace(0, 1, parts[i][0].toUpper());

	return parts.join("");
}

std::string defaultGlobalLoggingPath()
{
	QString result = QString::fromStdString(documentsDataPath("global"));
	result = QDir::cleanPath(result);
	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result.toStdString();
}

std::string defaultLoggingPath
(
	const std::string& appName
)
{
	QString result = QString::fromStdString(documentsDataPath(appName)) + QDir::separator() + "logs";
	result = QDir::cleanPath(result);
	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result.toStdString();
}

void renewInstallRegistry
(
	QSettings& settings,
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

	settings.setValue("Install UUID", QString::fromStdString(newInstallGuid));
	settings.setValue("Install Path", QCoreApplication::applicationDirPath().toLatin1());
}

bool isNewInstall
(
	QSettings& settings,
	const std::string& currentInstallGuid
)
{
	QString uuid = settings.value("Install UUID", QString("xxxx")).toString();
	return uuid.toStdString() != currentInstallGuid;
}

std::string createFilenameTimeStamp()
{
	return QDateTime::currentDateTime().toString("_yyyy_dd_MM_HH_mm_ss").toStdString();
}

std::string killOneDrive(const std::string& testPath, const std::string& revertPath)
{
	QString righteousPath{QString::fromStdString(testPath)};

	if (righteousPath.contains("OneDrive -", Qt::CaseInsensitive)) // We don't allow that abomination, OneDrive.  What a piece of crap!
		righteousPath = QString::fromStdString(revertPath);

	return righteousPath.toStdString();
}

std::string getModuleFilePath(const std::string &moduleFileName)
{
	QString result;
	QString platformModuleFileName{QString::fromStdString(moduleFileName)};

#ifdef Q_OS_WIN
	char path[_MAX_PATH + 1];
	platformModuleFileName += ".dll";
	GetModuleFileNameA(GetModuleHandleA(platformModuleFileName.toLatin1().data()), path, sizeof(path));
	result = QByteArray(path);
#endif

	 return result.toStdString();
}

void cleanIt(const std::string &pathToClean)
{
	QProcess* cleanItProc = new QProcess(Q_NULLPTR);

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

#ifdef Q_OS_WINDOWS
	privileged = IsUserAnAdmin();
#endif
#ifdef Q_OS_LINUX
	privileged = (geteuid() == 0);
#endif

	return privileged;
}

bool executeBinaryAsAdministrator(const std::string &binary, const std::string &cmdArgs)
{
	bool result{false};

	QString qBinary = QString::fromStdString(binary);
	QString qCmdArgs = QString::fromStdString(cmdArgs);

#ifdef Q_OS_WINDOWS
	SHELLEXECUTEINFO shExecInfo{0};

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = L"runas";
	shExecInfo.lpFile = (LPCWSTR)qBinary.utf16();
	shExecInfo.lpParameters = (LPCWSTR)qCmdArgs.utf16();
	shExecInfo.lpDirectory = NULL;
	shExecInfo.nShow = SW_SHOW;
	shExecInfo.hInstApp = NULL;

	if (ShellExecuteEx(&shExecInfo))
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
	QString result{QString::fromStdString(filePath)};

#ifdef Q_OS_LINUX
	// On Linux machines, QDir::homePath() is returned as / if $HOME is not set.
	// QTAC applications work fine as they are triggered from the shell which has $HOME set.
	// On implementations which does not involve script execution from shell, the expandPath API may return invalid path.

	if (result.startsWith("~"))
	{
		QString homePath = QDir::homePath();
		if (homePath.compare(QDir::separator()) != 0)
			result.replace("~", homePath);
		else
			result.replace("~", "/root");
	}
#endif

	return result.toStdString();
}

std::string tacConfigRoot()
{
	QString result = QString::fromStdString(applicationDataPath());

	QString deviceListJsonPath = result + "/configurations/devicelist.json";
	bool isdeviceListJsonPathPresent = QFile(deviceListJsonPath).exists();
	if (isdeviceListJsonPathPresent == false)
	{
		result = "../../../../configurations/";
	}
	else
	{
		result += "/configurations/";
	}

	return result.toStdString();
}
