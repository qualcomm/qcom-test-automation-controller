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
			Anmol Jaiswal (anmojais@qti.qualcomm.com)
*/

#include "PreferencesDialog.h"

// TAC
#include "TACApplication.h"

// QCommon

// Qt
#include <QFileDialog>

PreferencesDialog::PreferencesDialog
(
	TACPreferences* preferences,
	QWidget* parent
) :
	QDialog(parent),
	_tacPreferences(preferences)
{
	setupUi(this);

	connect(this, &PreferencesDialog::preferencesChanged, TACApplication::tacAppInstance(), &TACApplication::on_preferencesChanged);

	_openLastDeviceOnStart->setChecked(_tacPreferences->openLastStart());

	_loggingCB->setChecked(_tacPreferences->loggingActive());
	_logLocation->setText(_tacPreferences->appLogPath());

	_autoShutDownEnable->setChecked(_tacPreferences->autoShutdownActive());
	_autoShutdownHours->setValue(_tacPreferences->autoShutdownDelay());

	connect(this, &QDialog::accepted, this, &PreferencesDialog::on_accepted);
}

void PreferencesDialog::on__setToDefaultsButton_clicked()
{
	_openLastDeviceOnStart->setChecked(_tacPreferences->defaultOpenLastStart());

	_loggingCB->setChecked(_tacPreferences->defaultLoggingState());
	_logLocation->setText(_tacPreferences->defaultAppLogPath());

	_autoShutDownEnable->setChecked(false);
	_autoShutdownHours->setValue(24.0);
}

void PreferencesDialog::on__browseForLogPathButton_clicked()
{
	QString currentDir = QDir::cleanPath(_logLocation->text());
	QString newLogLocation = QDir::cleanPath(QFileDialog::getExistingDirectory(this, "Select logs directory", currentDir));
	if (newLogLocation.isEmpty() == false)
	{
		if (newLogLocation.toLower() != currentDir.toLower())
		{
			_logLocation->setText(newLogLocation);
		}
	}
}

void PreferencesDialog::on_accepted()
{
	_tacPreferences->saveOpenLastStart(_openLastDeviceOnStart->isChecked());

	_tacPreferences->saveLoggingActive(_loggingCB->isChecked());
	_tacPreferences->saveAppLogPath(_logLocation->text());

	_tacPreferences->saveAutoShutdownActive(_autoShutDownEnable->isChecked());
	_tacPreferences->saveAutoShutdownDelay(_autoShutdownHours->value());

	emit preferencesChanged();
}

void PreferencesDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;

	default:
		break;
	}
}
