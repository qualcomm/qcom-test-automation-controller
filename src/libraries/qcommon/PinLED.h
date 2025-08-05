#ifndef PINLED_H
#define PINLED_H
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
