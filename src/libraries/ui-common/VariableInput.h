#ifndef VARIABLEINPUT_H
#define VARIABLEINPUT_H

// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
