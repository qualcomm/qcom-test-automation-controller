#ifndef TACFRAME_H
#define TACFRAME_H

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
// Copyright ©2018 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
