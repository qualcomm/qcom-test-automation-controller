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


