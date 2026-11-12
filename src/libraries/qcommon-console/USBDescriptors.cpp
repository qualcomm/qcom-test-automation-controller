// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "USBDescriptors.h"

// QCommonConsole
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

#include <cctype>

static std::string toLowerCopy(const std::string& value)
{
	std::string result{value};

	for (char& c: result)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	return result;
}

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
	_modifyDate = QDateTime::currentDateTime().toString().toStdString();
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
	const std::string& usbDescriptor
)
{
	PlatformID result{0};

	std::string targetUsbDescriptor = toLowerCopy(usbDescriptor);

	for (const auto& usbDescriptorEntry: _descriptorList)
	{
		if (toLowerCopy(usbDescriptorEntry._usbDescriptor) == targetUsbDescriptor)
		{
			result = usbDescriptorEntry._platformID;
			break;
		}
	}

	return result;
}

void USBDescriptors::addUSBDescriptor(USBDescriptor usbDescriptor)
{
	_descriptorList.push_back(usbDescriptor);
}

bool USBDescriptors::load
(
	const std::string& filePath
)
{
	bool result(false);

	QJsonDocument document;
	QFile jsonFile(QString::fromStdString(filePath));

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
					_lastError = QObject::tr("JSON Document is NULL").toStdString();
				}
			}
			else
			{
				_lastError = (QObject::tr("Error parsing configuration file") + parserError.errorString()).toStdString();
			}
		}
	}
	else
	{
		_lastError = QObject::tr("Unable to open platform configuration file ").toStdString() + filePath;
	}

	return result;
}

bool USBDescriptors::save
(
	const std::string &filePath
)
{
	bool result{false};

	QFile targetFile(QString::fromStdString(filePath));
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
				QFile jsonFile(QString::fromStdString(filePath));

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

std::string USBDescriptors::getLastError()
{
	return _lastError;
}

bool USBDescriptors::read(QJsonObject& parentLevel)
{
	bool result{false};
	QJsonValue jsonValue;

	jsonValue = parentLevel.value(kModifyDate);
	if (jsonValue.isNull() == false)
		_modifyDate = jsonValue.toString().toStdString();

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
						usbDescriptor._usbDescriptor = catalogData[kUSBDescriptor].toString().toStdString();

					if (catalogData.contains(kName) && catalogData[kName].isString())
						usbDescriptor._name = catalogData[kName].toString().toStdString();

					if (catalogData.contains(kDescription) && catalogData[kDescription].isString())
						usbDescriptor._description = catalogData[kDescription].toString().toStdString();

					if (catalogData.contains(kRevisionNumber))
						usbDescriptor._revision = static_cast<uint32_t>(catalogData[kRevisionNumber].toInt());

					if (catalogData.contains(kConfigFilePath) && catalogData[kConfigFilePath].isString())
						usbDescriptor._configurationFilePath = catalogData[kConfigFilePath].toString().toStdString();

#ifdef Q_OS_LINUX
					usbDescriptor._configurationFilePath = expandPath(usbDescriptor._configurationFilePath);
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
	parentLevel[kModifyDate] = QString::fromStdString(_modifyDate);

	QJsonArray catalog;

	for (const auto& usbDescriptor : std::as_const(_descriptorList))
	{
		QJsonObject usbDescriptorObject;

		usbDescriptorObject[kPlatformId] = static_cast<int>(usbDescriptor._platformID);
		usbDescriptorObject[kDebugBoardType] = static_cast<int>(usbDescriptor._debugBoardType);
		usbDescriptorObject[kUSBDescriptor] = QString::fromStdString(usbDescriptor._usbDescriptor);
		usbDescriptorObject[kName] = QString::fromStdString(usbDescriptor._name);
		usbDescriptorObject[kDescription] = QString::fromStdString(usbDescriptor._description);
		usbDescriptorObject[kRevisionNumber] = static_cast<int>(usbDescriptor._revision);
		usbDescriptorObject[kConfigFilePath] = QString::fromStdString(usbDescriptor._configurationFilePath);

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

