#ifndef CREATECONFIGURATIONDIALOG_H
#define CREATECONFIGURATIONDIALOG_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "ui_CreateConfigurationDialog.h"
#include "DebugBoardType.h"

// QT
#include <QDialog>

class CreateConfigurationDialog :
	public QDialog,
	Ui::CreateConfigurationDialogClass
{
	Q_OBJECT

public:
	CreateConfigurationDialog(QWidget* parent);
	~CreateConfigurationDialog();

	DebugBoardType getPlatform();
	int getChipCount();

private slots:
	void on__platformComboBox_currentIndexChanged(int index);

private:
	DebugBoardType _platformType{eUnknownDebugBoard};
};

#endif // CREATECONFIGURATIONDIALOG_H
