// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "PinLED.h"

#include "ui_PinLED.h"


PinLED::PinLED
(
	QWidget* parent
) :
	QWidget(parent),
	_ui(new Ui::PinLEDClass)
{
	_ui->setupUi(this);

	connect(_ui->_led, &LEDWidget::mouseReleased, this, &PinLED::ledMouseRelease);
}

bool PinLED::isEnabled()
{
	return _ui->_led->isEnabled();
}

void PinLED::setEnabled(bool enabled)
{
	_ui->_led->setEnabled(enabled);
	_ui->_label->setEnabled(enabled);

	if (enabled == false)
	{
		_pinNumber = 0;
		_ui->_label->setToolTip("");
		_ui->_led->setLedColor(LEDWidget::eOff);
	}
}

bool PinLED::getState()
{
	return _state;
}

void PinLED::setInverted(bool invertState)
{
	_invertedState = invertState;
}

void PinLED::setPinNumber
(
	PinID pinNumber,
	PinMap* pinMap // = Q_NULLPTR
)
{
	_pinNumber = pinNumber;
	if (pinMap != Q_NULLPTR)
	{
		pinMap->insert(pinNumber, this);
	}
}

void PinLED::setText(const QString &labelText)
{
	_ui->_label->setText(labelText);
}

void PinLED::setFont(const QFont &font)
{
	_ui->_label->setFont(font);
}

void PinLED::setTooltip(const QString &tooltipText)
{
	_ui->_label->setToolTip(tooltipText);
}

void PinLED::setState(bool state)
{
	_state = state;

	if (_invertedState == false)
		_ui->_led->setLedColor(_state ? LEDWidget::eGreen : LEDWidget::eOff);
	else
		_ui->_led->setLedColor(_state ? LEDWidget::eOff : LEDWidget::eGreen);
}

void PinLED::ledMouseRelease
(
	bool state
)
{
	if (isEnabled())
	{
		emit pinTriggered(_pinNumber, _state);
	}

	_state = state;
}

