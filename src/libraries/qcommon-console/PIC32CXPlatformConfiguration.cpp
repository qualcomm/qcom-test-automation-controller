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
// Copyright 2024-2025 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author:	Biswajit Roy <biswroy@qti.qualcomm.com>

#include "AlpacaScript.h"
#include "PIC32CXPlatformConfiguration.h"
#include "PlatformConfigurationException.h"

#include "Range.h"

#include <QJsonArray>
#include <QJsonObject>

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
const QString kInverted(QStringLiteral("inverted"));
const QString kName(QStringLiteral("name"));
const QString kToolTip(QStringLiteral("help_hint"));
const QString kCommand(QStringLiteral("command"));
const QString kCommmandGroup(QStringLiteral("command_group"));
const QString kClassicAction(QStringLiteral("classic_action"));
const QString kTabName(QStringLiteral("tab_name"));


_PIC32CXPlatformConfiguration::_PIC32CXPlatformConfiguration()
{
	_platform = ePIC32CXAuto;

	_resetActive = true;

	_PIC32CXPlatformConfiguration::initialize();

	// Add default tabs to _editorTabs
	Tab generalTab, deviceInfoTab, terminalTab;

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

	terminalTab._name = "Terminal";
	terminalTab._moveable = true;
	terminalTab._configurable = false;
	terminalTab._ordinal = 2;
	terminalTab._userTab = false;

	_tabs.append(generalTab);
	_tabs.append(deviceInfoTab);
	_tabs.append(terminalTab);
}

_PIC32CXPlatformConfiguration::~_PIC32CXPlatformConfiguration()
{
}

Pins _PIC32CXPlatformConfiguration::getPins()
{
	Pins result;

	PIC32CXPinList pic32cxPinList = getActivePins();

	for (const auto& pic32cxPin: pic32cxPinList)
	{
		PinEntry pinEntry;

		pinEntry._pin = pic32cxPin._setPin;
		pinEntry._enabled = pic32cxPin._enabled;
		pinEntry._hash = pic32cxPin._hash;
		pinEntry._cellLocation = pic32cxPin._cellLocation;
		pinEntry._commandGroup = pic32cxPin._commandGroup;
		pinEntry._pinCommand = pic32cxPin._pinCommand;
		pinEntry._inverted = pic32cxPin._inverted;
		pinEntry._pinLabel = pic32cxPin._pinLabel;
		pinEntry._pinTooltip = pic32cxPin._pinTooltip;
		pinEntry._tabName = pic32cxPin._tabName;

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

PIC32CXPinList _PIC32CXPlatformConfiguration::getAllPins()
{
	PIC32CXPinList pinList;

	for (const auto& pinData: std::as_const(_pinEntries))
	{
		pinList.append(pinData);
	}

	auto sortLambda = [] (PIC32CXPinData& p1, PIC32CXPinData& p2) -> bool
	{
		return p1._setPin < p2._setPin;
	};

	std::sort(pinList.begin(), pinList.end(), sortLambda);

	return pinList;
}

PIC32CXPinList _PIC32CXPlatformConfiguration::getActivePins()
{
	PIC32CXPinList pinList;

	for (const auto& pinData: std::as_const(_pinEntries))
	{
		if (pinData._enabled == true)
			pinList.append(pinData);
	}

	auto sortLambda = [] (PIC32CXPinData& p1, PIC32CXPinData& p2) -> bool
	{
		return p1._setPin < p2._setPin;
	};

	std::sort(pinList.begin(), pinList.end(), sortLambda);

	return pinList;
}

bool _PIC32CXPlatformConfiguration::getPinEnableState(const PinID pinId) const
{
	bool result{true};

	HashType hash = PIC32CXPinData::makePIC32CXHash(pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._enabled;
	}

	return result;
}
void _PIC32CXPlatformConfiguration::setPinEnableState(const HashType hash, bool newState)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._enabled != newState)
		{
			pinData._enabled = newState;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update state of an invalid pin!");
	}
}

bool _PIC32CXPlatformConfiguration::getPinInvertedState(const PinID pinId) const
{
	bool result{false};

	HashType hash = PIC32CXPinData::makePIC32CXHash(pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._inverted;
	}

	return result;
}

void _PIC32CXPlatformConfiguration::setPinInvertedState(const HashType hash, bool newState)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._inverted != newState)
		{
			pinData._inverted = newState;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update state of an invalid pin!");
	}
}

QString _PIC32CXPlatformConfiguration::getPinLabel(const PinID pinId) const
{
	QString result;

	HashType hash = PIC32CXPinData::makePIC32CXHash(pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinLabel;
	}
	return result;
}

void _PIC32CXPlatformConfiguration::setPinLabel(const HashType hash, const QString& pinLabel)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		pinData._pinLabel = pinLabel;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add label to an invalid pin!");
	}
}

QString _PIC32CXPlatformConfiguration::getPinTooltip(const PinID pinId) const
{
	QString result;

	HashType hash = PIC32CXPinData::makePIC32CXHash(pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinTooltip;
	}

	return result;
}

void _PIC32CXPlatformConfiguration::setPinTooltip(const HashType hash, const QString& pinTooltip)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		pinData._pinTooltip = pinTooltip;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add tooltip to an invalid pin!");
	}
}

QString _PIC32CXPlatformConfiguration::getPinCommand(const PinID pinId) const
{
	QString result;

	HashType hash = PIC32CXPinData::makePIC32CXHash(pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinCommand;
	}

	return result;
}

void _PIC32CXPlatformConfiguration::setPinCommand(const HashType hash, const QString &pinCommand)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		if (pinData._pinCommand != pinCommand)
		{
			pinData._pinCommand = pinCommand;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add command to an invalid pin!");
	}
}

CommandGroups _PIC32CXPlatformConfiguration::getPinGroup(const PinID pinId) const
{
	CommandGroups result{eUnknownCommandGroup};

	HashType hash = PIC32CXPinData::makePIC32CXHash(pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._commandGroup;
	}

	return result;
}

void _PIC32CXPlatformConfiguration::setPinGroup(const HashType hash, const CommandGroups commandGroup)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		if (pinData._commandGroup != commandGroup)
		{
			pinData._commandGroup = commandGroup;

			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add category to an invalid pin!");
	}
}

QString _PIC32CXPlatformConfiguration::getTabName(const PinID pinId) const
{
	QString result;

	HashType hash = PIC32CXPinData::makePIC32CXHash(pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._tabName;
	}

	return result;
}

void _PIC32CXPlatformConfiguration::setTabName(const HashType hash, const QString& tabName)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._tabName != tabName)
		{
			pinData._tabName = tabName;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add tab name for an invalid pin!");
	}
}

QPoint _PIC32CXPlatformConfiguration::getPinCellLocation(const PinID pinId) const
{
	QPoint result(-1,-1);

	HashType hash = PIC32CXPinData::makePIC32CXHash(pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._cellLocation;
	}

	return result;
}

void _PIC32CXPlatformConfiguration::setPinCellLocation(const HashType hash, const QPoint& cellLocation)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		if (pinData._cellLocation != cellLocation)
		{
			pinData._cellLocation = cellLocation;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set cell location for an invalid pin!");
	}
}

void _PIC32CXPlatformConfiguration::cascadeTabDelete(const QString &deleteMe)
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

void _PIC32CXPlatformConfiguration::cascadeTabRename
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

bool _PIC32CXPlatformConfiguration::read(QJsonObject &parentLevel)
{
	bool result{false};
	QJsonValue jsonValue;

	result = _PlatformConfiguration::read(parentLevel);

	QJsonArray pinDataArray = parentLevel.value(kPlatformEntries).toArray();

	for (auto pinIndex: range(pinDataArray.count()))
	{
		QJsonObject pinEntry = pinDataArray.at(pinIndex).toObject();

		PIC32CXPinData pinData;

		if (pinEntry.contains(kPinNumber))
		{
			pinData._setPin = pinEntry.value(kPinNumber).toInt();
			pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
		}

		if (pinEntry.contains(kEnabled))
			pinData._enabled = pinEntry.value(kEnabled).toBool();

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

		if (pinEntry.contains(kTabName))
			pinData._tabName = pinEntry.value(kTabName).toString();

		if (pinEntry.contains(kRunPriority))
			pinData._cellLocation = toQPoint(pinEntry.value(kRunPriority).toString());

		_pinEntries.insert(pinData._hash, pinData);
	}

	return result;
}

void _PIC32CXPlatformConfiguration::write(QJsonObject &parentLevel)
{
	_PlatformConfiguration::write(parentLevel);

	// Writing platform configuration mapping
	QJsonArray jsonPlatformPinData;

	for (const auto& pinData : std::as_const(_pinEntries))
	{
		QJsonObject pic32cxPinData;

		pic32cxPinData.insert(kPinNumber, static_cast<int>(pinData._setPin));
		pic32cxPinData.insert(kEnabled, pinData._enabled);
		pic32cxPinData.insert(kInverted, pinData._inverted);
		pic32cxPinData.insert(kName, pinData._pinLabel);
		pic32cxPinData.insert(kToolTip, pinData._pinTooltip);
		pic32cxPinData.insert(kCommand, pinData._pinCommand);
		pic32cxPinData.insert(kCommmandGroup, pinData._commandGroup);
		// run_prority indicates cell_location (the location of the PinLED within TAC)
		pic32cxPinData[kRunPriority] = fromQPoint(pinData._cellLocation);
		pic32cxPinData.insert(kTabName, pinData._tabName);

		jsonPlatformPinData.append(pic32cxPinData);
	}

	parentLevel[kPlatformEntries] = jsonPlatformPinData;
}

void _PIC32CXPlatformConfiguration ::initialize()
{
	PIC32CXPinData pinData;

	pinData.clear();

	pinData._setPin = 3;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "JMI2C.2";
	pinData._pinCommand = "iic2";
	pinData._pinTooltip = "Debug pin";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 4;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "Battery";
	pinData._pinCommand = "battery";
	pinData._pinTooltip = "Disconnects power to the device";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 6;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "KK Power Enable";
	pinData._pinCommand = "kkpwr";
	pinData._pinTooltip = "Enables KK Power";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 7;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "MD PS HOLD";
	pinData._pinCommand = "pshold";
	pinData._pinTooltip = "MD PS HOLD";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(1,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 10;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "KK Resin N";
	pinData._pinCommand = "sresn";
	pinData._pinTooltip = "KK Resin N";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(2,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 11;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "PMS Power On";
	pinData._pinCommand = "pmspwr";
	pinData._pinTooltip = "PMS Power On";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 16;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;

	pinData._inverted = false;
	pinData._pinLabel = "UEFI";
	pinData._pinCommand = "uefi";
	pinData._pinTooltip = "UEFI";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,2);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 20;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "Power Off";
	pinData._pinCommand = "pkey";
	pinData._pinTooltip = "Power Off";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 21;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "PCIE0 Attention";
	pinData._pinCommand = "pcie0";
	pinData._pinTooltip = "PCIE 0 Attention";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 27;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "PCIE1 Attention";
	pinData._pinCommand = "pcie1";
	pinData._pinTooltip = "PCIE 1 Attention";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(1,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 101;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "PCIE2 Attention";
	pinData._pinCommand = "pcie2";
	pinData._pinTooltip = "PCIE 2 Attention";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(2,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 102;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "PCIE3 Attention";
	pinData._pinCommand = "pcie3";
	pinData._pinTooltip = "PCIE 3 Attention";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 103;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "JMI2C.10";
	pinData._pinCommand = "iic10";
	pinData._pinTooltip = "Debug pin";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 110;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "Kratos Trigger";
	pinData._pinCommand = "kratos";
	pinData._pinTooltip = "Kratos Trigger";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 111;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "JMI2C.4";
	pinData._pinCommand = "iic4";
	pinData._pinTooltip = "Debug pin";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 114;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "QCC BLD Disconnect";
	pinData._pinCommand = "qccbld";
	pinData._pinTooltip = "QCC BLD Disconnect";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 115;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "BB BLD Disconnect";
	pinData._pinCommand = "bbbld";
	pinData._pinTooltip = "BB BLD Disconnect";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 116;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "Secondary Fastboot";
	pinData._pinCommand = "sfastboot";
	pinData._pinTooltip = "Fastboot SS";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(1,1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 117;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "JMI2C.5";
	pinData._pinCommand = "iic5";
	pinData._pinTooltip = "Debug pin";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 118;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "PG QAM VR4P5";
	pinData._pinCommand = "inputvr4p5";
	pinData._pinTooltip = "PG QAM VR4P5";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 119;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "PG QAM SAIL VR3P3";
	pinData._pinCommand = "inputvr3p3";
	pinData._pinTooltip = "PG QAM SAIL VR3P3";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 120;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "Force MD PS HOLD";
	pinData._pinCommand = "forcemdpshold";
	pinData._pinTooltip = "Force MD PS HOLD";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(1,1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 121;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "Force SS PS HOLD";
	pinData._pinCommand = "forcesspshold";
	pinData._pinTooltip = "Force SS PS HOLD";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(2,1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 123;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 128;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "USB0";
	pinData._pinCommand = "usb0";
	pinData._pinTooltip = "Disconnect USB0";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(1,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 129;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "PMS PGOOD";
	pinData._pinCommand = "pgood";
	pinData._pinTooltip = "-";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(4,0);

	_pinEntries[pinData._hash] = pinData;
	
	pinData.clear();

	pinData._setPin = 204;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 205;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "USB2";
	pinData._pinCommand = "usb2";
	pinData._pinTooltip = "USB 2 Disable";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 206;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "Fast Power Off";
	pinData._pinCommand = "inputpwr";
	pinData._pinTooltip = "Fast K Power Off Enable";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(4,2);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 207;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "Mode 0";
	pinData._pinCommand = "mode0";
	pinData._pinTooltip = "Mode 0 for Primary SOC";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(1,2);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 215;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "Mode 1";
	pinData._pinCommand = "mode1";
	pinData._pinTooltip = "Mode 1";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(2,2);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 216;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "MD EDL";
	pinData._pinCommand = "pedl";
	pinData._pinTooltip = "MD EDL";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,3);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 217;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "Mode 0";
	pinData._pinCommand = "mode0";
	pinData._pinTooltip = "Mode 0";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 219;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "SS EDL";
	pinData._pinCommand = "sedl";
	pinData._pinTooltip = "SS EDL";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(1,3);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 221;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "JMI2C.13";
	pinData._pinCommand = "iic13";
	pinData._pinTooltip = "Debug pin";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 224;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "PMS Fast Power Off";
	pinData._pinCommand = "inputpoff";
	pinData._pinTooltip = "PMS Fast Power Off";
	pinData._commandGroup = eUnknownCommandGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 225;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "PMS Enable";
	pinData._pinCommand = "inputpms";
	pinData._pinTooltip = "PMS Enable";
	pinData._commandGroup = eUnknownCommandGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 226;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "BB BLD EN";
	pinData._pinCommand = "bbblden";
	pinData._pinTooltip = "-";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 227;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "QCC BLD EN";
	pinData._pinCommand = "qccblden";
	pinData._pinTooltip = "-";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 228;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "Mode 2";
	pinData._pinCommand = "mode2";
	pinData._pinTooltip = "Mode 2";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 311;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "EUD";
	pinData._pinCommand = "eud";
	pinData._pinTooltip = "EUD for Primary SOC";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(2,3);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 312;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "USB1";
	pinData._pinCommand = "usb1";
	pinData._pinTooltip = "USB1 Disconnect";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(2,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 320;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = false;
	pinData._inverted = false;
	pinData._pinLabel = "JMI2C.7";
	pinData._pinCommand = "iic7";
	pinData._pinTooltip = "Debug I2C 7";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(-1,-1);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();

	pinData._setPin = 321;
	pinData._hash = PIC32CXPinData::makePIC32CXHash(pinData._setPin);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "Fastboot MD";
	pinData._pinCommand = "fastboot";
	pinData._pinTooltip = "MD Fastboot";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(2,1);

	_pinEntries[pinData._hash] = pinData;

	// Quick Settings for PIC32CX

	_buttons.clear();

	Button button;

	button._label = "Power On";
	button._tab = "General";
	button._cellLocation = QPoint(0, 0);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Power on the MTP/Device";
	button._command = "powerOn";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Power Off";
	button._tab = "General";
	button._cellLocation = QPoint(1, 0);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Power off the MTP/Device";
	button._command = "powerOff";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Boot MD EDL";
	button._tab = "General";
	button._cellLocation = QPoint(2, 0);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Boots the device to primary EDL";
	button._command = "bootToEDL";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Boot SS EDL";
	button._tab = "General";
	button._cellLocation = QPoint(0, 1);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Boots the device to secondary EDL";
	button._command = "bootToSecondaryEDL";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Boot to UEFI";
	button._tab = "General";
	button._cellLocation = QPoint(1, 1);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Boots the device to UEFI";
	button._command = "bootToUEFI";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Boot to MD Fastboot";
	button._tab = "General";
	button._cellLocation = QPoint(2, 1);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Boots the device to primary fastboot";
	button._command = "bootToFastboot";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Boot to SS MD Fastboot";
	button._tab = "General";
	button._cellLocation = QPoint(3, 0);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Boots the device to secondary fastboot";
	button._command = "bootToSecondaryFastboot";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	_alpacaScript = AlpacaScript::defaultScript(_platform);

	// Script variables

	_scriptVariables.clear();

	ScriptVariable variable;

	variable._name = "edl";
	variable._label = "EDL timing (ms)";
	variable._tooltip = "Configurable Boot to EDL timing in milliseconds";
	variable._type = eIntegerType;
	variable._defaultValue = 1500;
	variable._cellLocation = QPoint(0,0);

	_scriptVariables.insert(variable._name, variable);

	variable._name = "uefi";
	variable._label = "UEFI timing (ms)";
	variable._tooltip = "Configurable Boot to UEFI timing in milliseconds";
	variable._type = eIntegerType;
	variable._defaultValue = 10000;
	variable._cellLocation = QPoint(0,1);

	_scriptVariables.insert(variable._name, variable);

	variable._name = "fastboot";
	variable._label = "Fastboot timing (ms)";
	variable._tooltip = "Configurable Boot to fastboot timing in milliseconds";
	variable._type = eIntegerType;
	variable._defaultValue = 10000;
	variable._cellLocation = QPoint(1,0);

	_scriptVariables.insert(variable._name, variable);
}
