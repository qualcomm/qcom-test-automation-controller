#ifndef ALPACAAPPLICATION_H
#define ALPACAAPPLICATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear
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
