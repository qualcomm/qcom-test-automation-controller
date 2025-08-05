// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "PrintEfEr.h"

QByteArray PrintEfEr::FormatString
(
	const QByteArray& formatString, 
	int oneInt
)
{
	QByteArray entry;
	if (formatString.contains("%s") == true)
	{
		entry = "Error: Attempt to format integer as a string";
	}
	else
	{
		char temp[1024];

		sprintf(temp, formatString.constData(), oneInt);

		entry = QByteArray(temp);
	}

	return entry;
}

QByteArray PrintEfEr::FormatString
(
	const QByteArray& formatString, 
	const char* aString
)
{
	char temp[1024];

	sprintf(temp, formatString.constData(), aString);

	return QByteArray(temp);
}

// Two Paramters
QByteArray PrintEfEr::FormatString
(
	const QByteArray& formatString, 
	int oneInt, 
	int twoInt
)
{
	QByteArray entry;
	if (formatString.contains("%s") == true)
	{
		entry = "Error: Attempt to format integer as a string";
	}
	else
	{
		char temp[1024];

		sprintf(temp, formatString.constData(), oneInt, twoInt);

		entry = QByteArray(temp);
	}

	return entry;
}

QByteArray PrintEfEr::FormatString
(
	const QByteArray& formatString, 
	int oneInt, 
	const char* aString
)
{
	char temp[1024];

	sprintf(temp, formatString.constData(), oneInt, aString);

	return QByteArray(temp);
}

QByteArray PrintEfEr::FormatString
(
	const QByteArray& formatString, 
	const char* aString, 
	int twoInt
)
{
	char temp[1024];

	sprintf(temp, formatString.constData(), aString, twoInt);

	return QByteArray(temp);
}

// Three paramters
QByteArray PrintEfEr::FormatString
(
	const QByteArray& formatString, 
	int oneInt, 
	int twoInt, 
	int threeInt
)
{
	QByteArray entry;

	if (formatString.contains("%s") == true)
	{
		entry = "Error: Attempt to format integer as a string";
	}
	else
	{
		char temp[1024];

		sprintf(temp, formatString.constData(), oneInt, twoInt, threeInt);

		entry = QByteArray(temp);
	}

	return entry;
}

QByteArray PrintEfEr::FormatString
(
	const QByteArray& formatString, 
	const char* aString, 
	int twoInt, 
	int threeInt
)
{
	char temp[1024];

	sprintf(temp, formatString.constData(), aString, twoInt, threeInt);

	return QByteArray(temp);
}

QByteArray PrintEfEr::FormatString
(
	const QByteArray& formatString, 
	int oneInt, 
	const char* aString, 
	int threeInt
)
{
	char temp[1024];

	sprintf(temp, formatString.constData(), oneInt, aString, threeInt);

	return QByteArray(temp);
}

QByteArray PrintEfEr::FormatString
(
	const QByteArray& formatString, 
	int oneInt, 
	int twoInt, 
	const char* aString
)
{
	char temp[1024];

	sprintf(temp, formatString.constData(), oneInt, twoInt, aString);

	return QByteArray(temp);
}
