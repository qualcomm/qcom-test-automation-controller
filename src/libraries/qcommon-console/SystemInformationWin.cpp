// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SystemInformation.h"
#include "SystemUsername.h"

// Qt
#include <QDebug>
#include <QSettings>

#include <string>

#ifdef Q_OS_WIN
// windows
#include <windows.h>
#include <lmcons.h>

const QString KVersionPath(QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"));
#endif

SystemInformation::SystemInformation()
{

}

std::string GetLastErrorAsString()
{
	std::string result;

#ifdef Q_OS_WIN
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
#ifdef Q_OS_WIN
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

#ifdef Q_OS_WIN
		QSettings settings(KVersionPath, QSettings::NativeFormat);

		_osName = settings.value("ProductName", QString("unknown")).toString().toStdString();
		_osVersion = settings.value("ReleaseId").toString().toStdString();
#endif
	}

	return _osName;
}

std::string SystemInformation::osVersion()
{
	if (_osVersion.empty())
	{
#ifdef Q_OS_WIN
		QSettings settings(KVersionPath, QSettings::NativeFormat);

		_osName = settings.value("ProductName", QString("unknown")).toString().toStdString();
		_osVersion = settings.value("ReleaseId").toString().toStdString();
#endif
	}

	return _osVersion;
}

std::string SystemInformation::totalPhysicalMemory()
{
	if (_totalPhysicalMemory.empty())
	{
#ifdef Q_OS_WIN
		ULONGLONG memoryInKilobytes;

		if (::GetPhysicallyInstalledSystemMemory(&memoryInKilobytes) != FALSE)
		{
			double kiloBytes = (static_cast<double>(memoryInKilobytes) / 1024.0 / 1024.0);
			_totalPhysicalMemory = QString::number(kiloBytes).toStdString();
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

#ifdef Q_OS_WIN
	ULONGLONG memoryInKilobytes;

	if (::GetPhysicallyInstalledSystemMemory(&memoryInKilobytes) != FALSE)
	{
		double kiloBytes = (static_cast<double>(memoryInKilobytes) / 1024.0 / 1024.0);
		result = QString::number(kiloBytes).toStdString();
	}
#endif
	return result;
}
