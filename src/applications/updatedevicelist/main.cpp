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

// Update Device List
#include "FTDITemplateCompiler.h"
#include "UDLCommandLine.h"
#include "UpdateDeviceList.h"

// qcommon-console
#include "AppCore.h"
#include "PreferencesBase.h"
#include "QCommonConsole.h"
#include "version.h"

// QT
#include <QCoreApplication>
#include <QDir>

// C++
#include <iostream>


int main(int argc, char *argv[])
{
	QString kAppName{"UpdateDeviceList"};

	QCoreApplication a(argc, argv);

	a.setApplicationName(kAppName);
	a.setApplicationVersion(UPDATE_DEVICE_LIST);

	AppCore* appCore = AppCore::getAppCore();
	PreferencesBase preferencesBase;

	preferencesBase.setAppName(kAppName.toLatin1(), UPDATE_DEVICE_LIST);
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();

	UDLCommandLine parser(a.arguments());
	UpdateDeviceList deviceList;
    FTDITemplateCompiler ftdiTemplateCompiler;

	bool verbose = parser.verbose();

	if (parser.helpRequested() == false)
	{
		QString fileDir = parser.userDir();
		deviceList.setDeviceListDir(fileDir);
		deviceList.setVerbosity(verbose);
		deviceList.write();

        ftdiTemplateCompiler.setDeviceListDir(fileDir);
        ftdiTemplateCompiler.setVerbosity(verbose);
        ftdiTemplateCompiler.write();
	}
	else
	{
		QString helpText = parser.helpText();
		std::cout << helpText.toLatin1().data() << std::endl;
	}

	return 0;
}
