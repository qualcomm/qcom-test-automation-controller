#ifndef ALPACAAPPLICATION_H
#define ALPACAAPPLICATION_H
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
// Copyright 2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonGlobal.h"

// QCommon
#include "AppCore.h"

// QT
#include <QApplication>

class QCOMMON_EXPORT AlpacaApplication :
	public QApplication
{
Q_OBJECT

public:
	explicit AlpacaApplication(int& argc, char** argv, const QString& appName, const QString& appVersion);
	~AlpacaApplication();

	bool initialize(PreferencesBase* preferencesBase);

	static AlpacaApplication* alpacaAppinstance();
	static AppCore* appCore()
	{
		return AlpacaApplication::alpacaAppinstance()->_appCore;
	}

	bool readyToRate();
	void showRateDialog();

protected:
	void cleanupLogs();

	virtual void shutDown() = 0;
	virtual bool event(QEvent* e);

protected:
	QString						_appName;
	QString						_appVersion;
	AppCore*					_appCore{Q_NULLPTR};
	bool						_dateStale{false};
};

#endif // ALPACAAPPLICATION_H
