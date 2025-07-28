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
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
