/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
		Author: Michael Simpson (msimpson@qti.qualcomm.com)
				Biswajit Roy (biswroy@qti.qualcomm.com)
*/

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


QString applicationBinPath()
{
	QString result;
	QString appName{"QTAC"};

#ifdef Q_OS_WIN
	result = "C:/Program Files (x86)/Qualcomm/" + appName + "/";
#endif

#ifdef Q_OS_LINUX
	result = "/opt/qcom/" + appName + "/bin/";
#endif

	result = QDir::cleanPath(result);

	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result;
}

QString applicationDataPath()
{
	QString result;

#ifdef Q_OS_WIN
	result = "C:/ProgramData/Qualcomm/QTAC/";
#endif

#ifdef Q_OS_LINUX
	result = "/var/lib/qcom/data/QTAC/";
#endif

	result = QDir::cleanPath(result);

	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result;
}

QString documentsDataPath
(
	const QString& append
)
{
	QString result;
	QString appName;

	appName = QCoreApplication::applicationName();
	if (appName.isEmpty())
		appName = "QTAC";

#ifdef Q_OS_WIN
	// QStandardPaths would return the "One Drive" location. Excel documents don't like living here
	result = QDir::homePath() + QDir::separator() + "Documents";
#else
	result = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif

	result = QDir::cleanPath(result + QDir::separator() + appName);
	if (append.isEmpty() == false)
	{
		result = QDir::cleanPath(result + QDir::separator() + append);
	}

	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result;
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

QString defaultGlobalLoggingPath()
{
	QString result = documentsDataPath("global");
	result = QDir::cleanPath(result);
	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result;
}

QString defaultLoggingPath
(
	const QString& appName
)
{
	QString result = documentsDataPath(appName) + QDir::separator() + "logs";
	result = QDir::cleanPath(result);
	if (QDir(result).exists() == false)
		QDir().mkpath(result);

	return result;
}

void renewInstallRegistry
(
	QSettings& settings,
	const QString& newInstallGuid
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
	settings.setValue("Install Path", QCoreApplication::applicationDirPath().toLatin1());
}

bool isNewInstall
(
	QSettings& settings,
	const QString& currentInstallGuid
)
{
	QString uuid = settings.value("Install UUID", QString("xxxx")).toString();
	return uuid != currentInstallGuid;
}

QString createFilenameTimeStamp()
{
	return QDateTime::currentDateTime().toString("_yyyy_dd_MM_HH_mm_ss");
}


QString killOneDrive(const QString& testPath, const QString& revertPath)
{
	QString righteousPath{testPath};

	if (righteousPath.contains("OneDrive -", Qt::CaseInsensitive)) // We don't allow that abomination, OneDrive.  What a piece of crap!
		righteousPath = revertPath;

	return righteousPath;
}

QString getModuleFilePath(const QString &moduleFileName)
{
	QString result;
	QString platformModuleFileName{moduleFileName};

#ifdef Q_OS_WIN
	char path[_MAX_PATH + 1];
	platformModuleFileName += ".dll";
	GetModuleFileNameA(GetModuleHandleA(platformModuleFileName.toLatin1().data()), path, sizeof(path));
	result = QByteArray(path);
#endif

	 return result;
}

void kickIt()
{
	QProcess* kickItProc = new QProcess(Q_NULLPTR);

	QStringList arguments;

	arguments << "upload" << "-telematics";

	if (kickItProc->startDetached("qik.exe", arguments))
	{
		kickItProc->waitForStarted();
	}

	kickItProc->deleteLater();
}

void cleanIt(const QString &pathToClean)
{
	QProcess* cleanItProc = new QProcess(Q_NULLPTR);

	QStringList arguments;

	arguments << pathToClean;

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

bool executeBinaryAsAdministrator(const QString &binary, const QString &cmdArgs)
{
	bool result{false};

#ifdef Q_OS_WINDOWS
	SHELLEXECUTEINFO shExecInfo{0};

	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.hwnd = NULL;
	shExecInfo.lpVerb = L"runas";
	shExecInfo.lpFile = (LPCWSTR)binary.utf16();
	shExecInfo.lpParameters = (LPCWSTR)cmdArgs.utf16();
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

	process.start("sudo", {binary, cmdArgs});
	if (process.waitForFinished())
	{
		if(process.exitCode() == 0)
			result = true;
	}
#endif

	return result;
}

QString expandPath(const QString &filePath)
{
	QString result{filePath};

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

	return result;
}

QString tacConfigRoot(bool expandThePath)
{
	Q_UNUSED(expandThePath);

	QString result = applicationDataPath() + "/tac_configs/";

	return result;
}
