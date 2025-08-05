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
