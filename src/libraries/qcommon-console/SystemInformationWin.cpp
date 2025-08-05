// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "SystemInformation.h"
#include "SystemUsername.h"

// Qt
#include <QDebug>
#include <QSettings>

#ifdef Q_OS_WIN
// windows
#include <windows.h>
#include <lmcons.h>

const QString KVersionPath(QStringLiteral("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"));
#endif

SystemInformation::SystemInformation()
{

}

QString GetLastErrorAsString()
{
	QString result;

#ifdef Q_OS_WIN
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if(errorMessageID == 0) {
		return QString(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
								 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	result = QString::fromStdString(message);
#endif
	return result;
}

QString SystemInformation::computerName()
{
	if (_computerName.isEmpty())
	{
#ifdef Q_OS_WIN
		char temp[CNLEN + 1];
		DWORD tempLen = sizeof(temp);
		GetComputerNameA(temp, &tempLen);

		_computerName = QString(temp);
#endif
	}

	return _computerName;
}

QString SystemInformation::userName()
{
	if (_userName.isEmpty())
	{
		SystemUsername userInfo;
		_userName = userInfo.userName();
	}
	return _userName;
}

QString SystemInformation::osName()
{
	if (_osName.isEmpty())
	{

#ifdef Q_OS_WIN
		QSettings settings(KVersionPath, QSettings::NativeFormat);

		_osName = settings.value("ProductName", QString("unknown")).toString();
		_osVersion = settings.value("ReleaseId").toString();
#endif
	}

	return _osName;
}

QString SystemInformation::osVersion()
{
	if (_osVersion.isEmpty())
	{
#ifdef Q_OS_WIN
		QSettings settings(KVersionPath, QSettings::NativeFormat);

		_osName = settings.value("ProductName", QString("unknown")).toString();
		_osVersion = settings.value("ReleaseId").toString();
#endif
	}

	return _osVersion;
}

QString SystemInformation::totalPhysicalMemory()
{
	if (_totalPhysicalMemory.isEmpty())
	{
#ifdef Q_OS_WIN
		ULONGLONG memoryInKilobytes;

		if (::GetPhysicallyInstalledSystemMemory(&memoryInKilobytes) != FALSE)
		{
			qreal kiloBytes = (static_cast<qreal>(memoryInKilobytes) / 1024.0 / 1024.0);
			_totalPhysicalMemory = QString::number(kiloBytes);
		}
		else
		{
			_totalPhysicalMemory = "unknown";
		}
#endif
	}

	return _totalPhysicalMemory;
}

QString SystemInformation::totalVirtualMemory()
{
	QString result("Unknown");

#ifdef Q_OS_WIN
	ULONGLONG memoryInKilobytes;

	if (::GetPhysicallyInstalledSystemMemory(&memoryInKilobytes) != FALSE)
	{
		qreal kiloBytes = (static_cast<qreal>(memoryInKilobytes) / 1024.0 / 1024.0);
		result = QString::number(kiloBytes);
	}
#endif
	return result;
}
