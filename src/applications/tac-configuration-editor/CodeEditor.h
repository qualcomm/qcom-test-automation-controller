#ifndef CODEEDITOR_H
#define CODEEDITOR_H
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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "PlatformConfiguration.h"

#include <QDialog>

#include "ui_CodeEditor.h"

class CodeEditor :
	public QDialog,
	Ui::CodeEditor
{
	Q_OBJECT

public:
	CodeEditor(PlatformConfiguration platformConfig, QWidget* parent = Q_NULLPTR);
	~CodeEditor();

	void setScript();
	QByteArray getScript();

	void readVariables();

signals:
	void startNotification(const QString& message);
	void ScriptVariablesUpdated();

private slots:
	void on_buttonBox_accepted();
	void on__addVariable_clicked();
	void on__removeVariable_clicked();
	void on__variableTable_cellClicked(int row, int column);
	void onVariableTypeChanged(QString newType);
	void onVariableTableItemChanged(QTableWidgetItem* twi);
	void onVariableCheckBoxUpdated(bool newState);
	void onVariableTableItemDoubleClicked(QTableWidgetItem* twi);

	void on__variableTable_itemClicked(QTableWidgetItem *item);

private:
	void setupVariableTableColumns();

	PlatformConfiguration				_platformConfiguration;
};

#endif // CODEEDITOR_H
