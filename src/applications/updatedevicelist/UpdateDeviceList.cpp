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
