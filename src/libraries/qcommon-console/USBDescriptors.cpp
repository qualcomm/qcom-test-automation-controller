// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "USBDescriptors.h"

// QCommonConsole
#include "ConsoleApplicationEnhancements.h"
#include "Range.h"
#include "StringUtilities.h"

#include <cctype>

static std::string toLowerCopy(const std::string& value)
{
	std::string result{value};

	for (char& c: result)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	return result;
}

// Parameters in the JSON TAC Configuration
const std::string kModifyDate("modification_date");
const std::string kUSBCatalog("catalog");
const std::string kDebugBoardType("debugBoardType");
const std::string kPlatformId("platform_id");
const std::string kName("name");
const std::string kDescription("description");
const std::string kUSBDescriptor("usb_descriptor");
const std::string kRevisionNumber("revision");
const std::string kConfigFilePath("configPath");
const std::string kChip1BusSet("chip1BusSet");
const std::string kChip2BusSet("chip2BusSet");
const std::string kChip3BusSet("chip3BusSet");
const std::string kChip4BusSet("chip4BusSet");

USBDescriptors::USBDescriptors()
{
	_modifyDate = currentDateTimeString();
}

std::vector<PlatformID> USBDescriptors::getPlatforms()
{
	std::vector<PlatformID> result;

	for (const auto& entry: _descriptorList)
	{
		result.push_back(entry._platformID);
	}

	return result;
}

DescriptorList USBDescriptors::getDescriptors()
{
	return _descriptorList;
}

DescriptorList USBDescriptors::getDescriptorsByBoardType(DebugBoardType debugBoardType)
{
	DescriptorList result;

	for (const auto& entry: _descriptorList)
	{
		if (entry._debugBoardType == debugBoardType)
			result.push_back(entry);
	}

	return result;
}

USBDescriptor USBDescriptors::getPlatformDescriptor(PlatformID platformID)
{
	USBDescriptor result;

	for (const auto& entry: _descriptorList)
	{
		if (entry._platformID == platformID)
		{
			result = entry;
			break;
		}
	}

	return result;
}

PlatformID USBDescriptors::getPlatformId(const std::string& usbDescriptor)
{
	PlatformID result{0};

	std::string lowerUsbDescriptor = toLowerCopy(usbDescriptor);

	for (const auto& entry: _descriptorList)
	{
		if (lowerUsbDescriptor.find(toLowerCopy(entry._usbDescriptor)) != std::string::npos)
		{
			result = entry._platformID;
			break;
		}
	}

	return result;
}

void USBDescriptors::addUSBDescriptor(USBDescriptor usbDescriptor)
{
	_descriptorList.push_back(usbDescriptor);
}

std::string USBDescriptors::getLastError()
{
	return _lastError;
}

bool USBDescriptors::load(const std::string& filePath)
{
	ConfigurationFile config;
	if (!config.load(filePath))
	{
		_lastError = "Unable to open platform configuration file " + filePath;
		return false;
	}

	readConfig(config);
	return true;
}

bool USBDescriptors::save(const std::string &filePath)
{
	ConfigurationFile config;
	writeConfig(config);
	return config.save(filePath, true);
}

bool USBDescriptors::readConfig(const ConfigurationFile& config)
{
	bool result{false};

	if (config.hasProperty(kModifyDate))
		_modifyDate = config.getProperty(kModifyDate);

	const ConfigurationTable& usbCatalog = config.getTable(kUSBCatalog);
	if (!usbCatalog.empty())
	{
		_descriptorList.clear();

		for (const auto& catalogData : usbCatalog)
		{
			USBDescriptor usbDescriptor;

			if (catalogData.find(kPlatformId) != catalogData.end())
			{
				usbDescriptor._platformID = ConfigurationFile::getRecordInt(catalogData, kPlatformId);

				if (catalogData.find(kDebugBoardType) != catalogData.end())
					usbDescriptor._debugBoardType = static_cast<DebugBoardType>(ConfigurationFile::getRecordInt(catalogData, kDebugBoardType));

				if (catalogData.find(kUSBDescriptor) != catalogData.end())
					usbDescriptor._usbDescriptor = ConfigurationFile::getRecordString(catalogData, kUSBDescriptor);

				if (catalogData.find(kName) != catalogData.end())
					usbDescriptor._name = ConfigurationFile::getRecordString(catalogData, kName);

				if (catalogData.find(kDescription) != catalogData.end())
					usbDescriptor._description = ConfigurationFile::getRecordString(catalogData, kDescription);

				if (catalogData.find(kRevisionNumber) != catalogData.end())
					usbDescriptor._revision = ConfigurationFile::getRecordUInt(catalogData, kRevisionNumber);

				if (catalogData.find(kConfigFilePath) != catalogData.end())
					usbDescriptor._configurationFilePath = ConfigurationFile::getRecordString(catalogData, kConfigFilePath);

#if defined(__linux__) || defined(Q_OS_LINUX)
				usbDescriptor._configurationFilePath = expandPath(usbDescriptor._configurationFilePath);
#endif

				if (usbDescriptor._debugBoardType == eFTDI)
				{
					if (catalogData.find(kChip1BusSet) != catalogData.end())
						usbDescriptor._pinSets[0] = static_cast<FTDIPinSet>(ConfigurationFile::getRecordInt(catalogData, kChip1BusSet));

					if (catalogData.find(kChip2BusSet) != catalogData.end())
						usbDescriptor._pinSets[1] = static_cast<FTDIPinSet>(ConfigurationFile::getRecordInt(catalogData, kChip2BusSet));

					if (catalogData.find(kChip3BusSet) != catalogData.end())
						usbDescriptor._pinSets[2] = static_cast<FTDIPinSet>(ConfigurationFile::getRecordInt(catalogData, kChip3BusSet));

					if (catalogData.find(kChip4BusSet) != catalogData.end())
						usbDescriptor._pinSets[3] = static_cast<FTDIPinSet>(ConfigurationFile::getRecordInt(catalogData, kChip4BusSet));
				}

				_descriptorList.push_back(usbDescriptor);
			}
		}
		result = true;
	}

	return result;
}

void USBDescriptors::writeConfig(ConfigurationFile& config)
{
	config.setProperty(kModifyDate, _modifyDate);

	ConfigurationTable catalog;

	for (const auto& usbDescriptor : _descriptorList)
	{
		ConfigurationRecord usbDescriptorObject;

		usbDescriptorObject[kPlatformId] = std::to_string(static_cast<int>(usbDescriptor._platformID));
		usbDescriptorObject[kDebugBoardType] = std::to_string(static_cast<int>(usbDescriptor._debugBoardType));
		usbDescriptorObject[kUSBDescriptor] = usbDescriptor._usbDescriptor;
		usbDescriptorObject[kName] = usbDescriptor._name;
		usbDescriptorObject[kDescription] = usbDescriptor._description;
		usbDescriptorObject[kRevisionNumber] = std::to_string(static_cast<int>(usbDescriptor._revision));
		usbDescriptorObject[kConfigFilePath] = usbDescriptor._configurationFilePath;

		if (usbDescriptor._debugBoardType == eFTDI)
		{
			usbDescriptorObject[kChip1BusSet] = std::to_string(static_cast<int>(usbDescriptor._pinSets[0]));
			usbDescriptorObject[kChip2BusSet] = std::to_string(static_cast<int>(usbDescriptor._pinSets[1]));
			usbDescriptorObject[kChip3BusSet] = std::to_string(static_cast<int>(usbDescriptor._pinSets[2]));
			usbDescriptorObject[kChip4BusSet] = std::to_string(static_cast<int>(usbDescriptor._pinSets[3]));
		}

		catalog.push_back(usbDescriptorObject);
	}

	config.setTable(kUSBCatalog, catalog);
}
