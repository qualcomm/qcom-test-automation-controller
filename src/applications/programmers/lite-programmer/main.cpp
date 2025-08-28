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


// LITEProgrammer
#include "ProgrammerCommandLine.h"

// QCommonConsole
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
