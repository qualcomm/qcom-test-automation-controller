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
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/


// EepromCmd
#include "PSOCProgrammerCmdLine.h"

// epmLib
#include "EPMDevice.h"

// QCommon
#include "AppCore.h"
#include "QCommonConsole.h"
#include "PlatformID.h"
#include "Range.h"

// Qt
#include <QCoreApplication>
#include <QDateTime>

// C++
#include <iostream>

using namespace std;

PlatformIDs gPsocIDS;

const QByteArray kAppName{"PSOCProgrammer"};
const QByteArray kAppVersion{"1.2.0"};


void listDevices()
{
	auto deviceCount = _EPMDevice::deviceCount();

	if (deviceCount > 0)
	{
		cout << "Devices" << endl;
		for (auto index : range(deviceCount))
		{
			auto epmDevice = _EPMDevice::device(index);

			cout << "   Device:" << epmDevice->getTargetName().toLatin1().data() <<
				 " GUID:" << epmDevice->getUUID().toLatin1().data() <<
				 " Platform ID:" << epmDevice->getPlatformIDString().toLatin1().data() << endl;
		}
	}
	else
	{
		cout << "No devices found." << endl;
	}

	cout << endl;
}

void listPlatforms()
{
	PlatformContainer::initialize();
	PlatformIDList platformIDs = PlatformContainer::getDebugBoardsOfType(ePSOC);

	cout << "Platforms" << endl;
	for (const auto& platformID: platformIDs)
	{
		cout << "   Platform: " << platformID->_description.toLatin1().data() << " ID: " << QString::number(platformID->_platformID).toLatin1().data() << endl;
	}

	cout << endl;
}

void getProgramming()
{
	EPMDevices epmDevices;

	quint32 deviceCount =_EPMDevice::getEPMDevices(epmDevices);
	if (deviceCount == 1)
	{
		EPMDevice epmDevice = epmDevices.at(0);

		if (epmDevice.isNull() == false)
		{
			try
			{
				epmDevice->open();

				if (epmDevice->programmed())
				{
					TargetInfo targetInfo;

					epmDevice->eepromRead();

					targetInfo = epmDevice->_targetInfo;

					std::cout << "Device: " << std::endl;

					quint8 chipset = epmDevice->_version._firmwareVersion[1];

					QString chipsetString{"Unknown"};
					switch (chipset)
					{
					case 3:
						chipsetString = "LP038";
						break;

					case 4:
						chipsetString = "LP030";
						break;
					}

					std::cout << "   Chipset: " << chipsetString.toLatin1().constData() << std::endl;
					std::cout << "   Serial Number: " << targetInfo.serialNumber().constData() << std::endl;
					QString platformDesc = PlatformContainer::toString(targetInfo._platformIdentifier);
					std::cout << QString("   Platform ID: %1 (%2)").arg(platformDesc).arg(targetInfo._platformIdentifier).toLatin1().constData() << std::endl;
					std::cout << "   Model Name: " << targetInfo.modelName().constData() << std::endl;
					std::cout << "   UUID: " << targetInfo._epmBoardUuid.toQUuid().toString().toLatin1().constData() << std::endl;
					QDateTime eepromTime = QDateTime::fromSecsSinceEpoch(targetInfo._eepromProgrammedTime);
					std::cout << "   Program Time: " << eepromTime.toString().toLatin1().constData() << std::endl;

				}

				epmDevice->close();
			}
			catch (...)
			{

			}
		}
	}
}

bool programDevice(PSOCCommandLine& commandLine)
{
	bool result{false};

	EPMDevices epmDevices;

	QByteArray serialNumber = commandLine.serialNumber();
	QByteArray modelName = commandLine.modelName();
	quint32 platformID = commandLine.platformID();

	if (serialNumber.isEmpty() && modelName.isEmpty() && platformID == 0)
	{
		cout << "Parameters are empty. Nothing to program" << endl;
		return false;
	}

	quint32 deviceCount =_EPMDevice::getEPMDevices(epmDevices);
	if (deviceCount == 1)
	{
		TargetInfo targetInfo;

		EPMDevice epmDevice = epmDevices.at(0);

		if (epmDevice.isNull() == false)
		{
			try
			{
				epmDevice->open();

				if (epmDevice->programmed())
				{
					epmDevice->eepromRead();

					targetInfo = epmDevice->_targetInfo;
				}
				else
					targetInfo._eepromProgrammedTime = QDateTime::currentDateTime().toSecsSinceEpoch();

				if (targetInfo._epmBoardUuid.toQUuid().isNull())
				{
					QUuid uuid = QUuid::createUuid();
					targetInfo._epmBoardUuid = uuid;
				}

				if (platformID != 0)
					targetInfo._platformIdentifier = platformID;

				if (serialNumber.isEmpty() == false)
					targetInfo.setSerialNumber(serialNumber);

				if (modelName.isEmpty() == false)
					targetInfo.setModelName(modelName);

				epmDevice->_targetInfo = targetInfo;

				try
				{
					epmDevice->eepromWrite();

					cout << "EEPROM Updated" << endl << "   The EEPROM has been updated with the new data." << endl;

					result = true;
				}
				catch (EPMException& error)
				{
					Q_UNUSED(error)

					cout << "EEPROM Error" << endl << "The EEPROM has not been updated with the new data." << endl;
				}

			}
			catch (EPMException& epmError)
			{
				if (epmError.epmError() == MICRO_EPM_NOT_PROGRAMMED)
				{
					cout << "Unprogrammed Debug Board \n" << endl <<
						"This debug board has not been programmed by the factory. "
						"The Alpaca group has no idea who that contact is for your group. Ask the person who supplied the MTP or device.  You are going to have "
						"to find the board supplier for your group." << endl;
				}
			}
		}
		else
		{
			cout << "Device Not Found" << endl;
		}
	}
	else
	{
		cout << "One, and only one device can be plugged in for this operation" << endl;
	}

	return result;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	a.setApplicationName(kAppName);
	a.setApplicationVersion(kAppVersion);

	AppCore* appCore = AppCore::getAppCore();
	PreferencesBase preferencesBase;

	preferencesBase.setAppName(kAppName, kAppVersion);
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();

		PSOCCommandLine parser(a.arguments());

	if (parser.helpRequested() == false)
	{
		bool earlyReturn{false}; // if the user asks for information items, don't program the device

		_EPMDevice::updateDeviceList();

		if (parser.versionRequested())
		{
			std::cout << "Version: " << parser.version().data() << endl;
			earlyReturn = true;
		}

		if (parser.listSet())
		{
			listDevices();
			earlyReturn = true;
		}

		if (parser.listPlatformSet())
		{
			listPlatforms();
			earlyReturn = true;
		}

		if (parser.getProgramming())
		{
			getProgramming();
			earlyReturn = true;
		}

		if (earlyReturn == false)
		{
			if (parser.programSet())
			{
				programDevice(parser);
			}
		}
	}
	else
	{
		QString helpText = parser.helpText();
		std::cout << helpText.toLatin1().data();
	}

	return 0;
}
