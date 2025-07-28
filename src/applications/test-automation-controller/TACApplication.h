#ifndef TACAPPLICATION_H
#define TACAPPLICATION_H
// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2019-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
