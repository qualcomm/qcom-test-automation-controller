#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear


// Author: Michael Simpson msimpson@qti.qualcomm.com

#include "ui_PreferencesDialog.h"

#include "TACPreferences.h"

class PreferencesDialog :
	public QDialog,
	private Ui::PreferencesDialogClass
{
Q_OBJECT

public:
	PreferencesDialog(TACPreferences* preferences, QWidget* parent = Q_NULLPTR);

signals:
	void preferencesChanged();

private slots:
	void on__setToDefaultsButton_clicked();
	void on__browseForLogPathButton_clicked();
	void on_accepted();

protected:
	void changeEvent(QEvent* e);

	TACPreferences*				_tacPreferences;
};

#endif // PREFERENCESDIALOG_H
