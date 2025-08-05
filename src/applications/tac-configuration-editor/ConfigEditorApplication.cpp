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

#include "ConfigEditorApplication.h"
#include "ConfigWindow.h"

// libTAC
#include "TACDefines.h"

// QCommon
#include "AboutDialog.h"

//QT
#include <QFileDialog>
#include <QMessageBox>

ConfigEditorApplication::ConfigEditorApplication
(
	int& argc,
	char** argv
) :
	AlpacaApplication (argc, argv, kTACConfigEditorApp, kTACConfigEditorVersion)
{
	_preferences.setAppName(kTACConfigEditorApp.toLatin1(), kTACConfigEditorVersion.toLatin1());

	// TODO: do we need a preference for the configuration editor, Michael?
	// _preferences.setLoggingActive(true);
	initialize(&_preferences);
}

ConfigEditorApplication::~ConfigEditorApplication()
{

}

ConfigWindow* ConfigEditorApplication::createConfigWindow()
{
	ConfigWindow* result;

	result = new ConfigWindow;
	result->setVisible(true);

	return result;
}

void ConfigEditorApplication::showAboutDialog()
{
	AboutDialog aboutDialog(Q_NULLPTR);

	aboutDialog.setTitle("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">TAC Configuration Editor</span></p></body></html>");

	QFile file(":/About.txt");
	if (file.open(QIODevice::ReadOnly) == true)
	{
		QByteArray html = file.readAll();
		aboutDialog.setAboutText(html);
		file.close();
	}

	aboutDialog.setAppName(kTACConfigEditorApp);
	aboutDialog.setAppVersion(kTACConfigEditorVersion.toLatin1());

	QPixmap pixMap = QPixmap(QString::fromUtf8(":/TACConfigEditor_BS.png"));
	aboutDialog.setBackSplash(pixMap);

	aboutDialog.exec();
}

ConfigEditorApplication *ConfigEditorApplication::appInstance()
{
	return qobject_cast<ConfigEditorApplication*>(QCoreApplication::instance());
}

void ConfigEditorApplication::shutDown()
{
	quit();
}

