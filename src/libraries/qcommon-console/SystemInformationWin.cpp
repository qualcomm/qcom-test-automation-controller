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
