// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACFrame.h"

#include "ui_TACFrame.h"

#include "CommandButton.h"
#include "PSOCPlatformConfiguration.h"
#include "ScriptVariable.h"
#include "UserWidget.h"
#include "VariableInput.h"

// QCommon
#include "AppCore.h"
#include "Range.h"
#include "TACException.h"
#include "QCommonTypes.h"
#include "QAlpacaDevice.h"

// TAC

// QT
#include <QDir>
#include <QFileInfoList>
#include <QGroupBox>

#include <functional>
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
	shutDown();
	delete _alpacaDeviceBridge;
	_alpacaDeviceBridge = nullptr;
	delete _ui;
}

void TACFrame::shutDown()
{
	if (_alpacaDeviceBridge != nullptr)
	{
		_alpacaDeviceBridge->close();
	}
	if (_alpacaDevice != nullptr)
	{
		_alpacaDevice->onPinStateChanged = nullptr;
	}
}

QString TACFrame::name()
{
	if (_alpacaDeviceBridge != nullptr)
	{
		return _alpacaDeviceBridge->name();
	}

	return QString();
}

void TACFrame::setName(const QString& newName)
{
	if (_alpacaDeviceBridge != nullptr)
	{
		_alpacaDeviceBridge->setName(newName);
	}
}

void TACFrame::setDevice
(
	AlpacaDevice alpacaDevice
)
{
	_alpacaDevice = alpacaDevice;
	if (_alpacaDeviceBridge != nullptr)
	{
		delete _alpacaDeviceBridge;
		_alpacaDeviceBridge = nullptr;
	}

	if (alpacaDevice != nullptr)
	{
		_alpacaDeviceBridge = new QAlpacaDevice(alpacaDevice, this);
		connect(_alpacaDeviceBridge, &QAlpacaDevice::pinStateChanged, this, &TACFrame::onPinResponse);

		setPlatformConfiguration(alpacaDevice->platformConfiguration());

		_ui->_infoGroupBox->hide();
	}
}

QString TACFrame::portName()
{
	if (_alpacaDeviceBridge != nullptr)
	{
		return _alpacaDeviceBridge->portName();
	}

	return QString();
}

void TACFrame::onVariableValueUpdated(ScriptVariable scriptVariable)
{
	if (_alpacaDeviceBridge != nullptr)
	{
		QString variableName = toQString(scriptVariable._name);
		QString variableValue = toQVariant(scriptVariable._defaultValue).toString();
		_alpacaDeviceBridge->updateScriptVariableValue(variableName, variableValue);
	}
}

void TACFrame::setPlatformConfiguration
(
	PlatformConfiguration platformConfiguration
)
{
	Q_ASSERT(platformConfiguration != nullptr);

	_platformConfiguration = platformConfiguration;

	_ui->_resetFrame->setVisible(_platformConfiguration->getResetEnabledState());

	setupUITabs();
	setupUIPins();
	setupUII2CPins();
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
		QString tabName = toQString(tab._name).toLower();

		if ((tabName == "general" || tabName == "device info") == false)
		{
			UserWidget* userWidget = new UserWidget;
			userWidget->setProperty("tabkey", toQString(tab._name));
			_ui->_tabs->insertTab(999, userWidget, toQString(tab._name));
		}
	}
}

void TACFrame::setupUIPins()
{
	QString tabName;

	Pins pins = _platformConfiguration->getPins();

	for (const auto& pin: pins)
	{
		QString currentTab = toQString(pin._tabName);
		if (tabName != currentTab)
		{
			tabName = currentTab;

			populatePinLEDs(pins, tabName);
		}
	}
}

void TACFrame::setupUII2CPins()
{
	_PSOCPlatformConfiguration* psocConfig = dynamic_cast<_PSOCPlatformConfiguration*>(_platformConfiguration.get());
	if (psocConfig == Q_NULLPTR)
		return;

	QString tabName;

	Pins pins = psocConfig->getI2CPinEntries();

	for (const auto& pin: pins)
	{
		QString currentTab = toQString(pin._tabName);
		if (tabName != currentTab)
		{
			tabName = currentTab;

			populateI2CPinLEDs(pins, tabName);
		}
	}
}

void TACFrame::setupUIQuickSettings()
{
	QString tabName;

	ButtonList buttons = _platformConfiguration->getButtons();

	for (const auto& button: buttons)
	{
		QString currentTab = toQString(button._tab);
		if (tabName != currentTab)
		{
			tabName = currentTab;

			populateQuickSettingsButtons(buttons, tabName);
		}
	}
}

void TACFrame::setupUIVariables()
{
	ScriptVariables variables = _platformConfiguration->getVariables();

	for (const auto& [name, variable]: variables)
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
			for (const auto& pin: pins)
			{
				QPoint cellLoc = toQPoint(pin._cellLocation);
				if (cellLoc.y() < 0 || cellLoc.x() < 0)
					emit startNotification(QString("Cell location violation found for pin: %1").arg(pin._pin), eErrorNotification);

				else if (toQString(pin._tabName) == tabName)
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
							layout->addWidget(pinLED, cellLoc.y(), cellLoc.x(), 1, 1);

							pinLED->setText(toQString(pin._pinLabel));
							pinLED->setFont(font);
							pinLED->setEnabled(_alpacaDevice != nullptr);
							pinLED->setInverted(pin._inverted);
							pinLED->setInitialState(pin._initialValue);
							pinLED->setPinNumber(pin._pin, &_pinMap);
							pinLED->setToolTip(toQString(pin._pinTooltip));

							connect(pinLED, &PinLED::pinTriggered, this, &TACFrame::onPinTriggered);
						}
					}
				}
			}
		}
	}
}

void TACFrame::populateI2CPinLEDs(const Pins &pins, const QString &tabName)
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
			for (const auto& pin: pins)
			{
				QPoint cellLoc = toQPoint(pin._cellLocation);
				if (cellLoc.y() < 0 || cellLoc.x() < 0)
					emit startNotification(QString("Cell location violation found for pin: %1").arg(pin._pin), eErrorNotification);

				else if (toQString(pin._tabName) == tabName)
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
							layout->addWidget(pinLED, cellLoc.y(), cellLoc.x(), 1, 1);

							pinLED->setText(toQString(pin._pinLabel));
							pinLED->setFont(font);
							pinLED->setEnabled(_alpacaDevice != nullptr);
							pinLED->setInverted(pin._inverted);
							pinLED->setInitialState(pin._initialValue);
							pinLED->setPinNumber(pin._hash, &_i2cPinMap);
							pinLED->setToolTip(toQString(pin._pinTooltip));

							connect(pinLED, &PinLED::pinTriggered, this, &TACFrame::onI2CPinTriggered);
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
		QPoint cellLoc = toQPoint(button._cellLocation);
		if (cellLoc.y() < 0 || cellLoc.x() < 0)
			emit startNotification(QString("Cell location violation found for quick settings label: %1").arg(toQString(button._label)), eErrorNotification);

		else if (toQString(button._tab) == tabName)
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
					commandButton->setObjectName(QString("Command %1").arg(toQString(button._command)));
					layout->addWidget(commandButton, cellLoc.y(), cellLoc.x(), 1, 1);

					commandButton->setText(toQString(button._label));
					commandButton->setCommand(toQString(button._command));
					commandButton->setEnabled(_alpacaDevice != nullptr);
					commandButton->setToolTip(toQString(button._toolTip));

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
			QPoint cellLoc = toQPoint(variable._cellLocation);
			VariableInput* variableInput = new VariableInput(groupBox);
			variableInput->setObjectName(QString("Variable name: %1").arg(toQString(variable._name)));
			layout->addWidget(variableInput, cellLoc.y(), cellLoc.x());

			variableInput->setName(toQString(variable._name));
			variableInput->setLabel(toQString(variable._label), variable._type);
			variableInput->setTooltip(toQString(variable._tooltip));
			variableInput->setType(variable._type);
			variableInput->setDefaultValue(toQVariant(variable._defaultValue), variable._type);
			variableInput->setEnabled(_alpacaDevice != nullptr);
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
		QWidget* widget = _ui->_tabs->widget(i);
		QString tabKey = widget->property("tabkey").toString();

		if (tabKey.isEmpty())
			tabKey = _ui->_tabs->tabText(i);

		if (tabKey.toLower() == tabName.toLower())
		{
			result = widget;
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
	if (_alpacaDeviceBridge != nullptr)
	{
		try
		{
			AppCore::writeToApplicationLogLine("TACFrame::onPinTriggered(" + std::to_string(pin) + ")");
			_alpacaDeviceBridge->setPinState(pin, !state);
		}
		catch(TACException& e)
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

void TACFrame::onI2CPinTriggered
(
	HashType hash,
	bool state
)
{
	_PSOCPlatformConfiguration* psocConfig = dynamic_cast<_PSOCPlatformConfiguration*>(_platformConfiguration.get());
	if (psocConfig == Q_NULLPTR || _alpacaDeviceBridge == nullptr)
		return;

	PSOCI2CData i2cData = psocConfig->getI2CSlave(hash);

	QString i2cAddress = QString("0x%1 0x%2")
		.arg(i2cData._slaveAddress, 2, 16, QChar('0'))
		.arg(i2cData._writeAddress, 2, 16, QChar('0'));

	_alpacaDeviceBridge->setAddressPinState(i2cAddress, i2cData._pin, !state);
}

void TACFrame::onCommandTriggered(const QString &command)
{
	if (_alpacaDeviceBridge != nullptr)
	{
		_alpacaDeviceBridge->quickCommand(command);
	}
}

void TACFrame::on__tabs_tabBarClicked(int index)
{
	if (_platformConfiguration != nullptr)
	{
		// let's build up the Info page
		_ui->_hardwareType->setText(toQString(_platformConfiguration->getPlatformString()));

		if (_alpacaDeviceBridge != nullptr)
		{
			_ui->_hardwareVersion->setText(_alpacaDeviceBridge->hardwareVersionString());
			_ui->_firmwareVersion->setText(_alpacaDeviceBridge->firmwareVersion());
			_ui->_chipset->setText(_alpacaDeviceBridge->chipVersion());
			_ui->_deviceName->setText(_alpacaDeviceBridge->name());
			_ui->_deviceUUID->setText(_alpacaDeviceBridge->uuid());
			_ui->_serialNumber->setText(_alpacaDeviceBridge->serialNumber());
			_ui->_platformID->setText(QString::number(_alpacaDeviceBridge->platformID()));
			_ui->_macAddress->setText(_alpacaDeviceBridge->macAddress());
			_ui->_configurationFile->setText(toQString(_platformConfiguration->name()));
			_ui->_configurationDate->setText(toQString(_platformConfiguration->modificationDate()));
			_ui->_configurationFileVersion->setText(toQString(_platformConfiguration->getFileVersion()));
		}
	}
}
