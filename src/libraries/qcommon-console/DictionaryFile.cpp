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
