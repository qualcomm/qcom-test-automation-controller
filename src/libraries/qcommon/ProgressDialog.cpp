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
// Copyright 2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "ProgressDialog.h"

#include "ui_ProgressDialog.h"

ProgressDialog::ProgressDialog
(
	const QString& description,
	quint32 duration,
	QWidget* parent
) :
	QDialog(parent),
	_current(0),
	_duration(duration)
{
	_ui = new Ui::ProgressDialogClass;

	_ui->setupUi(this);

	_ui->_description->setText(description);
	_ui->_progressBar->setMaximum(static_cast<int>(duration));

	_timer.setInterval(1000);
	connect(&_timer, &QTimer::timeout, this, &ProgressDialog::on__timer_timeout);
	_timer.start();
}

void ProgressDialog::on__timer_timeout()
{
	_current++;
	_ui->_progressBar->setValue(static_cast<int>(_current));
	if (_current > _duration)
	{
		_timer.stop();
		accept();
	}
}

void ProgressDialog::changeEvent
(
	QEvent* e
)
{
	QDialog::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		_ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
