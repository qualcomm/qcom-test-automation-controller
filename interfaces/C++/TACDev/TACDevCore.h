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

#include "TACDev.h"

// libTAC
#include "AlpacaDevice.h"
#include "TACPreferences.h"

// QCommon
#include "AlpacaSharedLibrary.h"

// QT
#include <QMap>

using namespace std;

class DevTacCore :
	public QObject,
	public AlpacaSharedLibrary
{
	Q_OBJECT
public:
	DevTacCore()
	{
	}

	~DevTacCore()
	{
	}

	bool initialize(const QByteArray& appName, const QByteArray& appVersion);

	void setLoggingState(bool state);
	bool getLoggingState();

	AppCore* appCore();

	AlpacaDevice getAlpacaDevice(TAC_HANDLE tacHandle);

	TAC_RESULT GetDeviceCount(int* deviceCount)
	{
		TAC_RESULT result{TACDEV_INIT_FAILED};

		if (_initialized == true)
		{
			*deviceCount = 0;
			_AlpacaDevice::updateAlpacaDevices();
			_AlpacaDevice::getAlpacaDevices(_alpacaDevices);

			*deviceCount = _alpacaDevices.count();

			result = NO_TAC_ERROR;
		}

		return result;
	}

	const AlpacaDevices& GetAlpacaDevices()
	{
		return _alpacaDevices;
	}

	TAC_HANDLE OpenHandleByDescription(const char* portName);
	TAC_RESULT CloseTACHandle(TAC_HANDLE tacHandle);

private slots:
	void onErrorEvent(const QByteArray& message);

private:
	bool							_initialized{false};
	TACPreferences					_preferences;
	AlpacaDevices					_alpacaDevices;

	QMap<TAC_HANDLE, AlpacaDevice>	_openDevices;

};
