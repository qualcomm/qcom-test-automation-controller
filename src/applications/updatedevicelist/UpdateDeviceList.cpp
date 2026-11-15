// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "UpdateDeviceList.h"

// QCommon
#include "ConsoleApplicationEnhancements.h"
#include "FTDIPlatformConfiguration.h"
#include "PlatformConfiguration.h"
#include "USBDescriptors.h"
#include "QCommonTypes.h"

// Qt
#include <QDir>

// C++
#include <iostream>
#include <map>

UpdateDeviceList::UpdateDeviceList()
{

}

void UpdateDeviceList::setDeviceListDir(const QString &fileDir)
{
	_fileDir = fileDir;
}

void UpdateDeviceList::setVerbosity(bool verbose)
{
	_verbose = verbose;
}

void UpdateDeviceList::save(const QString &filePath)
{
	std::map<PlatformID, USBDescriptor> usbDescriptorMap;

	if (_verbose)
		std::cout << std::endl << std::endl << "Platform ID: " << std::endl;

	PlatformIDList platformIDList = PlatformContainer::getDebugBoards();
	for (auto& platformID: platformIDList)
	{
		USBDescriptor usbDescriptor;

		usbDescriptor._platformID = platformID->_platformID;
		usbDescriptor._debugBoardType = platformID->_boardtype;
		usbDescriptor._name = platformID->_description;
		usbDescriptor._description = platformID->_description;
		for (int i{0}; i < kMaxPinSetCount; i++)
			usbDescriptor._pinSets[0] = platformID->_pinSets[0];

		usbDescriptorMap[usbDescriptor._platformID] = usbDescriptor;

		if (_verbose)
			std::cout << "   Found Platform ID: " << usbDescriptor._name << std::endl;
	}

	QDir configurationDir(filePath);

	QStringList nameFilters;
	nameFilters << "*.tcnf";

	if (_verbose)
		std::cout << std::endl << std::endl << "Configuration Files at path: " << filePath.toLatin1().constData() << std::endl;

	QFileInfoList entryInfoList = configurationDir.entryInfoList(nameFilters, QDir::Files, QDir::Name);
	for (const auto& entryInfo: entryInfoList)
	{
		PlatformConfiguration platformConfiguration = _PlatformConfiguration::openPlatformConfiguration(entryInfo.filePath().toStdString());

		if (_verbose)
			std::cout << "   Entry: " << entryInfo.filePath().toLatin1().constData() << std::endl;

		if (platformConfiguration != nullptr)
		{
			USBDescriptor usbDescriptor;

			const std::string kUSBDescriptorPath = _fileDir.isEmpty() ? tacConfigRoot() : (QDir::cleanPath(filePath) + QDir::separator()).toStdString();

			usbDescriptor._platformID = platformConfiguration->getPlatformId();
			usbDescriptor._debugBoardType = platformConfiguration->getPlatform();
			usbDescriptor._name = platformConfiguration->name();
			usbDescriptor._description = platformConfiguration->description();
			usbDescriptor._revision = platformConfiguration->fileVersion();
			usbDescriptor._configurationFilePath = kUSBDescriptorPath + entryInfo.fileName().toStdString();
			usbDescriptor._usbDescriptor = platformConfiguration->getUSBDescriptor();

			if (usbDescriptor._debugBoardType == eFTDI)
			{
				_FTDIPlatformConfiguration* fpc = dynamic_cast<_FTDIPlatformConfiguration*>(platformConfiguration.get());
				if (fpc != nullptr)
				{
					for (int i = 0; i < 4; i++)
					{
						usbDescriptor._pinSets[i] = fpc->getPinSet(i);
					}
				}
			}

			usbDescriptorMap[usbDescriptor._platformID] = usbDescriptor;

			if (_verbose)
				std::cout << "   Entry: " << entryInfo.filePath().toLatin1().constData() << " processed" << std::endl;
		}
		else
		{
			if (_verbose)
				std::cout << "   Entry: " << entryInfo.filePath().toLatin1().constData() << " failed" << std::endl;
		}
	}

	USBDescriptors usbDescriptors;

	for (const auto& [id, usbDescriptor]: usbDescriptorMap)
	{
		usbDescriptors.addUSBDescriptor(usbDescriptor);
	}

	QString savePath = QDir::cleanPath(filePath + QDir::separator() + toQString(kUSBDescriptorFileName));
	if (usbDescriptors.save(savePath.toStdString()) == true)
	{
		if (_verbose)
			std::cout<< std::endl << "   Entry: " << savePath.toLatin1().constData() << " saved" << std::endl;
	}
	else
	{
		std::cout<< std::endl << "   Entry: " << savePath.toLatin1().constData() << " save failed. Try with administrator privilege" << std::endl;
	}
}

void UpdateDeviceList::write()
{
	QString configFilePath;

	if (_fileDir.isEmpty() == false)
	{
		configFilePath = _fileDir;

		if (QDir(configFilePath).exists())
		{
			if (_verbose)
				std::cout << "Processing path " << configFilePath.toLatin1().data() << std::endl;

			save(configFilePath);
		}
		else
		{
			std::cout << configFilePath.toLatin1().data() << " does not exist " << std::endl;
		}
	}
	else
	{
		configFilePath = QDir::cleanPath(toQString(tacConfigRoot()));
		if (QDir(configFilePath).exists())
		{
			if (_verbose)
				std::cout << "Processing path " << configFilePath.toLatin1().data() << std::endl;

			save(configFilePath);
		}
		else
		{
			std::cout << configFilePath.toLatin1().data() << " does not exist " << std::endl;
		}
	}
}
