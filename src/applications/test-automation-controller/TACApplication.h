#ifndef TACAPPLICATION_H
#define TACAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// TAC
class TACWindow;

// libTAC
#include "TACPreferences.h"

// QCommon
#include "AlpacaApplication.h"

// Qt
#include <QList>

class TACApplication :
	public AlpacaApplication
{
Q_OBJECT

public:
	TACApplication(int &argc, char **argv);
	~TACApplication();

	static TACApplication* tacAppInstance();
	static TACWindow* createTACWindow(bool show = true);
	static TACWindow* openByName(const QByteArray& deviceName);
	bool openLastDevice();

	static bool disconnectTACWindow(TACWindow* tacWindow);

	static bool checkToSeeIfPortIsInUse(const QByteArray& portName);

	void showAboutDialog();
	void quitTAC();
	void quitTACByMsg();

	TACPreferences* preferences()
	{
		return &_preferences;
	}

public slots:
	void on_preferencesChanged();

signals:
	void preferencesChanged();

protected:
	virtual void shutDown();

private:
	static bool					_starting;
	static QList<TACWindow*>	_tacWindows;

	TACPreferences				_preferences;
	static QPoint				_createWindowOrigin;
};

#endif // TACAPPLICATION_H
