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

#include "USBDescriptors.h"

// qcommon-console
#include "ConsoleApplicationEnhancements.h"
#include "Range.h"

// QT
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>

// Parameters in the JSON TAC Configuration
const QString kModifyDate(QStringLiteral("modification_date"));
const QString kUSBCatalog(QStringLiteral("catalog"));
const QString kDebugBoardType(QStringLiteral("debugBoardType"));
const QString kPlatformId(QStringLiteral("platform_id"));
const QString kName(QStringLiteral("name"));
const QString kDescription(QStringLiteral("description"));
const QString kUSBDescriptor(QStringLiteral("usb_descriptor"));
const QString kRevisionNumber(QStringLiteral("revision"));
const QString kConfigFilePath(QStringLiteral("configPath"));
const QString kChip1BusSet(QStringLiteral("chip1BusSet"));
const QString kChip2BusSet(QStringLiteral("chip2BusSet"));
const QString kChip3BusSet(QStringLiteral("chip3BusSet"));
const QString kChip4BusSet(QStringLiteral("chip4BusSet"));

USBDescriptors::USBDescriptors()
{
	_modifyDate = QDateTime::currentDateTime().toString();
}

DescriptorList USBDescriptors::getDescriptors()
{
	return _descriptorList;
}

DescriptorList USBDescriptors::getDescriptorsByBoardType
(
	DebugBoardType debugBoardType
)
{
	DescriptorList result;

	for (const auto& descriptor: std::as_const(_descriptorList))
	{
		if (descriptor._debugBoardType == debugBoardType)
			result.push_back(descriptor);
	}

	return result;
}

USBDescriptor USBDescriptors::getPlatformDescriptor
(
	PlatformID platformID
)
{
	USBDescriptor result;

	for (const auto& usbDescriptor: std::as_const(_descriptorList))
	{
		if (usbDescriptor._platformID == platformID)
		{
			result = usbDescriptor;
			break;
		}
	}

	return result;
}

PlatformID USBDescriptors::getPlatformId
(
	const QByteArray& usbDescriptor
)
{
	PlatformID result{0};

	for (const auto& usbDescriptorEntry: std::as_const(_descriptorList))
	{
		if (usbDescriptorEntry._usbDescriptor.compare(usbDescriptor, Qt::CaseInsensitive) == 0)
		{
			result = usbDescriptorEntry._platformID;
			break;
		}
	}

	return result;
}

void USBDescriptors::addUSBDescriptor(USBDescriptor usbDescriptor)
{
	_descriptorList.append(usbDescriptor);
}

bool USBDescriptors::load
(
	const QString& filePath
)
{
	bool result(false);

	QJsonDocument document;
	QFile jsonFile(filePath);

	if (jsonFile.open(QIODevice::ReadOnly))
	{
		QByteArray jsonFileContents = jsonFile.readAll();
		if (jsonFileContents.isEmpty() == false)
		{
			QJsonParseError parserError;

			document = QJsonDocument::fromJson(jsonFileContents, &parserError);
			if (parserError.error == QJsonParseError::NoError)
			{
				if (document.isNull() == false)
				{
					result = true;

					QJsonObject rootLevel = document.object();
					read(rootLevel);
				}
				else
				{
					_lastError = QObject::tr("JSON Document is NULL") ;
				}
			}
			else
			{
				_lastError = QObject::tr("Error parsing configuration file") + parserError.errorString();
			}
		}
	}
	else
	{
		_lastError = QObject::tr("Unable to open platform configuration file ") + filePath;
	}

	return result;
}

bool USBDescriptors::save
(
	const QString &filePath
)
{
	bool result{false};

	QFile targetFile(filePath);
	if (targetFile.open(QIODevice::WriteOnly) == true)
	{
		QJsonObject rootLevel;
		QJsonDocument document;

		write(rootLevel);

		document = QJsonDocument(rootLevel);

		if (document.isNull() == false)
		{
			QByteArray jsonDocument = document.toJson(QJsonDocument::Indented);
			if (jsonDocument.isEmpty() == false)
			{
				QFile jsonFile(filePath);

				if (jsonFile.open(QIODevice::WriteOnly) == true)
				{
					jsonFile.write(jsonDocument);
					jsonFile.close();

					result = true;
				}
			}
		}
	}

	return result;
}

QString USBDescriptors::getLastError()
{
	return _lastError;
}

bool USBDescriptors::read(QJsonObject& parentLevel)
{
	bool result{false};
	QJsonValue jsonValue;

	jsonValue = parentLevel.value(kModifyDate);
	if (jsonValue.isNull() == false)
		_modifyDate = jsonValue.toString();

	QJsonArray usbCatalog = parentLevel.value(kUSBCatalog).toArray();
	if (usbCatalog.count() > 0)
	{
		_descriptorList.clear();

		for (auto catalogIndex: range(usbCatalog.count()))
		{
			USBDescriptor usbDescriptor;

			QJsonObject catalogData = usbCatalog.at(catalogIndex).toObject();
			if (catalogData.isEmpty() == false)
			{

				if (catalogData.contains(kPlatformId))
				{
					usbDescriptor._platformID = catalogData[kPlatformId].toInt();

					if (catalogData.contains(kDebugBoardType))
						usbDescriptor._debugBoardType = static_cast<DebugBoardType>(catalogData[kDebugBoardType].toInt());

					if (catalogData.contains(kUSBDescriptor) && catalogData[kUSBDescriptor].isString())
						usbDescriptor._usbDescriptor = catalogData[kUSBDescriptor].toString().toLatin1();

					if (catalogData.contains(kName) && catalogData[kName].isString())
						usbDescriptor._name = catalogData[kName].toString();

					if (catalogData.contains(kDescription) && catalogData[kDescription].isString())
						usbDescriptor._description = catalogData[kDescription].toString();

					if (catalogData.contains(kRevisionNumber))
						usbDescriptor._revision = static_cast<quint32>(catalogData[kRevisionNumber].toInt());

					if (catalogData.contains(kConfigFilePath) && catalogData[kConfigFilePath].isString())
						usbDescriptor._configurationFilePath = catalogData[kConfigFilePath].toString().toLatin1();

#ifdef Q_OS_LINUX
					usbDescriptor._configurationFilePath = expandPath(QString(usbDescriptor._configurationFilePath)).toLatin1();
#endif

					if (usbDescriptor._debugBoardType == eFTDI)
					{
						if (catalogData.contains(kChip1BusSet))
							usbDescriptor._pinSets[0] = static_cast<FTDIPinSet>(catalogData[kChip1BusSet].toInt());

						if (catalogData.contains(kChip2BusSet))
							usbDescriptor._pinSets[1] = static_cast<FTDIPinSet>(catalogData[kChip2BusSet].toInt());

						if (catalogData.contains(kChip3BusSet))
							usbDescriptor._pinSets[2] = static_cast<FTDIPinSet>(catalogData[kChip3BusSet].toInt());

						if (catalogData.contains(kChip4BusSet))
							usbDescriptor._pinSets[3] = static_cast<FTDIPinSet>(catalogData[kChip4BusSet].toInt());
					}

					_descriptorList.push_back(usbDescriptor);
				}
			}
		}
	}

	return result;
}

void USBDescriptors::write(QJsonObject& parentLevel)
{
	parentLevel[kModifyDate] = _modifyDate;

	QJsonArray catalog;

	for (const auto& usbDescriptor : std::as_const(_descriptorList))
	{
		QJsonObject usbDescriptorObject;

		usbDescriptorObject[kPlatformId] = static_cast<int>(usbDescriptor._platformID);
		usbDescriptorObject[kDebugBoardType] = static_cast<int>(usbDescriptor._debugBoardType);
		usbDescriptorObject[kUSBDescriptor] = QString(usbDescriptor._usbDescriptor);
		usbDescriptorObject[kName] = usbDescriptor._name;
		usbDescriptorObject[kDescription] = usbDescriptor._description;
		usbDescriptorObject[kRevisionNumber] = static_cast<int>(usbDescriptor._revision);
		usbDescriptorObject[kConfigFilePath] = QString(usbDescriptor._configurationFilePath);

		if (usbDescriptor._debugBoardType == eFTDI)
		{
			usbDescriptorObject[kChip1BusSet] = static_cast<int>(usbDescriptor._pinSets[0]);
			usbDescriptorObject[kChip2BusSet] = static_cast<int>(usbDescriptor._pinSets[1]);
			usbDescriptorObject[kChip3BusSet] = static_cast<int>(usbDescriptor._pinSets[2]);
			usbDescriptorObject[kChip4BusSet] = static_cast<int>(usbDescriptor._pinSets[3]);
		}

		catalog.append(usbDescriptorObject);
	}

	parentLevel.insert(kUSBCatalog, catalog);
}

