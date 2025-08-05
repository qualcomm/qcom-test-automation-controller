#ifndef PINLED_H
#define PINLED_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "QCommonGlobal.h"

#include "PinID.h"

#include <QFont>
#include <QMap>
#include <QWidget>

class PinLED;

typedef QMap<PinID, PinLED*> PinMap;

namespace Ui {
	class PinLEDClass;
}

class QCOMMON_EXPORT PinLED :
	public QWidget
{
Q_OBJECT

public:
	PinLED(QWidget* parent);

	bool isEnabled();
	void setEnabled(bool enabled = true);

	void setInitialState(bool initialState)
	{
		_initialState = initialState;
		setState(_initialState);
	}
	bool getInitialState()
	{
		return _initialState;
	}

	bool getState();
	void setInverted(bool invertState);

	void setPinNumber(PinID pinNumber, PinMap* pinMap = Q_NULLPTR);
	void setText(const QString& labelText);
	void setFont(const QFont& font);
	void setTooltip(const QString& tooltipText);

signals:
	void pinTriggered(PinID pinNumber, bool state);

public slots:
	void setState(bool state);
	void ledMouseRelease(bool state);

private:
	Ui::PinLEDClass*			_ui{Q_NULLPTR};
	bool						_state{false};
	PinID						_pinNumber{0};
	bool						_invertedState{false};
	bool						_initialState{false};
};


#endif // PINLED_H
