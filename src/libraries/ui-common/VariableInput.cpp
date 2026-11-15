// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "VariableInput.h"
#include "QCommonTypes.h"

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
	_variable._name = toStdString(name);
}

QString VariableInput::name() const
{
	return toQString(_variable._name);
}

void VariableInput::setLabel(const QString& label, const VariableType variableType)
{
	_variable._label = toStdString(label);
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

QString VariableInput::label() const
{
	return toQString(_variable._label);
}

void VariableInput::setTooltip(const QString &tooltip)
{
	_variable._tooltip = toStdString(tooltip);
	_variableValue->setToolTip(tooltip);
}

QString VariableInput::tooltip() const
{
	return toQString(_variable._tooltip);
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
	_variable._defaultValue = toScriptVariableValue(value, variableType);
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
	return toQVariant(_variable._defaultValue);
}

void VariableInput::setValue(const QVariant &value)
{
	_variable._defaultValue = toScriptVariableValue(value, _variable._type);
}

QVariant VariableInput::value()
{
	return toQVariant(_variable._defaultValue);
}

void VariableInput::onSpinBoxValueChanged(double value)
{
	setDefaultValue(value, _variable._type);
	emit variableValueUpdated(_variable);
}
