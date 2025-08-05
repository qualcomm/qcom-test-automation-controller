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

