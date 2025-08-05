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
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "FTDICheckCommandLine.h"
#include "FTDICheckApplication.h"

// qcommon-console
#include "AppCore.h"
#include "PreferencesBase.h"
#include "QCommonConsole.h"
#include "version.h"

// Qt
#include <QCoreApplication>

// C++
#include <iostream>

int main(int argc, char *argv[])
{
	int result{0};
	QString kAppName{"ftdi-check"};

	QCoreApplication a(argc, argv);

	a.setApplicationName(kAppName);
	a.setApplicationVersion(FTDI_CHECK_VERSION);

	AppCore* appCore = AppCore::getAppCore();
	PreferencesBase preferencesBase;

	preferencesBase.setAppName(kAppName.toLatin1(), UPDATE_DEVICE_LIST);
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();

	FTDICheckCommandLine parser(a.arguments());
	FTDICheckApplication ftdiApp;

	if (parser.helpRequested())
	{
		QString helpText = parser.helpText();
		std::cout << helpText.toLatin1().data() << std::endl;
	}
	else if (parser.value("validate") == "true")
		result = ftdiApp.verifyInstalledDriver();
	else if (parser.value("uninstall") == "true")
		result = ftdiApp.uninstallDriver();
	else if (parser.value("install") == "true")
		result = ftdiApp.installDriver();
	else if (parser.value("version") == "true")
		ftdiApp.showVersion();
	else
		result = ftdiApp.defaultActivity();

	return result;
}
