#ifndef CODEEDITOR_H
#define CODEEDITOR_H
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
