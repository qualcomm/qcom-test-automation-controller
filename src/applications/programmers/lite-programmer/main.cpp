// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// LITEProgrammer
#include "ProgrammerCommandLine.h"

// Qt
#include <QCoreApplication>

// QCommonConsole
#include "AppCore.h"
#include "FTDIDevice.h"
#include "QCommonConsole.h"
#include "Range.h"
#include "StringUtilities.h"

// c++
#include <iostream>

using namespace std;

void listDevices()
{
	AlpacaDevices alpacaDevices;

	_AlpacaDevice::getAlpacaDevices(alpacaDevices, eFTDI);

	auto deviceCount = alpacaDevices.size();
	if (deviceCount > 0)
	{
		cout << "Devices" << endl;
		for (auto index : range(deviceCount))
		{
			auto alpacaDevice = alpacaDevices.at(index);

			cout << "   Device:" << alpacaDevice->portName() <<
				 " GUID:" << alpacaDevice->uuid() <<
				 " Platform ID:" << alpacaDevice->platformID() << endl;
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
		cout << "   Platform: " << platformID->_description << " ID: " << platformID->_platformID << endl;
	}

	cout << endl;
}

bool programDevice
(
	ProgrammerCommandLine& commandLine
)
{
	bool result{false};

	std::string serialNumber = toLower(commandLine.serialNumber());
	PlatformID platformID = commandLine.platformID();
	if (!serialNumber.empty())
	{
		if (platformID != 0)
		{
			AlpacaDevices alpacaDevices;

			_AlpacaDevice::getAlpacaDevices(alpacaDevices, eFTDI);

			auto deviceCount = alpacaDevices.size();
			if (deviceCount > 0)
			{
				bool programmingError{false};

				for (auto index : range(deviceCount))
				{
					auto alpacaDevice = alpacaDevices.at(index);

					if (toLower(alpacaDevice->serialNumber()) == serialNumber)
					{
						std::string errorMessage;

						result = FTDIDevice::programDevice(alpacaDevice, platformID, errorMessage);
						if (!result)
						{
							cout << "FTDIDevice::programDevice failed " << errorMessage << endl;
							programmingError = true;
						}
						else
							result = true;

						break;
					}
				}

				if (!result && !programmingError)
					cout << "Serial Number " << serialNumber << " not found" << endl;
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

	preferencesBase.setAppName(kAppName.toStdString(), "1.1");
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();
	ProgrammerCommandLine parser(a.arguments());

	if (parser.helpRequested() == false)
	{
		bool earlyReturn{false}; // if the user asks for information items, don't program the device

		if (parser.versionRequested())
		{
			std::cout << "Version: " << parser.version() << endl;
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
		std::cout << parser.helpText();
	}
}
