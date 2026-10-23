// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ConsoleApplicationEnhancements.h"
#include "AppCore.h"
#include "Range.h"

// Qt
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QTemporaryFile>

#ifdef Q_OS_WINDOWS
	#include <windows.h>
	#include <shellapi.h>
	#include <shlobj.h>
	#pragma comment(lib, "shell32")
#endif
#ifdef Q_OS_LINUX
	#include <pwd.h>
	#include <unistd.h>
#endif

const QString kAppName("QTAC");

namespace
{

// Confirms a directory exists and actually accepts writes.
//
// Existence alone is not enough. A home directory can be present but read
// only, full, or over its storage quota, and in those cases every later write
// fails. Probing with a real file is the only reliable test.
bool directoryIsWritable(const QString& path)
{
	QDir dir(path);

	if (path.isEmpty() || dir.exists() == false)
	{
		return false;
	}

	// The unique name template keeps concurrent processes from colliding, and
	// the probe removes itself when it goes out of scope.
	QTemporaryFile probe(dir.filePath(QStringLiteral("qtac-probe-XXXXXX.tmp")));
	probe.setAutoRemove(true);

	return probe.open();
}

} // namespace

QString applicationBinPath()
{
	const QString result = QDir::cleanPath(QCoreApplication::applicationDirPath())
	                       + QDir::separator();
	return result;
}

QString applicationDataPath()
{
	QString result;

	#ifdef Q_OS_WIN
		result = "C:/ProgramData/Qualcomm/" + kAppName;
	#endif

	#ifdef Q_OS_LINUX
		result = "/var/lib/qcom/data/" + kAppName;
	#endif

	return result;
}

// Returns the per user directory that holds configuration, or an empty string
// when no such directory can be established.
//
// Callers must treat an empty return value as "configuration storage is not
// available" and fail rather than carry on, because continuing without
// configuration means device settings cannot be read or written.
//
// Diagnostics here deliberately use qWarning() rather than the application log.
// The application log location is itself derived from this function, so logging
// through it would recurse.
QString documentsDataPath
(
	const QString& append
)
{
	QString base;
	QString appName;

	appName = QCoreApplication::applicationName();
	if (appName.isEmpty())
		appName = kAppName;

#ifdef Q_OS_WIN
	// QStandardPaths would return the "One Drive" location. Excel documents don't like living here
	base = QDir::homePath() + QDir::separator() + "Documents";
#else
	base = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif

	// QStandardPaths yields an empty string when it cannot resolve a location.
	// Concatenating onto that would silently produce a relative path rooted at
	// the working directory, so reject it instead.
	if (base.isEmpty())
	{
		qWarning() << "Alpaca: no documents location is available for the current user."
			<< "Configuration cannot be read or written.";
		return QString();
	}

	// QDir::homePath() falls back to the root directory when HOME is not set.
	// That is worth reporting, because configuration then lands somewhere the
	// user almost certainly did not intend. Whether it is usable is decided by
	// the writability check below rather than assumed here.
	if (QDir::cleanPath(QDir::homePath()) == QDir::rootPath())
	{
		qWarning() << "Alpaca: the home directory resolves to the root directory."
			<< "HOME is probably not set for this process.";
	}

	QString result = QDir::cleanPath(base + QDir::separator() + appName);
	if (append.isEmpty() == false)
	{
		result = QDir::cleanPath(result + QDir::separator() + append);
	}

	if (QDir(result).exists() == false)
	{
		if (QDir().mkpath(result) == false)
		{
			qWarning() << "Alpaca: unable to create the configuration directory" << result
				<< "- the location may be missing, read only, or out of space.";
			return QString();
		}
	}

	// The directory can exist and still refuse writes, for example when the
	// volume is full or the user is over quota.
	if (directoryIsWritable(result) == false)
	{
		qWarning() << "Alpaca: the configuration directory" << result
			<< "is not writable - the location may be read only, full, or over quota.";
		return QString();
	}

	return result;
}

bool documentsDataPathAvailable()
{
	return documentsDataPath(QString()).isEmpty() == false;
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
	// An empty base means there is nowhere to log. Returning empty keeps the
	// caller from building a path rooted at the filesystem root.
	QString result = documentsDataPath("global");
	if (result.isEmpty())
	{
		return QString();
	}

	result = QDir::cleanPath(result);
	if (QDir(result).exists() == false)
	{
		if (QDir().mkpath(result) == false)
		{
			return QString();
		}
	}

	return result;
}

QString defaultLoggingPath
(
	const QString& appName
)
{
	QString base = documentsDataPath(appName);
	if (base.isEmpty())
	{
		return QString();
	}

	QString result = QDir::cleanPath(base + QDir::separator() + "logs");
	if (QDir(result).exists() == false)
	{
		if (QDir().mkpath(result) == false)
		{
			return QString();
		}
	}

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
		if (homePath.compare(QDir::separator()) == 0)
		{
			// HOME is not set. Ask the password database for the real home of
			// the current user instead of assuming /root, which is wrong for
			// every non-root user.
			const struct passwd* pw = getpwuid(getuid());
			if (pw != Q_NULLPTR && pw->pw_dir != Q_NULLPTR && pw->pw_dir[0] != '\0')
			{
				homePath = QString::fromLocal8Bit(pw->pw_dir);
			}
			qWarning() << "expandPath: HOME is not set, using" << homePath << "for ~";
		}
		result.replace(0, 1, homePath);
	}
#endif

	return result;
}

// Returns the folder holding devicelist.json and the stock configurations,
// always with a trailing separator because callers append a file name.
//
// The data folder is the same for every product layout, so it is checked
// first. When it is missing (developer builds, or a damaged install) the
// folders above the executable are searched. The result is always absolute;
// the old fallback was relative to the working directory and silently
// pointed at the wrong place whenever the application was started from
// somewhere other than its build folder.
QString tacConfigRoot()
{
	static bool reportedFallback{false};
	static bool reportedMissing{false};

	const QString kDeviceList(QStringLiteral("devicelist.json"));
	const QString dataRoot = QDir::cleanPath(applicationDataPath() + "/configurations");

	if (QFile::exists(dataRoot + "/" + kDeviceList))
	{
		return dataRoot + "/";
	}

	// applicationDirPath() needs a QCoreApplication. TACDev can be loaded by
	// hosts that have none, so only search next to the executable when one
	// exists.
	if (QCoreApplication::instance() != Q_NULLPTR)
	{
		QDir dir(QCoreApplication::applicationDirPath());

		for (int i = 0; i < 5; ++i)
		{
			const QString candidate = QDir::cleanPath(dir.absolutePath() + "/configurations");
			if (QFile::exists(candidate + "/" + kDeviceList))
			{
				if (reportedFallback == false)
				{
					reportedFallback = true;
					qWarning() << "tacConfigRoot:" << dataRoot << "has no" << kDeviceList
						<< "- using" << candidate;
				}
				return candidate + "/";
			}

			if (dir.cdUp() == false)
				break;
		}
	}

	if (reportedMissing == false)
	{
		reportedMissing = true;
		qWarning() << "tacConfigRoot: no" << kDeviceList << "found in" << dataRoot
			<< "or above the executable - configurations will not load";
	}

	return dataRoot + "/";
}

// Writes the resolved install and data locations to the debug output once at
// startup, so a wrong path in the field can be diagnosed from the log alone.
void logInstallLayout()
{
	const QString exeDir = QCoreApplication::instance() != Q_NULLPTR
		? QDir::cleanPath(QCoreApplication::applicationDirPath())
		: QStringLiteral("<no application>");

	qInfo().noquote() << "Install layout: executable folder =" << exeDir;
	qInfo().noquote() << "Install layout: data folder       =" << applicationDataPath();
	qInfo().noquote() << "Install layout: configurations    =" << tacConfigRoot();
	qInfo().noquote() << "Install layout: documents folder  =" << documentsDataPath(QString());
}
