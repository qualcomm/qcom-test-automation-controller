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

// tacLib
#include "AlpacaDevice.h"
#include "TACPreferences.h"

// QCommon
#include "AppCore.h"
#include "QCommonConsole.h"

// Qt
#include <QCoreApplication>

// C++
#include <iostream>

using namespace std;

class DevTacCore
{
public:
	DevTacCore()
	{
		_appCore = AppCore::getAppCore();

		_preferences.setAppName("TAC", "1.1.1");
		_appCore->setPreferences(&_preferences);
		_appCore->postAutomationEvent();
	}

	~DevTacCore()
	{
	}

	void setLoggingState(bool state)
	{
		_appCore->setAppLogging(state);
	}

	bool getLoggingState()
	{
		return _appCore->appLoggingActive();
	}

	AppCore* appCore()
	{
		return AppCore::getAppCore();
	}

private:
	TACPreferences				_preferences;
	AppCore*					_appCore{Q_NULLPTR};
};

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	DevTacCore tacCore;

	InitializeQCommonConsole();

	QString errorString;
	bool forQuts{false};

	if (argc > 1)
	{
		if (a.arguments().at(1) == "-quts")
			forQuts = true;
	}

	AlpacaDevices alpacaDevices;

	_AlpacaDevice::updateAlpacaDevices();
	_AlpacaDevice::getAlpacaDevices(alpacaDevices);

	if (alpacaDevices.count() > 0)
	{
		if (!forQuts)
			cout << "Devices:" << endl;

		for (const auto& alpacaDevice : std::as_const(alpacaDevices))
		{
			if (!forQuts)
			{
				cout << "   Device:" << alpacaDevice->portName().data() << " Serial:" << alpacaDevice->serialNumber().data() << endl;
			}
			else
			{
				cout << alpacaDevice->portName().data() << ";" << alpacaDevice->serialNumber().data()<< ";" << endl;
			}
		}
	}
	else
	{
		if (errorString.isEmpty())
		{
			cout << "No devices found." << endl;
		}
		else
		{
			cout << errorString.toLatin1().data() << endl;
		}
	}
}
