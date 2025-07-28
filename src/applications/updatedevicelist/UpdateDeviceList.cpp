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
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "UpdateDeviceList.h"

// QCommon
#include "ConsoleApplicationEnhancements.h"
#include "FTDIPlatformConfiguration.h"
#include "PlatformConfiguration.h"
#include "USBDescriptors.h"

// Qt
#include <QDir>

// C++
#include <iostream>

#ifdef Q_OS_WIN
const QString kServerConfigDir(QStringLiteral("C:\\github\\open-source\\qcom-test-automation-controller\\configurations"));
#endif
#ifdef Q_OS_LINUX
	const QString kServerConfigDir = expandPath("/local/mnt/workspace/github/open-source/qcom-test-automation-controller/configurations");
#endif

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
	QMap<PlatformID, USBDescriptor> usbDescriptorMap;

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
			std::cout << "   Found Platform ID: " << usbDescriptor._name.toLatin1().data() << std::endl;
	}

	QDir configurationDir(filePath);

	QStringList nameFilters;
	nameFilters << "*.tcnf";

	if (_verbose)
		std::cout << std::endl << std::endl << "Configuration Files at path: " << filePath.toLatin1().constData() << std::endl;

	QFileInfoList entryInfoList = configurationDir.entryInfoList(nameFilters, QDir::Files, QDir::Name);
	for (const auto& entryInfo: entryInfoList)
	{
		PlatformConfiguration platformConfiguration = _PlatformConfiguration::openPlatformConfiguration(entryInfo.filePath());

		if (_verbose)
			std::cout << "   Entry: " << entryInfo.filePath().toLatin1().constData() << std::endl;

		if (platformConfiguration.isNull() == false)
		{
			USBDescriptor usbDescriptor;

			const QString kUSBDescriptorPath = tacConfigRoot(false);

			usbDescriptor._platformID = platformConfiguration->getPlatformId();
			usbDescriptor._debugBoardType = platformConfiguration->getPlatform();
			usbDescriptor._name = platformConfiguration->name();
			usbDescriptor._description = platformConfiguration->description();
			usbDescriptor._revision = platformConfiguration->fileVersion();
			usbDescriptor._configurationFilePath = QDir::cleanPath(QString(kUSBDescriptorPath + entryInfo.fileName())).toLatin1();
			usbDescriptor._usbDescriptor = platformConfiguration->getUSBDescriptor();

			if (usbDescriptor._debugBoardType == eFTDI)
			{
				_FTDIPlatformConfiguration* fpc = static_cast<_FTDIPlatformConfiguration*>(platformConfiguration.data());

				for (int i = 0; i < 4; i++)
				{
					usbDescriptor._pinSets[i] = fpc->getPinSet(i);
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

	for (const auto& usbDescriptor: std::as_const(usbDescriptorMap))
	{
		usbDescriptors.addUSBDescriptor(usbDescriptor);
	}

	QString savePath = QDir::cleanPath(filePath + QDir::separator() + kUSBDescriptorFileName);
	if (usbDescriptors.save(savePath) == true)
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
		configFilePath = QDir::cleanPath(kServerConfigDir);

		if (QDir(configFilePath).exists())
		{
			if (_verbose)
				std::cout << "Processing path " << configFilePath.toLatin1().data() << std::endl;

			save(configFilePath);
		}
		else
		{
			// don't print this one
			// std::cout << configFilePath.toLatin1().data() << " does not exist " << std::endl;
		}

		configFilePath = QDir::cleanPath(tacConfigRoot());
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
