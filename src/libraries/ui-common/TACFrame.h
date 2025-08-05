#ifndef TACFRAME_H
#define TACFRAME_H

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
#include "UIGlobalLib.h"

namespace Ui {
	class TACFrameClass;
} // namespace Ui

// libTAC
#include "PlatformConfiguration.h"

// QCommon
#include "Button.h"
#include "PinLED.h"
#include "AlpacaDevice.h"

// Qt
#include <QGroupBox>
#include <QWidget>

class UILIB_EXPORT TACFrame :
	public QWidget
{
Q_OBJECT

public:
	TACFrame(QWidget* parent = Q_NULLPTR);
	~TACFrame();

	// use for open device setup
	void setDevice(AlpacaDevice alpacaDevice);

	// use for static, non connected setup
	void setPlatformConfiguration(PlatformConfiguration platformConfiguration);
	void reset();

	void shutDown();

	QString name();
	void setName(const QString& newName);
	
	QString portName();

signals:
	void consoleMessagesReady(const QList<QByteArray>& consoleMessages);

public slots:
	void onPinTriggered(PinID pin, bool state);
	void onPinResponse(/*PinID*/ quint64 pin, bool state);

	void onCommandTriggered(const QString& command);

	void onVariableValueUpdated(ScriptVariable scriptVariable);

private slots:
	void on__tabs_tabBarClicked(int index);

private:
	void setupUITabs();
	void setupUIPins();
	void setupUIQuickSettings();
	void setupUIVariables();
	void cleanupEmptyGroupBoxes();

	void populateVariables(const ScriptVariable &variable);
	void populatePinLEDs(const Pins& pins, const QString& tabName);
	void populateQuickSettingsButtons(const ButtonList& buttons, const QString& tabName);

	QWidget* getTabWidget(const QString& tabName);

	Ui::TACFrameClass*			_ui{Q_NULLPTR};
	PinMap						_pinMap;
	AlpacaDevice				_alpacaDevice;
	PlatformConfiguration		_platformConfiguration;
};

#endif // TACFRAME_H
