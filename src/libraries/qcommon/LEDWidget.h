#ifndef LEDWIDGET_H
#define LEDWIDGET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/
#include "QCommonGlobal.h"

#include <QLabel>
#include <QPixmap>

class QCOMMON_EXPORT LEDWidget :
	public QLabel
{
	Q_OBJECT

public:
	LEDWidget(QWidget* parent = Q_NULLPTR);

	bool isOn();

	enum LedColor
	{
		eNotSet,
		eOff,
		eGreen,
		eBlue,
		eRed
	}							_ledColor;

	void setLedColor(LedColor ledColor);

	QSize sizeHint() const override;

signals:
	void mouseReleased(bool activeState);

protected:
	virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private:
	static bool					_initialized;
	static QPixmap*				_ledOff;
	static QPixmap*				_greenLED;
	static QPixmap*				_redLED;
	static QPixmap*				_blueLED;
};

#endif // LEDWIDGET_H
