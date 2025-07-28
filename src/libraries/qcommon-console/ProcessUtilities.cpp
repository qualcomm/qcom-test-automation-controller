// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2013-2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "ProcessUtilities.h"


// QCommon
#include <Range.h>

// QT
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QSet>
#include <QStringList>

bool isPlatformProcessRunning(const QString& applicationPath);
QString getModulePath(void);

bool isProcessRunning
(
	const QString& applicationName
)
{
	QString candidatePath{applicationName.toLower()};

	return isPlatformProcessRunning(candidatePath);
}

bool startProcess(const QString &applicationName)
{
	bool result{false};

	QString modulePath = getModulePath();
	if (modulePath.isEmpty() == false)
	{
		QFileInfo fileInfo(modulePath);
		QStringList arguments;

		QString appPath = QDir::cleanPath(fileInfo.absolutePath() + QDir::separator() + applicationName);

		result = QProcess::startDetached(appPath, arguments, fileInfo.absolutePath());
	}

	return result;
}

#ifdef Q_OS_WIN

#include <windows.h>
#include <tlhelp32.h>

#include <string>

bool isPlatformProcessRunning
(
	const QString& applicationPath
)
{
	bool result{false};

	std::wstring modulePath;
	HANDLE hProcessSnap;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe32;

		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hProcessSnap, &pe32))
		{
			QStringList processes;

			do
			{
				QString modulePathStr = QString::fromStdWString(std::wstring(&pe32.szExeFile[0])).toLower();
				if (processes.contains(modulePathStr) == false)
					processes.append(modulePathStr);
			} while (Process32Next(hProcessSnap, &pe32));

#ifdef DEBUG
			auto sortModuleLambda = [] (const QString& module1, const QString& module2) -> bool
			{
				return module1 < module2;
			};

			// this helps with interactive debugging only
			std::sort(processes.begin(), processes.end(), sortModuleLambda);
#endif
			for (const auto& processName: processes)
			{
				if (processName.contains(applicationPath))
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

QString getModulePath()
{
	QString result;

	WCHAR modulePath[MAX_PATH];
	HMODULE hModule{Q_NULLPTR};

	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCWSTR) &getModulePath, &hModule) != FALSE)
	{
		if (GetModuleFileName(hModule, modulePath, sizeof(modulePath)) != FALSE)
		{
			std::wstring moduleName = std::wstring(&modulePath[0]);
			result = QString::fromStdWString(modulePath);
		}
	}

	return result;
}


#else

QString getModulePath()
{
    QString result;

    QCoreApplication* instance = QCoreApplication::instance();
    if (instance != Q_NULLPTR)
        result = instance->applicationDirPath();

    return result;
}

bool isPlatformProcessRunning(const QString& applicationPath)
{
    QDir rootDir("/proc");

    QFileInfoList list = rootDir.entryInfoList();

    for (const auto index: range(list.size()))
    {
        QFileInfo fileInfo = list.at(index);

        if (fileInfo.isDir())
        {
            QString cmdFilePath = fileInfo.absoluteFilePath()+ QDir::separator() + "cmdline";
            QFileInfo cmdFile(cmdFilePath);
            if (cmdFile.exists())
            {
                QFile cmdlinefile(cmdFile.absoluteFilePath());
                if (cmdlinefile.open(QIODevice::ReadOnly))
                {
                    QByteArray contents = cmdlinefile.readAll();
                    QList<QByteArray> parts = contents.split(' ');

                    cmdlinefile.close();

                    if (applicationPath.toLower() == parts.at(0).toLower())
                        return true;
                }
            }
        }
    }
	return false;
}

#endif


