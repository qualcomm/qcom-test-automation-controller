// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AlpacaApplication.h"
#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"
#include "QTACDefines.h"
#include "EncryptedString.h"
#include "QuitAppEvent.h"

// Qt
#include <QtConcurrentRun>
#include <QDateTime>
#include <QDir>
#include <QFont>
#include <QMessageBox>

#include <QTextStream>

AlpacaApplication::AlpacaApplication
(
	int &argc,
	char **argv,
	const QString& appName,
	const QString& appVersion
) :
	QApplication(argc, argv),
	_appName(appName),
	_appVersion(appVersion)
{
	_appCore = AppCore::getAppCore();

	setApplicationName(kProductName);
	setOrganizationName("Qualcomm, Inc.");
	setOrganizationDomain("www.qualcomm.com");

#ifdef Q_OS_LINUX
	QFont font("Tahoma", 8);
	QApplication::setFont(font);
#endif

	setupApplicationStyle();
}

AlpacaApplication::~AlpacaApplication()
{
	AppCore::writeToApplicationLogLine("AlpacaApplication::~AlpacaApplication()");
}

bool AlpacaApplication::initialize(PreferencesBase *preferencesBase)
{
	bool result{false};

	_appCore->setPreferences(preferencesBase);

	cleanupLogs();

	result = true;

	return result;
}

AlpacaApplication* AlpacaApplication::alpacaAppinstance()
{
	return qobject_cast<AlpacaApplication*>(QCoreApplication::instance());
}

void AlpacaApplication::cleanupLogs()
{
	PreferencesBase* preferences = AppCore::getAppCore()->getPreferences();
	if (preferences != Q_NULLPTR)
	{
		cleanIt(preferences->appLogPath());
		cleanIt(preferences->runLogPath());
	}
}

bool AlpacaApplication::event(QEvent *e)
{
	QSettings alpacaSettings;

	quint32 eventType = e->type();
	switch (eventType)
	{
	case kQuitAppEvent:
		AppCore::writeToApplicationLogLine(_appName.toStdString() + " received an application quit event.");
		shutDown();
		break;

	default:
		return QApplication::event(e);
	}

	return true;
}
