// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// TACConfigEditor
#include "ConfigEditorApplication.h"
#include "ConfigWindow.h"


// QCommon
#include "QCommon.h"

int main(int argc, char *argv[])
{
	ConfigEditorApplication a(argc, argv);

	InitializeQCommon();

	ConfigWindow* w = ConfigEditorApplication::createConfigWindow();
	QStringList arguments = QApplication::arguments();
	if (arguments.count() > 1)
	{
		QString tacConfigFile = arguments.at(1);
		if (tacConfigFile.endsWith(".tcnf", Qt::CaseInsensitive))
			w->openFile(tacConfigFile);
	}

	return a.exec();
}
