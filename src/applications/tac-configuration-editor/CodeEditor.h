#ifndef CODEEDITOR_H
#define CODEEDITOR_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
