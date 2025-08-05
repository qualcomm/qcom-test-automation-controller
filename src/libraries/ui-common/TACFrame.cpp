// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

//	Author: Michael Simpson (msimpson@qti.qualcomm.com)
//			Biswajit Roy (biswroy@qti.qualcomm.com)

#include "TACFrame.h"

#include "ui_TACFrame.h"

#include "CommandButton.h"
#include "I2CWidget.h"
#include "ScriptVariable.h"
#include "TerminalWidget.h"
#include "UserWidget.h"
#include "VariableInput.h"

// QCommon
#include "AppCore.h"
#include "Range.h"
#include "TacException.h"

// TAC

// QT
#include <QDir>
#include <QFileInfoList>
#include <QGroupBox>
#include <QInputDialog>
#include <QList>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPair>
#include <QProcess>
#include <QScrollBar>

const QString kButtonsGroupBoxName(QStringLiteral("_buttonsGroupBox"));
const QString kConnectionsGroupBoxName(QStringLiteral("_connectionsGroupBox"));
const QString kSwitchesGroupBoxName(QStringLiteral("_switchesGroupBox"));
const QString kQuickSettingsGroupBoxName(QStringLiteral("_quickSettingsGroupBox"));
const QString kVariablesGroupBoxName(QStringLiteral("_variablesGroupBox"));

TACFrame::TACFrame
(
	QWidget* parent
) :
	QWidget(parent),
	_ui(new Ui::TACFrameClass)
{
	_ui->setupUi(this);

	QGridLayout* gridLayout;

	gridLayout = new QGridLayout(_ui->_connectionsGroupBox);
	gridLayout->setObjectName(QString::fromUtf8("ConnectionsLayout"));

	gridLayout = new QGridLayout(_ui->_buttonsGroupBox);
	gridLayout->setObjectName(QString::fromUtf8("ButtonsLayout"));

	gridLayout = new QGridLayout(_ui->_switchesGroupBox);
	gridLayout->setObjectName(QString::fromUtf8("SwitchesLayout"));

	gridLayout = new QGridLayout(_ui->_quickSettingsGroupBox);
	gridLayout->setObjectName(QString::fromUtf8("QuickSettingsLayout"));

	gridLayout = new QGridLayout(_ui->_variablesGroupBox);
	gridLayout->setObjectName(QString::fromUtf8("VariablesLayout"));

	_ui->General->setProperty("usertab", true);
}

TACFrame::~TACFrame()
{
	delete _ui;
}

void TACFrame::setDevice
(
	AlpacaDevice alpacaDevice
)
{
	if (alpacaDevice.isNull() == false)
	{
		_alpacaDevice = alpacaDevice;

		connect(_alpacaDevice.data(), &_AlpacaDevice::pinStateChanged, this, &TACFrame::onPinResponse);

		setPlatformConfiguration(alpacaDevice->platformConfiguration());
	}
	{
		// error message????
	}
}

QString TACFrame::portName()
{
	if (_alpacaDevice.isNull() == false)
	{
		return _alpacaDevice->portName();
	}

	return QByteArray();
}

void TACFrame::onVariableValueUpdated(ScriptVariable scriptVariable)
{
	if (_alpacaDevice.isNull() == false)
	{
		QByteArray variableName = scriptVariable._name.toLatin1();
		QByteArray variableValue = scriptVariable._defaultValue.toString().toLatin1();
		_alpacaDevice->updateScriptVariableValue(variableName, variableValue);
	}
}

void TACFrame::setPlatformConfiguration
(
	PlatformConfiguration platformConfiguration
)
{
	Q_ASSERT(platformConfiguration.isNull() == false);

	_ui->_infoGroupBox->hide();

	_platformConfiguration = platformConfiguration;

	_ui->_resetFrame->setVisible(_platformConfiguration->getResetEnabledState());

	setupUITabs();
	setupUIPins();
	setupUIQuickSettings();
	setupUIVariables();

	cleanupEmptyGroupBoxes();
}

void TACFrame::reset()
{
	// todo!!!
	if (_ui != Q_NULLPTR)
	{
		QObjectList children = this->children();
		for (const auto& child: children)
		{
			child->deleteLater();
		}

		delete _ui;
		_ui = Q_NULLPTR;
	}

	_ui = new Ui::TACFrameClass;
	_ui->setupUi(this);
	this->repaint();
}

void TACFrame::setupUITabs()
{
	Tabs tabs = _platformConfiguration->getTabs();
	for (const auto& tab: tabs)
	{
		QString tabName = tab._name.toLower();

		if ((tabName == "general" || tabName == "device info") == false)
		{
			if (tabName == "i2c")
			{
				I2CWidget* i2cWidget = new I2CWidget;

				_ui->_tabs->insertTab(999, i2cWidget, tab._name);
				i2cWidget->setEnabled(_alpacaDevice.isNull() == false);
			}
			else if (tabName == "terminal")
			{
				TerminalWidget* terminalWidget = new TerminalWidget;

				_ui->_tabs->insertTab(999, terminalWidget, tab._name);
				terminalWidget->setEnabled(_alpacaDevice.isNull() == false);
			}
			else
			{
				UserWidget* userWidget = new UserWidget;

				_ui->_tabs->insertTab(999, userWidget, tab._name);
			}
		}
	}
}

void TACFrame::setupUIPins()
{
	QString tabName;

	Pins pins = _platformConfiguration->getPins();

	for (const auto& pin: pins)
	{
		if (tabName != pin._tabName)
		{
			tabName = pin._tabName;

			populatePinLEDs(pins, tabName);
		}
	}
}

void TACFrame::setupUIQuickSettings()
{
	QString tabName;

	ButtonList buttons = _platformConfiguration->getButtons();

	for (const auto& button: buttons)
	{
		if (tabName != button._tab)
		{
			tabName = button._tab;

			populateQuickSettingsButtons(buttons, tabName);
		}
	}
}

void TACFrame::setupUIVariables()
{
	QString tabName;

	ScriptVariables variables = _platformConfiguration->getVariables();

	for (const auto& variable: variables)
		populateVariables(variable);
}

void TACFrame::cleanupEmptyGroupBoxes()
{
	for (auto i: range(_ui->_tabs->count()))
	{
		QGroupBox* groupBox;

		QWidget* tabWidget = _ui->_tabs->widget(i);
		if (tabWidget != Q_NULLPTR)
		{
			bool userTab = tabWidget->property("usertab").toBool();
			if (userTab == true)
			{
				groupBox = tabWidget->findChild<QGroupBox*>(kConnectionsGroupBoxName, Qt::FindDirectChildrenOnly);
				if (groupBox != Q_NULLPTR)
				{
					int childCount = groupBox->findChildren<PinLED*>().count();
					if (childCount == 0)
					{
						groupBox->setVisible(false);
					}
				}

				groupBox = tabWidget->findChild<QGroupBox*>(kButtonsGroupBoxName, Qt::FindDirectChildrenOnly);
				if (groupBox != Q_NULLPTR)
				{
					int childCount = groupBox->findChildren<PinLED*>().count();
					if (childCount == 0)
					{
						groupBox->setVisible(false);
					}
				}

				groupBox = tabWidget->findChild<QGroupBox*>(kSwitchesGroupBoxName, Qt::FindDirectChildrenOnly);
				if (groupBox != Q_NULLPTR)
				{
					int childCount = groupBox->findChildren<PinLED*>().count();
					if (childCount == 0)
					{
						groupBox->setVisible(false);
					}
				}

				groupBox = tabWidget->findChild<QGroupBox*>(kQuickSettingsGroupBoxName, Qt::FindDirectChildrenOnly);
				if (groupBox != Q_NULLPTR)
				{
					int childCount = groupBox->findChildren<CommandButton*>().count();
					if (childCount == 0)
					{
						groupBox->setVisible(false);
					}
				}
				groupBox = tabWidget->findChild<QGroupBox*>(kVariablesGroupBoxName, Qt::FindDirectChildrenOnly);
				if (groupBox != Q_NULLPTR)
				{
					int childCount = groupBox->findChildren<VariableInput*>().count();
					if (childCount == 0)
					{
						groupBox->setVisible(false);
					}
				}
			}
		}
	}
}

void TACFrame::populatePinLEDs(const Pins &pins, const QString &tabName)
{
	QWidget* tabWidget{getTabWidget(tabName)};
	QGroupBox* groupBox{Q_NULLPTR};

	QFont font;
	font.setPointSize(8);
	font.setBold(false);

	if (tabWidget != Q_NULLPTR)
	{
		bool userTab = tabWidget->property("usertab").toBool();
		if (userTab == true)
		{
			for (const auto& pin: std::as_const(pins))
			{
				if (pin._tabName == tabName)
				{
					switch (pin._commandGroup)
					{
					case eConnectionGroup:
						groupBox = tabWidget->findChild<QGroupBox*>(kConnectionsGroupBoxName, Qt::FindDirectChildrenOnly);
						break;

					case eButtonGroup:
						groupBox = tabWidget->findChild<QGroupBox*>(kButtonsGroupBoxName, Qt::FindDirectChildrenOnly);
						break;

					case eSwitchGroup:
						groupBox = tabWidget->findChild<QGroupBox*>(kSwitchesGroupBoxName, Qt::FindDirectChildrenOnly);
						break;

					default:
						break;
					}

					if (groupBox != Q_NULLPTR)
					{
						QGridLayout* layout = qobject_cast<QGridLayout*>(groupBox->layout());
						if (layout != Q_NULLPTR)
						{
							PinLED* pinLED = new PinLED(groupBox);
							pinLED->setObjectName(QString("Pin %1").arg(pin._pin));
							layout->addWidget(pinLED, pin._cellLocation.y(), pin._cellLocation.x(), 1, 1);

							pinLED->setText(pin._pinLabel);
							pinLED->setFont(font);
							pinLED->setEnabled(_alpacaDevice.isNull() == false);
							pinLED->setInverted(pin._inverted);
							pinLED->setInitialState(pin._initialValue);
							pinLED->setPinNumber(pin._pin, &_pinMap);
							pinLED->setToolTip(pin._pinTooltip);

							connect(pinLED, &PinLED::pinTriggered, this, &TACFrame::onPinTriggered);
						}
					}
				}
			}
		}
	}
}

void TACFrame::populateQuickSettingsButtons
(
	const ButtonList& buttons,
	const QString& tabName
)
{	QWidget* tabWidget{getTabWidget(tabName)};
	QGroupBox* groupBox{Q_NULLPTR};

	for (const auto& button: buttons)
	{
		if (button._tab == tabName)
		{
			switch (button._commandGroup)
			{
			case eConnectionGroup:
				groupBox = tabWidget->findChild<QGroupBox*>(kConnectionsGroupBoxName, Qt::FindDirectChildrenOnly);
				break;

			case eButtonGroup:
				groupBox = tabWidget->findChild<QGroupBox*>(kButtonsGroupBoxName, Qt::FindDirectChildrenOnly);
				break;

			case eSwitchGroup:
				groupBox = tabWidget->findChild<QGroupBox*>(kSwitchesGroupBoxName, Qt::FindDirectChildrenOnly);
				break;

			case eQuickSettingsGroup:
				groupBox = tabWidget->findChild<QGroupBox*>(kQuickSettingsGroupBoxName, Qt::FindDirectChildrenOnly);
				break;

			default:
				break;
			}

			if (groupBox != Q_NULLPTR)
			{
				QGridLayout* layout = qobject_cast<QGridLayout*>(groupBox->layout());
				if (layout != Q_NULLPTR)
				{
					CommandButton* commandButton = new CommandButton(groupBox);
					commandButton->setObjectName(QString("Command %1").arg(button._command));
					layout->addWidget(commandButton, button._cellLocation.y(), button._cellLocation.x(), 1, 1);

					commandButton->setText(button._label);
					commandButton->setCommand(button._command);
					commandButton->setEnabled(_alpacaDevice.isNull() == false);
					commandButton->setToolTip(button._toolTip);

					connect(commandButton, &CommandButton::commandTriggered, this, &TACFrame::onCommandTriggered);
				}
			}
		}
	}
}

void TACFrame::populateVariables(const ScriptVariable &variable)
{
	// Variables will be available only under the General tab
	QWidget* tabWidget{getTabWidget("General")};
	QGroupBox* groupBox = tabWidget->findChild<QGroupBox*>(kVariablesGroupBoxName, Qt::FindDirectChildrenOnly);

	if (groupBox != Q_NULLPTR)
	{
		QGridLayout* layout = qobject_cast<QGridLayout*>(groupBox->layout());
		if (layout != Q_NULLPTR)
		{
			VariableInput* variableInput = new VariableInput(groupBox);
			variableInput->setObjectName(QString("Variable name: %1").arg(variable._name));
			layout->addWidget(variableInput, variable._cellLocation.y(), variable._cellLocation.x());

			variableInput->setName(variable._name);
			variableInput->setLabel(variable._label, variable._type);
			variableInput->setToolTip(variable._tooltip);
			variableInput->setType(variable._type);
			variableInput->setDefaultValue(variable._defaultValue, variable._type);
			variableInput->setEnabled(_alpacaDevice.isNull() == false);
			connect(variableInput, &VariableInput::variableValueUpdated, this, &TACFrame::onVariableValueUpdated);
		}
	}
}

QWidget* TACFrame::getTabWidget(const QString& tabName)
{
	QWidget* result{Q_NULLPTR};

	int tabCount = _ui->_tabs->count();
	for (const int i: range(tabCount))
	{
		QString tabText = _ui->_tabs->tabText(i);
		if (tabText.toLower() == tabName.toLower())
		{
			result = _ui->_tabs->widget(i);
			break;
		}
	}

	return result;
}

void TACFrame::onPinTriggered
(
	PinID pin,
	bool state
)
{
	if (_alpacaDevice != Q_NULLPTR)
	{
		try
		{
			AppCore::writeToApplicationLogLine("TACFrame::onPinTriggered(" + QString::number(pin) +")");
			_alpacaDevice->setPinState(pin, !state);
		}
		catch(TacException& e)
		{
			if (e.errorCode() == TAC_DEVICE_INACTIVE)
			{
				QMessageBox::critical(this, "TAC Device disconnected", "TAC was unable to communicate with the TAC device. "
																	   "The TAC device may have been disconnected from host. "
																	   "Reconnect to the TAC device to fix the problem");
			}

			AppCore::writeToApplicationLogLine("TACFrame::onPinTriggered: error while setting pin state for TAC device. Was TAC device disconnected?");
		}
	}
}

void TACFrame::onPinResponse
(
	quint64 pin,
	bool state
)
{
	if (_pinMap.find(pin) != _pinMap.end())
		_pinMap[pin]->setState(state);
}

void TACFrame::onCommandTriggered(const QString &command)
{
	if (_alpacaDevice.isNull() == false)
	{
		_alpacaDevice->quickCommand(command.toLatin1());
	}
}

void TACFrame::on__tabs_tabBarClicked(int index)
{
	if (_platformConfiguration != Q_NULLPTR)
	{
		// let's build up the Info page
		_ui->_hardwareType->setText(_platformConfiguration->getPlatformString());

		if (_alpacaDevice.isNull() == false)
		{
			_ui->_hardwareVersion->setText(_alpacaDevice->hardwareVersionString());
			_ui->_firmwareVersion->setText(_alpacaDevice->firmwareVersion());
			_ui->_chipset->setText(_alpacaDevice->chipVersion());
			_ui->_deviceName->setText(_alpacaDevice->name());
			_ui->_deviceUUID->setText(_alpacaDevice->uuid());
			_ui->_serialNumber->setText(_alpacaDevice->serialNumber());
			_ui->_platformID->setText(QString::number(_alpacaDevice->platformID()));
			_ui->_macAddress->setText(_alpacaDevice->macAddress());
			_ui->_configurationFile->setText(_platformConfiguration->name());
			_ui->_configurationDate->setText(_platformConfiguration->modificationDate());
			_ui->_configurationFileVersion->setText(_platformConfiguration->getFileVersion());
		}
	}
}
