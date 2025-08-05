// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QShrinkFile.h"

// QCommon
#include "QShrinkDB.h"

// QT
#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include <QLibraryInfo>

// C++
#include <iostream>

using namespace std;


QShrinkFile::~QShrinkFile()
{
	Close();
}

bool QShrinkFile::IsDBFileUpToDate
(
	const QString& stringFile,
	const QString& databaseFile
)
{
	bool result(false);
	
	if (QFile::exists(databaseFile) == true)
	{
		QFileInfo sourceInfo(stringFile);
		quint64 fileSize = sourceInfo.size();

		QFileInfo destInfo(databaseFile);
		QString dictionaryPath = destInfo.absolutePath();

		QFile databaseDescFile(dictionaryPath + QDir::separator() + sourceInfo.baseName() + ".bin");
		if (databaseDescFile.open(QIODevice::ReadOnly) == true)
		{
			QDataStream dataStream(&databaseDescFile);
			QString descPath;
			quint64 descSize;

			dataStream >> descPath;
			dataStream >> descSize;

			databaseDescFile.close();

			if (descPath == stringFile && descSize == fileSize)
				result = true;
		}
	}

	return result;
}

bool QShrinkFile::ImportStringFile
(
	const QString& stringFile,
	const QString& databaseFile
)
{
	bool result(false);

	QString libPath = QLibraryInfo::location(QLibraryInfo::PluginsPath);

	libPath += QDir::separator() + QString("sqldrivers\\qsqlite.dll");

	if (QFile::exists(libPath) == false)
	{
		cout << "Missing DB driver: " << libPath.toLatin1().data() << endl;
		return false;
	}

	QFileInfo sourceInfo(stringFile);	
	quint64 fileSize = sourceInfo.size();

	if (sourceInfo.exists() == true)
	{
		if (QFile::exists(databaseFile) == true)
			QFile::remove(databaseFile);

		QFile sourceFile(stringFile);

		if (sourceFile.open(QIODevice::ReadOnly) == true)
		{
			QTextStream stringStream(&sourceFile);

			QShrinkFile qShrinkFile;

			if (qShrinkFile.Open(databaseFile) == true)
			{
				while (stringStream.atEnd() == false)
				{
					QString line = stringStream.readLine();

					line = line.trimmed();
					if (line.isEmpty() == false && line.startsWith("#") == false)
					{
						QStringList parts = line.split(":", QString::KeepEmptyParts);
						if (parts.count() >= 2)
						{
							bool okay;

							quint32 key = parts.at(0).toUInt(&okay);
							if (okay == true)
							{
								parts.removeAt(0);

								if (parts.empty() == false)
									qShrinkFile.AddEntry(key, parts.size() == 1 ? parts.at(0) : parts.join(":"));
							}
						}
					}
				}

				qShrinkFile.Close();

				result = true;
			}
			else	
				cout << QString("File: %1 not opened.").arg(databaseFile).toLatin1().data() << endl;
		}
		else	
			cout << QString("File: %1 not opened.").arg(stringFile).toLatin1().data() << endl;
	}
	else
		cout << QString("File: %1 not found.").arg(sourceInfo.absoluteFilePath()).toLatin1().data() << endl;

	return result;
}

bool QShrinkFile::AddEntry
(
	quint64 key, 
	const QString& value
)
{
	bool result(false);

	MessageLookupTable* messageLookupTable = _qShrinkDatabase->GetMessageLookupTable();
	if (messageLookupTable != nullptr)
	{
		MessageLookupRecord messageLookupRecord;

		messageLookupRecord.setHash(key);
		messageLookupRecord.setStringField(value);

		result = messageLookupTable->Add(messageLookupRecord);
	}

	return result;
}

bool QShrinkFile::Open
(
	const QString& filePath
)
{
	bool result(false);

	if (_qShrinkDatabase == nullptr)
	{
		_qShrinkDatabase = new QShrinkDatabase();
		if (_qShrinkDatabase != nullptr)
		{
			_qShrinkDatabase->SetFilePath(filePath);
			_qShrinkDatabase->CreateConnection("ShrinkFile");

			QSqlDatabase db = _qShrinkDatabase->Database();
			bool temp = db.isDriverAvailable("QSQLITE");

			result = _qShrinkDatabase->Open();
		}
	}

	return result;
}

void QShrinkFile::Close()
{
	if (_qShrinkDatabase != nullptr)
	{
		_qShrinkDatabase->Close();
		delete _qShrinkDatabase;
		_qShrinkDatabase = nullptr;
	}
}

QString QShrinkFile::GetDebugString
(
	quint64 stringKey
)
{
	QString result;

	if (_qShrinkDatabase != nullptr)
	{
		QMap<quint64, QString>::iterator debugStringEntry = _debugStringCache.find(stringKey);

		if (debugStringEntry == _debugStringCache.end())
		{
			MessageLookupTable* messageLookupTable = _qShrinkDatabase->GetMessageLookupTable();
			if (messageLookupTable != nullptr)
			{
				MessageLookupRecord record;

				if (messageLookupTable->Get(stringKey, record) == true)
					result = record.StringField();
					
				_debugStringCache[stringKey] = result;
			}
		}
		else
			result = debugStringEntry.value();
	}

	return result;
}

