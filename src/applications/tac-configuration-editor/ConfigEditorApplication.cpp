// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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

