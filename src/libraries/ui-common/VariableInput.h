#ifndef VARIABLEINPUT_H
#define VARIABLEINPUT_H

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

// Author: Biswajit Roy (biswroy@qti.qualcomm.com)

#include "ui_VariableInput.h"

// QCommonConsole
#include "ScriptVariable.h"

// Qt
#include <QWidget>


class VariableInput : public QWidget, public Ui::_variableInputWidget
{
	Q_OBJECT

public:
	explicit VariableInput(QWidget *parent = nullptr);
	~VariableInput();

	void setName(const QString& name);
	QString& name();

	void setLabel(const QString& label, const VariableType variableType);
	QString& label();

	void setTooltip(const QString& tooltip);
	QString& tooltip();

	void setType(const VariableType variableType);
	VariableType type();

	// TODO: What's the difference between setValue() and setDefaultValue()?
	void setDefaultValue(const QVariant& value, const VariableType variableType);
	QVariant defaultValue();

	void setValue(const QVariant& value);
	QVariant value();

signals:
	void variableValueUpdated(ScriptVariable scriptVariable);

private slots:
	void onSpinBoxValueChanged(double value);

private:
	ScriptVariable              _variable;
};

#endif // VARIABLEINPUT_H
