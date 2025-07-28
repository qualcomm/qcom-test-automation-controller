#ifndef PINLED_H
#define PINLED_H
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
// Copyright ©2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
