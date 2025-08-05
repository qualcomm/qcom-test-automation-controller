#ifndef TACFRAME_H
#define TACFRAME_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
