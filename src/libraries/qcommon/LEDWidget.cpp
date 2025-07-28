// Confidential and Proprietary Qualcomm Technologies, Inc.

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
// Copyright 2013-2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "LEDWidget.h"

// QT
#include <QVariant>

bool LEDWidget::_initialized(false);

QPixmap* LEDWidget::_ledOff(Q_NULLPTR);
QPixmap* LEDWidget::_greenLED(Q_NULLPTR);
QPixmap* LEDWidget::_redLED(Q_NULLPTR);
QPixmap* LEDWidget::_blueLED(Q_NULLPTR);

LEDWidget::LEDWidget
(
	QWidget* parent
) :
	QLabel(parent),
	_ledColor(eNotSet)
{
	QSizePolicy policy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	setSizePolicy(policy);
	setScaledContents(true);

	if (LEDWidget::_initialized == false)
	{
		LEDWidget::_ledOff = new QPixmap(":/resources/LedOff.png");
		LEDWidget::_greenLED = new QPixmap(":/resources/GreenLed.png");
		LEDWidget::_redLED = new QPixmap(":/resources/RedLed.png");
		LEDWidget::_blueLED = new QPixmap(":/resources/BlueLed.png");
	}

	setLedColor(LedColor::eOff);
}

bool LEDWidget::isOn()
{
	return property("state").toBool();
}

QSize LEDWidget::sizeHint() const
{
	return QWidget::sizeHint();
}

void LEDWidget::mouseReleaseEvent
(
	QMouseEvent *releaseEvent
)
{
	Q_UNUSED(releaseEvent);

	if (isEnabled())
		emit mouseReleased(_ledColor != LedColor::eOff);
}

void LEDWidget::setLedColor
(
	LedColor ledColor
)
{
	if (ledColor != _ledColor)
	{
		_ledColor = ledColor;
		QPixmap qPixMap;

		switch (_ledColor)
		{
		case LedColor::eGreen:
			qPixMap = *LEDWidget::_greenLED;
			setProperty("state", QVariant(true));
			break;

		case LedColor::eRed:
			qPixMap = *LEDWidget::_redLED;
			setProperty("state", QVariant(true));
			break;

		case LedColor::eBlue:
			qPixMap = *LEDWidget::_blueLED;
			setProperty("state", QVariant(true));
			break;

		case LedColor::eOff:
		default:
			qPixMap = *LEDWidget::_ledOff;
			setProperty("state", QVariant(false));
			break;
		}

		setPixmap(qPixMap);
		update();
	}
}
