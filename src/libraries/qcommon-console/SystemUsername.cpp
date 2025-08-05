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
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "SystemUsername.h"
#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>
#include <QStringList>

// Windows
#ifdef Q_OS_WIN
#include "lmcons.h"
#endif

const int kEventBufferSize(4096);


SystemUsername::SystemUsername()
{
#ifdef Q_OS_LINUX
	getUserNameLinux();
#endif
#ifdef Q_OS_WIN
	getUserNameWin();
#endif
}

QString SystemUsername::userName()
{
	return _userName;
}

void SystemUsername::getUserNameLinux()
{
	char buffer[1024];
	int result(-1);
#ifdef Q_OS_LINUX
	result = getlogin_r(buffer, 1024);
#endif
	if (result == 0)
		_userName = QString(buffer);
}

void SystemUsername::getUserNameWin()
{
#ifdef Q_OS_WIN
	DWORD status = ERROR_SUCCESS;
	EVT_HANDLE hResults = NULL;
	const wchar_t* pwsPath = L"Qualcomm-SharedLogin-CredentialProvider/Audit";
	const wchar_t* pwsQuery = L"Event/System[EventID=20]";

	hResults = EvtQuery(NULL, pwsPath, pwsQuery, EvtQueryChannelPath | EvtQueryReverseDirection | EvtSeekRelativeToFirst);
	if (NULL == hResults)
	{
		status = GetLastError();

		if (ERROR_EVT_CHANNEL_NOT_FOUND == status)
		{
			char username[UNLEN+1];
			DWORD username_len = UNLEN+1;
			GetUserNameA(username, &username_len);
			_userName = QString(username);
		}
		else if (ERROR_EVT_INVALID_QUERY == status)
			wprintf(L"The query is not valid.\n");
		else
			wprintf(L"EvtQuery failed with %lu.\n", status);
		if (hResults)
			EvtClose(hResults);
	}

	fetchLatestEvent(hResults);
#endif
}

#ifdef Q_OS_WIN
DWORD SystemUsername::parseEvent(EVT_HANDLE hEvent)
{
//	QRegExp re("SubjectUserName\'>(.*)</Data><Data Name='SubjectDomainName");
	DWORD dwBufferUsed = kEventBufferSize*2;
	WCHAR renderedContent[kEventBufferSize];
	char eventDataStr[kEventBufferSize];
	unsigned long long bufferSize(0);

	EvtRender(NULL, hEvent, EvtRenderEventXml, kEventBufferSize*2, renderedContent, &dwBufferUsed, 0);
	wcstombs_s(&bufferSize, eventDataStr, renderedContent, kEventBufferSize);

	// fix me
	QString eventData{eventDataStr};

//	re.indexIn(eventData, 0);
	QString filteredStr(""/*re.cap(0)*/);

	// The first capture from the regex should contain our username. Generally, this is of the format "SubjectUserName'>biswroy</Data><Data Name='SubjectDomainName"
	// Check if there is actually a captured string. If not, we ignore.
	if (!filteredStr.isEmpty())
	{
		// Try to split the captured string by the '> character so as to obtain "SubjectUserName and biswroy</Data><Data Name='SubjectDomainName"
		QStringList chunkedStr = filteredStr.split("'>");
		// If the list length is > 1, we have some data that can be filtered further.
		if (chunkedStr.length() > 1)
		{
			// Split the second string by </ which gives biswroy and Data><Data Name='SubjectDomainName"
			chunkedStr = chunkedStr[1].split("</");
			// If we have some string
			if (chunkedStr.length() > 0)
				// It must be the username
				_userName = chunkedStr[0];
		}
	}

	return 0;
}

DWORD SystemUsername::fetchLatestEvent(EVT_HANDLE hResults)
{
	DWORD status = ERROR_SUCCESS;
	EVT_HANDLE hEvents[ARRAY_SIZE];
	DWORD dwReturned = 0;

	// Get a block of the latest event from the result set.
	if (!EvtNext(hResults, ARRAY_SIZE, hEvents, TIMEOUT, 0, &dwReturned))
	{
		status = GetLastError();
		if (dwReturned > 0 && hEvents[0] != NULL)
			EvtClose(hEvents[0]);
	}

	if (dwReturned > 0 && ERROR_SUCCESS == (status = parseEvent(hEvents[0])))
	{
		EvtClose(hEvents[0]);
		hEvents[0] = NULL;
	}
	else
	{
		if (dwReturned > 0 && NULL != hEvents[0])
			EvtClose(hEvents[0]);
	}
	return status;
}
#endif
