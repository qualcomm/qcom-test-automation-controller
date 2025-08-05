// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
