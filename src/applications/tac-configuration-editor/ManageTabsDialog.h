#ifndef MANAGETABSDIALOG_H
#define MANAGETABSDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/


// libTAC
#include "PlatformConfiguration.h"
#include "Tabs.h"

// QT
#include <QDialog>

#include "ui_ManageTabsDialog.h"

class ManageTabsDialog :
	public QDialog,
	Ui::ManageTabsDialog
{
	Q_OBJECT

public:
	ManageTabsDialog(PlatformConfiguration platformConfiguration, QWidget* parent = Q_NULLPTR);
	~ManageTabsDialog();

	void setupTabs();

	Tabs getDeletedTabs();
	Tabs getTabs();

private slots:
	void on__addTabButton_clicked();
	void on__removeTabButton_clicked();
	void on__renameTabButton_clicked();
	void on__moveTabUpButton_clicked();
	void on__moveTabDownButton_clicked();

	void onCurrentRowChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

	void onTabVisibleChecked(bool checkedState);

private:
	Tab getTab(QTableWidgetItem* twi);
	void updateEnablements();
	bool testTabForMove(int tabIndex);

	PlatformConfiguration		_platformConfiguration;
	Tabs						_tabs;
	Tabs						_deletedTabs;
};

#endif // MANAGETABSDIALOG_H
