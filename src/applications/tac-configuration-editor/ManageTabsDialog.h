#ifndef MANAGETABSDIALOG_H
#define MANAGETABSDIALOG_H
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
// Copyright 2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
