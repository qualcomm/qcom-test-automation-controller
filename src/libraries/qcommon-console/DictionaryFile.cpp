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

// Author: msimpson, biswroy

#include "DictionaryFile.h"

// Qt
#include <QDataStream>
#include <QFile>

#include <PrintEfEr.h>

_DictionaryFile::_DictionaryFile()
{
}


_DictionaryFile::~_DictionaryFile()
{
}

bool _DictionaryFile::Open
(
	const QString& filePath // Dictionary File Path
)
{
	bool result(false);

	QFile dictionaryFile(filePath);

	if (dictionaryFile.open(QIODevice::ReadOnly) == true)
	{
		QDataStream dataStream(&dictionaryFile);

		quint8 nextChar;
		QByteArray line;

		while (dataStream.atEnd() == false)
		{
			dataStream >> nextChar;

			if (nextChar == (char) 0x01)
			{
				QList<QByteArray> parts = line.split(',');
				bool okay;
				int messageIndex = parts[0].toInt(&okay);
				if (okay == true)
				{
					QString displayLine;

					for (int i = 2; i < parts.count(); i++)
					{
						if (i != 2)
							displayLine += ",";
						displayLine += QString(parts[i]);
					}

					if (displayLine.isEmpty())
						displayLine = "<empty>";

					displayLine.remove("\r");
					displayLine.remove("\n");

					_strings[messageIndex] = displayLine.toLatin1();
				}

				line.clear();
			}
			else
			{
				line += nextChar;
			}
		}

		result = true;
	}

	return result;
}

QByteArray _DictionaryFile::GetEntry(int index)
{
	QMap<int, QByteArray>::iterator stringEntry = _strings.find(index);
	if (stringEntry != _strings.end())
		return stringEntry.value();

	return "";
}

QByteArray _DictionaryFile::FormatEntry
(
	int index
)
{
	return GetEntry(index);
}

QByteArray _DictionaryFile::FormatEntry
(
	int index, 
	int oneInt
)
{
	return PrintEfEr::FormatString(GetEntry(index), oneInt);
}

QByteArray _DictionaryFile::FormatEntry
(
	int index, 
	const char* aString
)
{
	return PrintEfEr::FormatString(GetEntry(index), aString);
}

QByteArray _DictionaryFile::FormatEntry
(
	int index, 
	int oneInt, 
	int twoInt
)
{
	return PrintEfEr::FormatString(GetEntry(index), oneInt, twoInt);
}

QByteArray _DictionaryFile::FormatEntry
(
	int index, 
	int oneInt, 
	const char* aString
)
{
	return PrintEfEr::FormatString(GetEntry(index), oneInt, aString);
}
	
QByteArray _DictionaryFile::FormatEntry
(
	int index, 
	const char* aString, 
	int twoInt
)
{
	return PrintEfEr::FormatString(GetEntry(index), aString, twoInt);
}

QByteArray _DictionaryFile::FormatEntry
(
	int index, 
	int oneInt, 
	int twoInt, 
	int threeInt
)
{
	return PrintEfEr::FormatString(GetEntry(index), oneInt, twoInt, threeInt);
}
	
QByteArray _DictionaryFile::FormatEntry
(
	int index, 
	const char* aString, 
	int twoInt, 
	int threeInt
)
{
	return PrintEfEr::FormatString(GetEntry(index), aString, twoInt, threeInt);
}

QByteArray _DictionaryFile::FormatEntry
(
	int index, 
	int oneInt, 
	const char* aString, 
	int threeInt
)
{
	return PrintEfEr::FormatString(GetEntry(index), oneInt, aString, threeInt);
}

QByteArray _DictionaryFile::FormatEntry
(
	int index, 
	int oneInt, 
	int twoInt, 
	const char* aString
)
{
	return PrintEfEr::FormatString(GetEntry(index), oneInt, twoInt, aString);
}
