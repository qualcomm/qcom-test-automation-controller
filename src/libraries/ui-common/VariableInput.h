#ifndef VARIABLEINPUT_H
#define VARIABLEINPUT_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: Biswajit Roy (biswroy@qti.qualcomm.com)

#include "ui_VariableInput.h"

// qcommon-console
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
