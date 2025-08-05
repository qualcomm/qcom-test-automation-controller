#ifndef TABEDITORDIALOG_H
#define TABEDITORDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "Tabs.h"

#include <QDialog>

#include "ui_TabEditorDialog.h"

class TabEditorDialog :
	public QDialog,
	public Ui::TabEditorDialog
{
	Q_OBJECT

public:
	explicit TabEditorDialog(Tab tab, QWidget* parent = Q_NULLPTR);
	~TabEditorDialog();

	Tab getTab()
	{
		return _tab;
	}


private slots:
	void on__visible_clicked(bool checked);

	void on_buttonBox_accepted();

private:
	Tab							_tab;
};

#endif // TABEDITORDIALOG_H
