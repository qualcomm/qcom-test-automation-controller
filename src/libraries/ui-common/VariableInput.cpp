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


#include "VariableInput.h"

VariableInput::VariableInput(QWidget *parent) :
	QWidget(parent)
{
	setupUi(this);
	// users can set value from 1s to 120s or 1ms to 120ms
	_variableValue->setRange(1, 120000);
	connect(_variableValue, &QSpinBox::valueChanged, this, &VariableInput::onSpinBoxValueChanged);
}

VariableInput::~VariableInput()
{
}

void VariableInput::setName(const QString &name)
{
	_variable._name = name;
}

QString &VariableInput::name()
{
	return _variable._name;
}

void VariableInput::setLabel(const QString& label, const VariableType variableType)
{
	_variable._label = label;
	switch(variableType)
	{
		case eUnknownVariableType:
		case eIntegerType:
		case eFloatType:
			_variableLabel->setText(label);
			break;
		case eBooleanType:
			_inputBoolean->setText(label);
	}
}

QString &VariableInput::label()
{
	return _variable._label;
}

void VariableInput::setTooltip(const QString &tooltip)
{
	_variable._tooltip = tooltip;
	_variableValue->setToolTip(tooltip);
}

QString &VariableInput::tooltip()
{
	return _variable._tooltip;
}

void VariableInput::setType(const VariableType variableType)
{
	_variableLabel->hide();
	_inputTypeUnknown->hide();
	_variableValue->hide();
	_inputBoolean->hide();

	_variable._type = variableType;
	switch(variableType)
	{
		case eUnknownVariableType:
			_variableLabel->setVisible(true);
			_inputTypeUnknown->setVisible(true);
			break;
		case eIntegerType:
		case eFloatType:
			_variableLabel->setVisible(true);
			_variableValue->setVisible(true);
			break;
		case eBooleanType:
			_variableHLayout->removeItem(_variableSpacer);
			_inputBoolean->setVisible(true);
			break;
	}
}

VariableType VariableInput::type()
{
	return _variable._type;
}

void VariableInput::setDefaultValue(const QVariant &value, const VariableType variableType)
{
	_variable._defaultValue = value;
	QString valueStr = value.toString();

	switch(variableType)
	{
		case eUnknownVariableType:
			break;
		case eIntegerType:
		case eFloatType:
			_variableValue->setValue(valueStr.toInt());
			break;
		case eBooleanType:
			Qt::CheckState state = (valueStr == "true")? Qt::Checked : Qt::Unchecked;
			_inputBoolean->setCheckState(state);
			break;
	}
}

QVariant VariableInput::defaultValue()
{
	return _variable._defaultValue;
}

void VariableInput::setValue(const QVariant &value)
{
	_variable._defaultValue = value;
}

QVariant VariableInput::value()
{
	return _variable._defaultValue;
}

void VariableInput::onSpinBoxValueChanged(double value)
{
	setDefaultValue(value, _variable._type);
	emit variableValueUpdated(_variable);
}
