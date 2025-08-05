#ifndef BUTTONEDITOR_H
#define BUTTONEDITOR_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)	
*/

#include "ui_ButtonEditor.h"

// libTAC
#include "PlatformConfiguration.h"

// QT
#include <QGroupBox>


class ButtonEditor :
	public QGroupBox,
	public Ui::ButtonEditor
{
	Q_OBJECT

public:
	ButtonEditor(QWidget* parent = Q_NULLPTR);
	~ButtonEditor();

	void setPlatformConfiguration(PlatformConfiguration platformConfig);
	void resetPlatform();

signals:
	void buttonsTableUpdated();

private slots:
	void on__addButton_clicked();
	void on__removeButton_clicked();
	void on__buttonsTable_itemClicked(QTableWidgetItem *item);

	void onTabsChanged(const QString& newText);
	void onTableItemChanged(QTableWidgetItem* twi);
	void onGroupChanged(const QString& newText);

private:
	void read();
	void updateEnablements();
	void setupColumnProperties();

	PlatformConfiguration		_platformConfiguration;
};

#endif // BUTTONEDITOR_H
