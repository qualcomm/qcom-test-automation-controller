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

#include "AlpacaSharedLibrary.h"

// QCommon
#include "QCommonConsole.h"

// Qt
#include <QDateTime>
#include <QDir>

#include <QTextStream>

AppCore* AlpacaSharedLibrary::_appCore{Q_NULLPTR};

AlpacaSharedLibrary::AlpacaSharedLibrary()
{
}

AlpacaSharedLibrary::~AlpacaSharedLibrary()
{
}

AppCore* AlpacaSharedLibrary::getAppCore()
{
	if (AlpacaSharedLibrary::_appCore == Q_NULLPTR)
	{
		AlpacaSharedLibrary::_appCore = AppCore::getAppCore();
	}

	return AlpacaSharedLibrary::_appCore;
}

bool AlpacaSharedLibrary::initialize
(
	const QByteArray &appName,
	const QByteArray &appVersion,
	PreferencesBase* preferencesBase
)
{
	bool result{false};

	_appName = appName;
	_appVersion = appVersion;

	AppCore* appCore = getAppCore();
	if (appCore != Q_NULLPTR)
	{
		appCore->setPreferences(preferencesBase);

		InitializeQCommonConsole();
	}

	return result;
}

bool AlpacaSharedLibrary::licenseIsValid()
{
	/* let me know when LIME works
		if (_validLicense == false)
			_validLicense = _appCore.checkLicense(kProductID, kCoreFeature);

		*/

	return _validLicense;
}

void AlpacaSharedLibrary::setLoggingState(bool state)
{
	AppCore* appCore = getAppCore();
	if (appCore != Q_NULLPTR)
	{
		appCore->setAppLogging(state);
	}
}

bool AlpacaSharedLibrary::getLoggingState()
{
	bool result{false};

	AppCore* appCore = getAppCore();
	if (appCore != Q_NULLPTR)
	{
		result = appCore->appLoggingActive();
	}

	return result;
}
