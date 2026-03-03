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
// Copyright 2022-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "PSOCPlatformConfiguration.h"

#include "PlatformConfigurationException.h"

// QCommon
#include "Range.h"
#include "RangedContainer.h"

// Qt
#include <QJsonObject>
#include <QJsonArray>

const QString kPlatformEntries(QStringLiteral("pins"));
const QString kTabOrderData(QStringLiteral("tabs"));

const bool kDefaultInitialPinValue(false);
const bool kDefaultPinInvertedState(false);
const CommandGroups kDefaultCommandGroup{eUnknownCommandGroup};
const QPoint kDefaultCellLocation(QPoint(-1,-1));

const QString kRunPriority(QStringLiteral("run_priority"));
const QString kPinNumber(QStringLiteral("pin_number"));
const QString kEnabled(QStringLiteral("enabled"));
const QString kIntialValue(QStringLiteral("initial_value"));
const QString kPriority(QStringLiteral("initialization_priority"));
const QString kInverted(QStringLiteral("inverted"));
const QString kName(QStringLiteral("name"));
const QString kToolTip(QStringLiteral("help_hint"));
const QString kCommand(QStringLiteral("command"));
const QString kCommmandGroup(QStringLiteral("command_group"));
const QString kClassicAction(QStringLiteral("classic_action"));
const QString kTabName(QStringLiteral("tab_name"));
const QString kMinFirmwareVersion(QStringLiteral("supportedFirmwareVer"));

PSOCPinEntries _PSOCPlatformConfiguration::_classicActions;

_PSOCPlatformConfiguration::_PSOCPlatformConfiguration()
{
	_platform = ePSOC;
	_platformId = kMaxPSOCPlatformId;

	_resetActive = true;

	if (_classicActions.empty())
		_PSOCPlatformConfiguration::initialize();

	for (auto [pinNumber, pinData] : RangedContainer(_PSOCPlatformConfiguration::_classicActions))
		_pinEntries[pinData._pin] = pinData;

	// Add default tabs to _editorTabs
	Tab generalTab, deviceInfoTab, i2cTab, terminalTab, fusionTab;

	generalTab._name = "General";
	generalTab._moveable = false;
	generalTab._configurable = true;
	generalTab._ordinal = 0;
	generalTab._userTab = false;

	deviceInfoTab._name = "Device Info";
	deviceInfoTab._moveable = false;
	deviceInfoTab._configurable = false;
	deviceInfoTab._ordinal = 1;
	deviceInfoTab._userTab = false;

	i2cTab._name = "I2C";
	i2cTab._moveable = true;
	i2cTab._visible = true;
	i2cTab._configurable = false;
	i2cTab._ordinal = 2;
	i2cTab._userTab = true;

	fusionTab._name = "Fusion";
	fusionTab._moveable = true;
	fusionTab._configurable = true;
	fusionTab._ordinal = 3;
	fusionTab._userTab = true;

	terminalTab._name = "Terminal";
	terminalTab._moveable = true;
	terminalTab._configurable = false;
	terminalTab._ordinal = 4;
	terminalTab._userTab = false;

	_tabs.append(generalTab);
	_tabs.append(deviceInfoTab);
	_tabs.append(i2cTab);
	_tabs.append(fusionTab);
	_tabs.append(terminalTab);

	_supportedFirmwareVer.append(kDefaultFirmwareVersion);
}

_PSOCPlatformConfiguration::~_PSOCPlatformConfiguration()
{
}

Pins _PSOCPlatformConfiguration::getPins()
{
	Pins result;

	PSOCPinList psocPinList = getActivePins();

	for (const auto& psocPin: psocPinList)
	{
		PinEntry pinEntry;

		pinEntry._pin = psocPin._pin;
		pinEntry._enabled = psocPin._enabled;
		pinEntry._hash = psocPin._hash;
		pinEntry._cellLocation = psocPin._cellLocation;
		pinEntry._commandGroup = psocPin._commandGroup;
		pinEntry._pinCommand = psocPin._pinCommand;
		pinEntry._initialValue = psocPin._initialValue;
		pinEntry._inverted = psocPin._inverted;
		pinEntry._pinLabel = psocPin._pinLabel;
		pinEntry._pinTooltip = psocPin._pinTooltip;
		pinEntry._tabName = psocPin._tabName;

		result.push_back(pinEntry);
	}

	auto sortLambda = [] (PinEntry& p1, PinEntry& p2) -> bool
	{
		if (p1._tabName != p2._tabName)
			return p1._tabName < p2._tabName;

		if (p1._commandGroup != p2._commandGroup)
			return p1._commandGroup < p2._commandGroup;

		return (p1._pin < p2._pin);
	};

	std::sort(result.begin(), result.end(), sortLambda);
	return result;
}

PSOCPinList _PSOCPlatformConfiguration::getAllPins()
{
	PSOCPinList pinList;

	for (const auto& pinData: std::as_const(_pinEntries))
	{
		pinList.append(pinData);
	}

	auto sortLambda = [] (PSOCPinData p1, PSOCPinData& p2) -> bool
	{
		if (p1._pin < p2._pin)
			return true;

		return false;
	};

	std::sort(pinList.begin(), pinList.end(), sortLambda);

	return pinList;
}

PSOCPinList _PSOCPlatformConfiguration::getActivePins()
{
	PSOCPinList pinList;

	for (const auto& pinData: std::as_const(_pinEntries))
	{
		if (pinData._enabled == true)
			pinList.append(pinData);
	}

	auto sortLambda = [] (PSOCPinData p1, PSOCPinData& p2) -> bool
	{
		if (p1._pin < p2._pin)
			return true;

		return false;
	};

	std::sort(pinList.begin(), pinList.end(), sortLambda);

	return pinList;
}

bool _PSOCPlatformConfiguration::getPinEnableState(const PinID pinId) const
{
	bool result{true};
	if (_pinEntries.find(pinId) !=  _pinEntries.end())
	{
		result = _pinEntries.value(pinId)._enabled;
	}

	return result;
}

void _PSOCPlatformConfiguration::setPinEnableState(const PinID pinId, bool newState)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		if (_pinEntries[pinId]._enabled != newState)
		{
			_pinEntries[pinId]._enabled = newState;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set initial pin value for an invalid pin!");
	}
}

bool _PSOCPlatformConfiguration::getInitialPinValue(const PinID pinId) const
{
	bool result{kDefaultInitialPinValue};

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._initialValue;

	return result;
}

void _PSOCPlatformConfiguration::setInitialPinValue(const PinID pinId, bool newState)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		if (_pinEntries[pinId]._initialValue != newState)
		{
			_pinEntries[pinId]._initialValue = newState;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set tooltip for an invalid pin!");
	}
}

quint64 _PSOCPlatformConfiguration::getPinInitializationPriority(const PinID pinId) const
{
	PinID result{pinId};

	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		result = _pinEntries.value(pinId)._initializationPriority;
	}

	return result;
}

void _PSOCPlatformConfiguration::setPinInitializationPriority(const PinID pinId, const int priority)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		if (_pinEntries[pinId]._initializationPriority != priority)
		{
			_pinEntries[pinId]._initializationPriority = priority;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set initial priority value for an invalid pin!");
	}
}

QString _PSOCPlatformConfiguration::getPinLabel(const PinID pinId) const
{
	QString result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._pinLabel;

	return result;
}

void _PSOCPlatformConfiguration::setPinLabel(const PinID pinId, const QString& pinLabel)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		_pinEntries[pinId]._pinLabel = pinLabel;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set pin label for an invalid pin!");
	}
}

QString _PSOCPlatformConfiguration::getPinTooltip(const PinID pinId) const
{
	QString result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._pinTooltip;

	return result;
}

void _PSOCPlatformConfiguration::setPinTooltip(const PinID pinId, const QString& pinTooltip)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		_pinEntries[pinId]._pinTooltip = pinTooltip;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set tooltip for an invalid pin!");
	}
}

bool _PSOCPlatformConfiguration::getPinInvertedState(const PinID pinId) const
{
	bool result{kDefaultPinInvertedState};

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._inverted;

	return result;
}

void _PSOCPlatformConfiguration::setPinInvertedState(const PinID pinId, bool newState)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		_pinEntries[pinId]._inverted = newState;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set invert state for an invalid pin!");
	}
}

QString _PSOCPlatformConfiguration::getPinCommand(const PinID pinId) const
{
	QString result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._pinCommand;

	return result;
}

void _PSOCPlatformConfiguration::setPinCommand(const PinID pinId, const QString& pinCommand)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		_pinEntries[pinId]._pinCommand = pinCommand;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set invert state for an invalid pin!");
	}
}

CommandGroups _PSOCPlatformConfiguration::getPinGroup(const PinID pinId) const
{
	CommandGroups result{kDefaultCommandGroup};

   if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._commandGroup;

	return result;
}

void _PSOCPlatformConfiguration::setPinGroup
(
	const PinID pinId,
	const CommandGroups commandGroup
)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		_pinEntries[pinId]._commandGroup = commandGroup;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set pin category for an invalid pin!");
	}
}

QString _PSOCPlatformConfiguration::getClassicAction(const PinID pinId) const
{
	QString result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._classicAction;

	return result;
}

void _PSOCPlatformConfiguration::setClassicAction(const PinID pinId, const QString& classicAction)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		_pinEntries[pinId]._classicAction = classicAction;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set classic action for an invalid pin!");
	}
}

QPoint _PSOCPlatformConfiguration::getPinCellLocation(const PinID pinId) const
{
	QPoint result{kDefaultCellLocation};

	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		result = _pinEntries[pinId]._cellLocation;
	}

	return result;
}

void _PSOCPlatformConfiguration::setPinCellLocation(const PinID pinId, const QPoint& cellLocation)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		_pinEntries[pinId]._cellLocation = cellLocation;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set classic action for an invalid pin!");
	}
}

void _PSOCPlatformConfiguration::cascadeTabDelete(const QString &deleteMe)
{
	for (int idx=0; idx<_tabs.length(); idx++)
	{
		if (_tabs.at(idx)._name == deleteMe)
		{
			_tabs.removeAt(idx);
		}
	}

	for (auto& pinEntry: _pinEntries)
	{
		if (pinEntry._tabName == deleteMe)
			pinEntry._tabName = QString();
	}
}

void _PSOCPlatformConfiguration::cascadeTabRename
(
	const QString& oldName,
	const QString& newName
)
{
	for (auto& pinEntry: _pinEntries)
	{
		if (pinEntry._tabName == oldName)
			pinEntry._tabName = newName;
	}
}

QString _PSOCPlatformConfiguration::getTabName(const PinID pinId) const
{
	QString result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		result = _pinEntries[pinId]._tabName;
	}

	return result;
}

void _PSOCPlatformConfiguration::setTabName(const PinID pinId, const QString pinTab)
{
	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		_pinEntries[pinId]._tabName = pinTab;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set invert state for an invalid pin!");
	}
}

bool _PSOCPlatformConfiguration::read(QJsonObject &parentLevel)
{
	bool result{false};
	QJsonValue jsonValue;

	result = _PlatformConfiguration::read(parentLevel);

	jsonValue = parentLevel.value(kMinFirmwareVersion);

	_supportedFirmwareVer.clear();
	if (jsonValue.isNull() == false)
	{
		if (jsonValue.isArray() == true)
		{
			for (auto fwVer : jsonValue.toArray())
				_supportedFirmwareVer.append(static_cast<quint32>(fwVer.toInt()));
		}

		if (_supportedFirmwareVer.size() == 0)
			_supportedFirmwareVer.append(kDefaultFirmwareVersion);
	}

	QJsonArray pinDataArray = parentLevel.value(kPlatformEntries).toArray();

	for (auto pinIndex: range(pinDataArray.count()))
	{
		QJsonObject pinEntry = pinDataArray.at(pinIndex).toObject();

		PSOCPinData pinData;

		if (pinEntry.contains(kPinNumber))
			pinData._pin = pinEntry.value(kPinNumber).toString().toULongLong();

		if (pinEntry.contains(kEnabled))
			pinData._enabled = pinEntry.value(kEnabled).toBool();

		if (pinEntry.contains(kIntialValue))
			pinData._initialValue = pinEntry.value(kIntialValue).toBool();

		if (pinEntry.contains(kPriority))
			pinData._initializationPriority = pinEntry.value(kPriority).toInt();

		if (pinEntry.contains(kInverted))
			pinData._inverted = pinEntry.value(kInverted).toBool();

		if (pinEntry.contains(kName))
			pinData._pinLabel = pinEntry.value(kName).toString();

		if (pinEntry.contains(kToolTip))
			pinData._pinTooltip = pinEntry.value(kToolTip).toString();

		if (pinEntry.contains(kCommand))
			pinData._pinCommand = pinEntry.value(kCommand).toString();

		if (pinEntry.contains(kCommmandGroup))
			pinData._commandGroup = CommandGroups(pinEntry.value(kCommmandGroup).toInt());

		if (pinEntry.contains(kClassicAction))
			pinData._classicAction = pinEntry.value(kClassicAction).toString();

		if (pinEntry.contains(kTabName))
			pinData._tabName = pinEntry.value(kTabName).toString();

		if (pinEntry.contains(kRunPriority))
			pinData._cellLocation = toQPoint(pinEntry.value(kRunPriority).toString());

		_pinEntries.insert(pinData._pin, pinData);
	}

	return result;
}

void _PSOCPlatformConfiguration::write(QJsonObject &parentLevel)
{
	_PlatformConfiguration::write(parentLevel);

	QJsonArray firmwareVerions;

	for (int fwVer : std::as_const(_supportedFirmwareVer))
	{
		QJsonValue val;
		firmwareVerions.append(fwVer);
	}

	parentLevel[kMinFirmwareVersion] = firmwareVerions;

	// Writing platform configuration mapping
	QJsonArray jsonPlatformPinData;

	for (const auto& pinData : std::as_const(_pinEntries))
	{
		QJsonObject psocPinData;

		psocPinData.insert(kPinNumber, QString::number(pinData._pin));
		psocPinData.insert(kEnabled, pinData._enabled);
		psocPinData.insert(kIntialValue, pinData._initialValue);
		psocPinData.insert(kPriority, pinData._initializationPriority);
		psocPinData.insert(kInverted, pinData._inverted);
		psocPinData.insert(kName, pinData._pinLabel);
		psocPinData.insert(kToolTip, pinData._pinTooltip);
		psocPinData.insert(kCommand, pinData._pinCommand);
		psocPinData.insert(kCommmandGroup, pinData._commandGroup);
		psocPinData.insert(kClassicAction, pinData._classicAction);
		// run_prority indicates cell_location (the location of the PinLED within TAC)
		psocPinData[kRunPriority] = fromQPoint(pinData._cellLocation);
		psocPinData.insert(kTabName, pinData._tabName);

		jsonPlatformPinData.append(psocPinData);
	}

	parentLevel[kPlatformEntries] = jsonPlatformPinData;
}

void _PSOCPlatformConfiguration ::initialize()
{
	PSOCPinData pinData;

	//--------------
	pinData.clear();
	pinData._pin = 53;
	pinData._pinLabel = "Battery";
	pinData._pinCommand = "battery";
	pinData._pinTooltip = "Battery Disconnect";
	pinData._initializationPriority = 2;
	pinData._tabName = "General";
	pinData._commandGroup = eConnectionGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_POWER_OFF";
	pinData._cellLocation = QPoint(0, 0);
	pinData._initialValue = false;
	pinData._inverted = true;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 55;
	pinData._pinLabel = "USB 0";
	pinData._pinCommand = "usb0";
	pinData._pinTooltip = "Disconnnects VBUS for USB 0";
	pinData._initializationPriority = 2;
	pinData._tabName = "General";
	pinData._commandGroup = eConnectionGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_USB0_DIS";
	pinData._cellLocation = QPoint(1, 0);
	pinData._initialValue = false;
	pinData._inverted = true;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 36;
	pinData._pinLabel = "USB 1";
	pinData._pinTooltip = "Disconnects VBUS for USB1";
	pinData._pinCommand = "usb1";
	pinData._initializationPriority = 2;
	pinData._tabName = "General";
	pinData._commandGroup = eConnectionGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_USB1_DIS";
	pinData._cellLocation = QPoint(2, 0);
	pinData._initialValue = false;
	pinData._inverted = true;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 50;
	pinData._pinLabel = "Power Key";
	pinData._pinCommand = "pkey";
	pinData._pinTooltip = "Presses the power key button";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eButtonGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_TC_START";
	pinData._cellLocation = QPoint(0, 0);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 34;
	pinData._pinLabel = "Volume Up";
	pinData._pinTooltip = "Presses the volume up button";
	pinData._pinCommand = "volup";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eButtonGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_VOL_UP";
	pinData._cellLocation = QPoint(0, 1);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 51;
	pinData._pinLabel = "Volume Down";
	pinData._pinCommand = "voldn";
	pinData._pinTooltip = "Presses the volume down button";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eButtonGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_RESET_PRI";
	pinData._cellLocation = QPoint(0, 2);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 18;
	pinData._pinLabel = "Disconnect UIM 1";
	pinData._pinTooltip = "Disconnects UIM 1";
	pinData._pinCommand = "uim1";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eSwitchGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_UIM1_DIS";
	pinData._cellLocation = QPoint(0, 0);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 19;
	pinData._pinLabel = "Disconnect UIM 2";
	pinData._pinTooltip = "Disconnects UIM 2";
	pinData._pinCommand = "uim2";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eSwitchGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_UIM2_DIS";
	pinData._cellLocation = QPoint(1, 0);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 54;
	pinData._pinLabel = "Emergency Download Mode (EDL)";
	pinData._pinCommand = "pedl";
	pinData._pinTooltip = "Enables Primary Emergency Download Mode";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eSwitchGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_SW_DWNLD_PRI";
	pinData._cellLocation = QPoint(0, 1);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 37;
	pinData._pinLabel = "Force PS_HOLD High";
	pinData._pinTooltip = "Forces PSHold high";
	pinData._pinCommand = "pshold";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eSwitchGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_FORCE_PS_HOLD";
	pinData._cellLocation = QPoint(0, 2);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 47;
	pinData._pinLabel = "Disconnect SD Card";
	pinData._pinTooltip = "Disconnects the SD card slot";
	pinData._pinCommand = "sdcard";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eSwitchGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_SDCARD_DISC";
	pinData._cellLocation = QPoint(0, 3);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 16;
	pinData._pinLabel = "Embedded USB Debug (EUD)";
	pinData._pinTooltip = "Enabled the embedded USB debugger";
	pinData._pinCommand = "eud";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eSwitchGroup;
	pinData._enabled = true;
	pinData._classicAction = "TC_EUD_EARLY_BOOT_EN";
	pinData._cellLocation = QPoint(0, 4);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 21;
	pinData._pinLabel = "Headset Disconnect";
	pinData._pinTooltip = "Disconnects the headset";
	pinData._pinCommand = "headset";
	pinData._initializationPriority = -1;
	pinData._tabName = "General";
	pinData._commandGroup = eSwitchGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_HEADSET_DIS";
	pinData._cellLocation = QPoint(0, 5);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 30;
	pinData._pinLabel = "Secondary Emergency Download Mode (EDL)";
	pinData._pinTooltip = "Secondary EDL";
	pinData._pinCommand = "sedl";
	pinData._initializationPriority = -1;
	pinData._tabName = "Fusion";
	pinData._commandGroup = eSwitchGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_FORCE_USB_BOOT_SEC";
	pinData._cellLocation = QPoint(0, 0);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 31;
	pinData._pinLabel = "Secondary PM_RESIN_N";
	pinData._pinTooltip = "Secondary PM_RESIN_N";
	pinData._pinCommand = "sresn";
	pinData._initializationPriority = -1;
	pinData._tabName = "Fusion";
	pinData._commandGroup = eSwitchGroup;
	pinData._enabled = true;
	pinData._classicAction = "TAC_RESET_SEC";
	pinData._cellLocation = QPoint(0, 1);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 29;
	pinData._initializationPriority = -1;
	pinData._initialValue = false;
	pinData._enabled = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 38;
	pinData._initializationPriority = -1;
	pinData._initialValue = false;
	pinData._enabled = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 39;
	pinData._initializationPriority = -1;
	pinData._initialValue = false;
	pinData._enabled = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 46;
	pinData._initializationPriority = -1;
	pinData._initialValue = false;
	pinData._enabled = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 48;
	pinData._initializationPriority = -1;
	pinData._initialValue = false;
	pinData._enabled = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 49;
	pinData._initializationPriority = -1;
	pinData._initialValue = false;
	pinData._enabled = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;

	//--------------
	pinData.clear();
	pinData._pin = 15;
	pinData._pinLabel = "";
	pinData._initializationPriority = -1;
	pinData._commandGroup = eUnknownCommandGroup;
	pinData._enabled = false;
	pinData._classicAction = "TAC_SOFT_RESET";
	pinData._cellLocation = QPoint(-1,-1);
	pinData._initialValue = false;
	pinData._inverted = false;

	_PSOCPlatformConfiguration::_classicActions[pinData._pin] = pinData;
}
