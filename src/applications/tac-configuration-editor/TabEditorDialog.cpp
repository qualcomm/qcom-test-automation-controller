// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "TabEditorDialog.h"

TabEditorDialog::TabEditorDialog
(
	Tab tab,
	QWidget* parent
) :
	QDialog(parent),
	_tab(tab)
{
	setupUi(this);

	QString tabName;
	bool selectAll{false};

	if (tab._newText.isEmpty() == false)
	{
		tabName = tab._newText;
	}
	else if (tab._name.isEmpty() == false)
	{
		tabName = tab._name;
	}
	else
	{
		tabName = "<Enter Tab Text>";
		selectAll = true;
	}

	_tabName->setText(tabName);
	_tabName->setEnabled(tab._userTab);
	if (selectAll)
		_tabName->selectAll();
}

TabEditorDialog::~TabEditorDialog()
{
}


void TabEditorDialog::on__visible_clicked(bool checked)
{
	_tab._visible = checked;
}


void TabEditorDialog::on_buttonBox_accepted()
{
	_tab._newText = _tabName->text();
}

