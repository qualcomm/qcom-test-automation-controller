// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
