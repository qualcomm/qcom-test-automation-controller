#ifndef CONFIGEDITORAPPLICATION_H
#define CONFIGEDITORAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

class ConfigWindow;

#include "TACPreferences.h"

// QCommon
#include "AlpacaApplication.h"

const QString kTACConfigPath(QStringLiteral("Last TAC Config Path"));

class ConfigEditorApplication :
	public AlpacaApplication
{
Q_OBJECT

public:
	ConfigEditorApplication(int &argc, char **argv);
	~ConfigEditorApplication();

	static ConfigWindow* createConfigWindow();

	void showAboutDialog();

	static ConfigEditorApplication* appInstance();

	TACPreferences* preferences()
	{
		return &_preferences;
	}

	TACPreferences				_preferences;
protected:
	virtual void shutDown();
};

#endif // CONFIGEDITORAPPLICATION_H
