#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonGlobal.h"

class Ui_ProgressDialogClass;

// Qt
#include <QDialog>
#include <QTimer>

class QCOMMON_EXPORT ProgressDialog :
	public QDialog
{
Q_OBJECT

public:
	ProgressDialog(const QString& description, quint32 duration, QWidget* parent = Q_NULLPTR);

private slots:
	void on__timer_timeout();

protected:
	void changeEvent(QEvent* e);

	Ui_ProgressDialogClass*		_ui{Q_NULLPTR};

	QTimer						_timer;
	quint32						_current;
	quint32						_duration;
};

#endif // PROGRESSDIALOG_H
