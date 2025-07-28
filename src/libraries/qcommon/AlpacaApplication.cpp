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

#include "AlpacaApplication.h"
#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"
#include "AlpacaDefines.h"
#include "DateCheckFailEvent.h"
#include "EncryptedString.h"
#include "HappinessDialog.h"
#include "LicenseFailEvent.h"
#include "QuitAppEvent.h"

// Qt
#include <QtConcurrentRun>
#include <QDateTime>
#include <QDir>
#include <QFont>
#include <QMessageBox>

#include <QTextStream>

// These strings are encrypted to prevent dynamic and static analyzers from fixing on the licensing.
EncryptedString kAboutMessage("161e191517042f4c1218432c280f1800060d613f190c13122e02504c430c32051d11100e2f2c01150a4f3019110d000e2c015e020c0c4b23020804082f0d1c41200e2f0f151117412315502b0c0461230204100a2e66310d1300220d50290213251b111306411509110c434c613e110f07086128152d02132040502a1613354c320810092e0a1604114d613505190a0f612415");
static EncryptedString kLicenseCheckFailed("0d0513040d12244c330906022a4c36000a0d2408");
static EncryptedString kLicenseMgrFailed("0d0513040d12244c3d000d00260902412a0f3218110f0004612a11080f");

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

	kickIt();
}

bool AlpacaApplication::initialize(PreferencesBase *preferencesBase)
{
	bool result{false};

	_appCore->setPreferences(preferencesBase);

	_appCore->postStartEvent();

	AppCore::writeToApplicationLogLine("readyToRate");
	if (readyToRate())
	{
		AppCore::writeToApplicationLogLine("showRateDialog");

		showRateDialog();
	}

	cleanupLogs();

	result = true;

	return result;
}

AlpacaApplication* AlpacaApplication::alpacaAppinstance()
{
	return qobject_cast<AlpacaApplication*>(QCoreApplication::instance());
}

bool AlpacaApplication::readyToRate()
{
	bool result{false};

	QSettings alpacaSettings;

	bool rated = alpacaSettings.value("rated", false).toBool();
	if (rated)
	{
		QDate lastRate = alpacaSettings.value("lastRating", QDate()).toDate();
		if (lastRate.isValid())
		{
			lastRate = lastRate.addDays(30);

			if (lastRate <= QDate::currentDate())
				result = true;
		}
		else
		{
			alpacaSettings.setValue("lastRating", QDate::currentDate());
		}
	}
	else
	{
		alpacaSettings.setValue("rated", true);
		alpacaSettings.setValue("lastRating", QDate::currentDate());
	}

	return result;
}

void AlpacaApplication::showRateDialog()
{
	HappinessDialog happinessDialog(Q_NULLPTR);

	if (happinessDialog.exec() == QDialog::Accepted)
	{
		HappinessRating rating = happinessDialog.getRating();

		QByteArray ratingString = "Rating";

		double value{.0};

		switch (rating)
		{
		case eNotSet: break;
		case eDissatisfied: value = 1.0; break;
		case eUnhappy: value = 2.0; break;
		case eSatisfied: value = 3.0; break;
		case eHappy: value = 4.0; break;
		case eLove: value = 5.0; break;
		}

		_appCore->postMetric(ratingString, value);
	}

	QSettings alpacaSettings;

	alpacaSettings.setValue("lastRating", QDate::currentDate());
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
		AppCore::writeToApplicationLogLine(_appName + " received an application quit event.");
		shutDown();
		break;

	case kDateCheckFailEvent:
	{
		AppCore::writeToApplicationLogLine("kDateCheckFailEvent");

		bool dateCheckNotified = alpacaSettings.value("dateCheckNotified", false).toBool();
		if (dateCheckNotified == false)
		{
			alpacaSettings.setValue("dateCheckNotified", true);
			//DateCheckFailEvent* dcfe = static_cast<DateCheckFailEvent*>(e);
			QMessageBox::warning(Q_NULLPTR,  "Date Check Failed", "Update your software");
		}
	}
		break;

	case kLicenseFailEvent:
	{
		AppCore::writeToApplicationLogLine("kLicenseFailEvent");

		bool failNotified = alpacaSettings.value("failNotified", false).toBool();
		if (failNotified == false)
		{
			alpacaSettings.setValue("failNotified", true);

			//LicenseFailEvent* lfe = static_cast<LicenseFailEvent*>(e);
			QMessageBox::warning(Q_NULLPTR,  "License Check Failed", "Check QPM for license activation");

		}
	}
		break;

	default:
		return QApplication::event(e);
	}

	return true;
}
