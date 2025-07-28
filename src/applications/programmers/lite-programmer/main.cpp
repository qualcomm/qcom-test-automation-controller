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
*/


// lite-programmer
#include "ProgrammerCommandLine.h"

// qcommon-console
#include "FTDIDevice.h"
#include "Range.h"
#include "QCommonConsole.h"

// Qt
#include <QCoreApplication>

// c++
#include <iostream>

using namespace  std;

void listDevices()
{
	AlpacaDevices alpacaDevices;

	_AlpacaDevice::getAlpacaDevices(alpacaDevices, eFTDI);

	auto deviceCount = alpacaDevices.count();
	if (deviceCount > 0)
	{
		cout << "Devices" << endl;
		for (auto index : range(deviceCount))
		{
			auto alpacaDevice = alpacaDevices.at(index);

			cout << "   Device:" << alpacaDevice->portName().data() <<
				 " GUID:" << alpacaDevice->uuid().toLatin1().data() <<
				 " Platform ID:" << QByteArray::number(alpacaDevice->platformID()).data() << endl;
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
	PlatformIDList platformIDs = PlatformContainer::getDebugBoardsOfType(eFTDI);

	cout << "Platforms" << endl;
	for (const auto& platformID: platformIDs)
	{
		cout << "   Platform: " << platformID->_description.toLatin1().data() << " ID: " << QString::number(platformID->_platformID).toLatin1().data() << endl;
	}

	cout << endl;
}

bool programDevice
(
	ProgrammerCommandLine& commandLine
)
{
	bool result{false};

	QByteArray serialNumber = commandLine.serialNumber().toLower();
	PlatformID platformID = commandLine.platformID();
	if (serialNumber.isEmpty() == false)
	{
		if (platformID != 0)
		{
			AlpacaDevices alpacaDevices;

			_AlpacaDevice::getAlpacaDevices(alpacaDevices, eFTDI);

			auto deviceCount = alpacaDevices.count();
			if (deviceCount > 0)
			{
				bool programmingError{false};

				for (auto index : range(deviceCount))
				{
					auto alpacaDevice = alpacaDevices.at(index);

					if (alpacaDevice->serialNumber().toLower() == serialNumber.toLower())
					{
						QByteArray errorMessage;

						result = FTDIDevice::programDevice(alpacaDevice, platformID, errorMessage);
						if (result == false)
						{
							cout << QString("FTDIDevice::programDevice failed " + errorMessage).toLatin1().data() << endl;
							programmingError = true;
						}
						else
							result = true;

						break;
					}
				}

				if (result == false && programmingError == false)
					cout << QString("Serial Number %1 not found").arg(serialNumber.data()).toLatin1().data() << endl;
			}
			else
			{
				cout << "No FTDI devices found." << endl;
			}
		}
		else
		{
			cout << "Platform ID required when programming the device" << endl;
		}
	}
	else
	{
		cout << "Serial Number required when programming the device" << endl;
	}

	return result;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	a.setApplicationName(kAppName);
	a.setApplicationVersion("1.1");

	AppCore* appCore = AppCore::getAppCore();
	PreferencesBase preferencesBase;

	preferencesBase.setAppName(kAppName.toLatin1(), "1.1");
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();
	ProgrammerCommandLine parser(a.arguments());

	if (parser.helpRequested() == false)
	{
		bool earlyReturn{false}; // if the user asks for information items, don't program the device

		if (parser.versionRequested())
		{
			std::cout << "Version: " << parser.version().data() << endl;
			earlyReturn = true;
		}

		_AlpacaDevice::updateAlpacaDevices();

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
}
