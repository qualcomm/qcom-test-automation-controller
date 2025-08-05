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
