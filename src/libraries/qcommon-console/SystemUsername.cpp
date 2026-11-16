// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "SystemUsername.h"
#include "AppCore.h"

#include <cstring>

// Windows
#if defined(_WIN32) || defined(_WIN64)
#include "lmcons.h"
#endif

const int kEventBufferSize(4096);

SystemUsername::SystemUsername()
{
#if defined(__linux__)
	getUserNameLinux();
#endif
#if defined(_WIN32) || defined(_WIN64)
	getUserNameWin();
#endif
}

std::string SystemUsername::userName()
{
	return _userName;
}

void SystemUsername::getUserNameLinux()
{
	char buffer[1024];
	int result(-1);
#if defined(__linux__)
	result = getlogin_r(buffer, 1024);
#endif
	if (result == 0)
	{
		_userName = std::string(buffer);
	}
	else
	{
		AppCore::writeToApplicationLogLine("SystemUsername::getUserNameLinux: getlogin_r failed with error code " + std::to_string(result));
	}
}

void SystemUsername::getUserNameWin()
{
#if defined(_WIN32) || defined(_WIN64)
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
			if (GetUserNameA(username, &username_len))
			{
				_userName = std::string(username);
			}
			else
			{
				DWORD uErr = GetLastError();
				AppCore::writeToApplicationLogLine("SystemUsername::getUserNameWin: GetUserNameA fallback failed with error code " + std::to_string(uErr));
			}
		}
		else if (ERROR_EVT_INVALID_QUERY == status)
		{
			AppCore::writeToApplicationLogLine("SystemUsername::getUserNameWin: EvtQuery query is invalid (code " + std::to_string(status) + ")");
		}
		else
		{
			AppCore::writeToApplicationLogLine("SystemUsername::getUserNameWin: EvtQuery failed with error code " + std::to_string(status));
		}
	}
	else
	{
		fetchLatestEvent(hResults);
		EvtClose(hResults);
	}
#endif
}

#if defined(_WIN32) || defined(_WIN64)
DWORD SystemUsername::parseEvent(EVT_HANDLE hEvent)
{
	DWORD dwBufferUsed = kEventBufferSize*2;
	WCHAR renderedContent[kEventBufferSize];
	char eventDataStr[kEventBufferSize];
	unsigned long long bufferSize(0);

	if (!EvtRender(NULL, hEvent, EvtRenderEventXml, kEventBufferSize*2, renderedContent, &dwBufferUsed, 0))
	{
		DWORD err = GetLastError();
		AppCore::writeToApplicationLogLine("SystemUsername::parseEvent: EvtRender failed with error code " + std::to_string(err));
		return err;
	}
	errno_t convErr = wcstombs_s(&bufferSize, eventDataStr, renderedContent, kEventBufferSize);
	if (convErr != 0)
	{
		AppCore::writeToApplicationLogLine("SystemUsername::parseEvent: wcstombs_s failed with error code " + std::to_string(convErr));
		return static_cast<DWORD>(convErr);
	}

	std::string filteredStr(eventDataStr);

	// The first capture from the regex should contain our username. Generally, this is of the format "SubjectUserName'>biswroy</Data><Data Name='SubjectDomainName"
	// Check if there is actually a captured string. If not, we ignore.
	if (!filteredStr.empty())
	{
		size_t pos1 = filteredStr.find("SubjectUserName'>");
		if (pos1 != std::string::npos)
		{
			size_t start = pos1 + strlen("SubjectUserName'>");
			size_t pos2 = filteredStr.find("</Data>", start);
			if (pos2 != std::string::npos)
			{
				_userName = filteredStr.substr(start, pos2 - start);
			}
		}
	}

	return ERROR_SUCCESS;
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
		if (status != ERROR_NO_MORE_ITEMS)
		{
			AppCore::writeToApplicationLogLine("SystemUsername::fetchLatestEvent: EvtNext failed with error code " + std::to_string(status));
		}
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
