/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
