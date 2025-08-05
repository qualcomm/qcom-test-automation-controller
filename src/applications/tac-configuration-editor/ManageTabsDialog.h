#ifndef MANAGETABSDIALOG_H
#define MANAGETABSDIALOG_H
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
