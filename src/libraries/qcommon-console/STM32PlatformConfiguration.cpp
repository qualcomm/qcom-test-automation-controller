// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "STM32PlatformConfiguration.h"

// QCommon
#include "DebugBoardType.h"
#include "Range.h"
#include "PlatformConfigurationException.h"
#include "StringUtilities.h"

// QT
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>

const QString kPin(QStringLiteral("pin"));
const QString kEnabled(QStringLiteral("enabled"));
const QString kInput(QStringLiteral("input"));
const QString kName(QStringLiteral("name"));
const QString kToolTip(QStringLiteral("help_hint"));
const QString kInverted(QStringLiteral("inverted"));
const QString kCommand(QStringLiteral("command"));
const QString kCommandGroup(QStringLiteral("command_group"));
const QString kRunPriority(QStringLiteral("run_priority"));
const QString kTabName(QStringLiteral("group"));

const QString kPinEntries(QStringLiteral("pins"));


_STM32PlatformConfiguration::_STM32PlatformConfiguration()
{
	_resetActive = false;

	Tab generalTab, deviceInfoTab;

	generalTab._name = "General";
	generalTab._visible = true;
	generalTab._moveable = false;
	generalTab._configurable = true;
	generalTab._ordinal = 0;
	generalTab._userTab = false;

	deviceInfoTab._name = "Device Info";
	deviceInfoTab._visible = true;
	deviceInfoTab._moveable = false;
	deviceInfoTab._configurable = false;
	deviceInfoTab._ordinal = 1;
	deviceInfoTab._userTab = false;

	_tabs.append(generalTab);
	_tabs.append(deviceInfoTab);

	initialize();
}

void _STM32PlatformConfiguration::initialize()
{
	_platform = eSTM32;
	_platformId = ALPACA_STM32_ID;

	_pinEntries.clear();

	STM32PinData pinData;

	pinData.clear();
	pinData = STM32PinData(0);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "EDL";
	pinData._pinCommand = "edl";
	pinData._pinTooltip = "Boot mode select. High = USB / EDL boot, Low = Normal boot";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();
	pinData = STM32PinData(2);
	pinData._enabled = true;
	pinData._inverted = true;
	pinData._pinLabel = "Battery";
	pinData._pinCommand = "battery";
	pinData._pinTooltip = "Disable power supply";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(1,0);

	_pinEntries[pinData._hash] = pinData;

	pinData.clear();
	pinData = STM32PinData(3);
	pinData._enabled = true;
	pinData._inverted = false;
	pinData._pinLabel = "Volume Down";
	pinData._pinCommand = "voldn";
	pinData._pinTooltip = "Volume down / USB-C host mode select button state";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(2,0);

	_pinEntries[pinData._hash] = pinData;

	_buttons.clear();

	Button button;

	button._label = "Power On";
	button._tab = "General";
	button._cellLocation = QPoint(0, 0);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Power cycle to normal boot mode";
	button._command = "powerOn";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Power Off";
	button._tab = "General";
	button._cellLocation = QPoint(1, 0);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Power off the device";
	button._command = "powerOff";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Reset";
	button._tab = "General";
	button._cellLocation = QPoint(2, 0);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "MPU reset pulse";
	button._command = "reset";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Boot to EDL";
	button._tab = "General";
	button._cellLocation = QPoint(0, 1);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Power cycle to USB / EDL boot mode";
	button._command = "bootToEDL";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	button._label = "Force USB-C Host Mode";
	button._tab = "General";
	button._cellLocation = QPoint(1, 1);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Force the device into USB-C host mode";
	button._command = "forceUsbcHostMode";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	_alpacaScript = "def powerOff()\n"
					"logComment ====== powerOff sequence start ======\n"
					"battery 0\n"
					"logComment ====== powerOff sequence finish ======\n\n"
					"def powerOn()\n"
					"logComment ====== powerOn sequence start ======\n"
					"battery 1\n"
					"edl 0\n"
					"voldn 0\n"
					"logComment ====== powerOn sequence finish ======\n\n"
					"def reset()\n"
					"logComment ====== reset sequence start ======\n"
					"battery 1\n"
					"battery 0\n"
					"edl 0\n"
					"voldn 0\n"
					"logComment ====== reset sequence finish ======\n\n"
					"def bootToEDL()\n"
					"logComment ====== bootToEDL sequence start ======\n"
					"battery 0\n"
					"edl 1\n"
					"delay $edl\n"
					"battery 1\n"
					"logComment ====== bootToEDL sequence finish ======\n\n"
					"def forceUsbcHostMode()\n"
					"logComment ====== forceUsbcHostMode sequence start ======\n"
					"edl 0\n"
					"battery 1\n"
					"voldn 1\n"
					"logComment ====== forceUsbcHostMode sequence finish ======";

	_scriptVariables.clear();

	ScriptVariable variable;

	variable._name = "edl";
	variable._label = "EDL timing (ms)";
	variable._tooltip = "Configurable Boot to EDL timing in milliseconds";
	variable._type = eIntegerType;
	variable._defaultValue = 100;
	variable._cellLocation = QPoint(0,0);

	_scriptVariables.insert(variable._name, variable);
}

_STM32PlatformConfiguration::~_STM32PlatformConfiguration()
{

}

STM32PinData _STM32PlatformConfiguration::getPinData(PinID pin)
{
	STM32PinData result;

	HashType hash = STM32PinData::makeSTM32Hash(pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		result = _pinEntries[hash];
	}

	return result;
}

STM32PinList _STM32PlatformConfiguration::getAllPins() const
{
	STM32PinList result;

	for (const auto& pinEntry : std::as_const(_pinEntries))
	{
		result.append(pinEntry);
	}

	auto sortLambda = [] (STM32PinData& p1, STM32PinData& p2) -> bool
	{
		return p1._pin < p2._pin;
	};

	std::sort(result.begin(), result.end(), sortLambda);

	return result;
}

STM32PinList _STM32PlatformConfiguration::getActivePins() const
{
	STM32PinList result;

	for (const auto& pinEntry : std::as_const(_pinEntries))
	{
		if (pinEntry._enabled == true)
			result.append(pinEntry);
	}

	auto sortLambda = [] (STM32PinData& p1, STM32PinData& p2) -> bool
	{
		return p1._pin < p2._pin;
	};

	std::sort(result.begin(), result.end(), sortLambda);

	return result;
}

Pins _STM32PlatformConfiguration::getPins()
{
	Pins result;

	STM32PinList stm32PinList = getActivePins();

	for (const auto& stm32Pin: stm32PinList)
	{
		PinEntry pinEntry;

		pinEntry._pin = stm32Pin._pin;
		pinEntry._enabled = stm32Pin._enabled;
		pinEntry._hash = stm32Pin._hash;
		pinEntry._cellLocation = stm32Pin._cellLocation;
		pinEntry._commandGroup = stm32Pin._commandGroup;
		pinEntry._pinCommand = stm32Pin._pinCommand;
		pinEntry._inverted = stm32Pin._inverted;
		pinEntry._pinLabel = stm32Pin._pinLabel;
		pinEntry._pinTooltip = stm32Pin._pinTooltip;
		pinEntry._tabName = stm32Pin._tabName;

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

bool _STM32PlatformConfiguration::getPinEnableState(const PinID pin) const
{
	bool result{true};

	HashType hash = STM32PinData::makeSTM32Hash(pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._enabled;
	}

	return result;
}

void _STM32PlatformConfiguration::setPinEnableState(HashType hash, bool newState)
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

bool _STM32PlatformConfiguration::getPinInvertedState(const PinID pin) const
{
	bool result{false};

	HashType hash = STM32PinData::makeSTM32Hash(pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._inverted;
	}

	return result;
}

void _STM32PlatformConfiguration::setPinInvertedState(HashType hash, bool newState)
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

QString _STM32PlatformConfiguration::getPinLabel(const PinID pin) const
{
	QString result;

	HashType hash = STM32PinData::makeSTM32Hash(pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinLabel;
	}
	return result;
}

void _STM32PlatformConfiguration::setPinLabel(HashType hash, const QString& pinLabel)
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

QString _STM32PlatformConfiguration::getPinTooltip(const PinID pin) const
{
	QString result;

	HashType hash = STM32PinData::makeSTM32Hash(pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinTooltip;
	}

	return result;
}

void _STM32PlatformConfiguration::setPinTooltip(HashType hash, const QString& pinTooltip)
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

QString _STM32PlatformConfiguration::getPinCommand(const PinID pin) const
{
	QString result;

	HashType hash = STM32PinData::makeSTM32Hash(pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinCommand;
	}

	return result;
}

void _STM32PlatformConfiguration::setPinCommand(HashType hash, const QString& pinCommand)
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

CommandGroups _STM32PlatformConfiguration::getPinGroup(const PinID pin) const
{
	CommandGroups result{eUnknownCommandGroup};

	HashType hash = STM32PinData::makeSTM32Hash(pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._commandGroup;
	}

	return result;
}

void _STM32PlatformConfiguration::setPinGroup(HashType hash, const CommandGroups commandGroup)
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

QString _STM32PlatformConfiguration::getTabName(const PinID pin) const
{
	QString result;

	HashType hash = STM32PinData::makeSTM32Hash(pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._tabName;
	}

	return result;
}

void _STM32PlatformConfiguration::setTabName(HashType hash, const QString& tabName)
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

QPoint _STM32PlatformConfiguration::getPinCellLocation(const PinID pin) const
{
	QPoint result(-1,-1);

	HashType hash = STM32PinData::makeSTM32Hash(pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._cellLocation;
	}

	return result;
}

void _STM32PlatformConfiguration::setPinCellLocation(HashType hash, const QPoint& cellLocation)
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

void _STM32PlatformConfiguration::cascadeTabDelete(const QString &deleteMe)
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

void _STM32PlatformConfiguration::cascadeTabRename(const QString& oldName, const QString& newName)
{
	for (auto& pinEntry: _pinEntries)
	{
		if (pinEntry._tabName == oldName)
			pinEntry._tabName = newName;
	}
}

bool _STM32PlatformConfiguration::read(QJsonObject& parentLevel)
{
	bool result{false};

	result = _PlatformConfiguration::read(parentLevel);
	if (result == true)
	{
		if (parentLevel.contains(kPinEntries) && parentLevel[kPinEntries].isArray())
		{
			QJsonArray pinsArray = parentLevel[kPinEntries].toArray();

			_pinEntries.clear();

			for (auto pinIndex: range(pinsArray.size()))
			{
				QJsonObject pinEntry = pinsArray[pinIndex].toObject();

				PinID pin{0};

				if (pinEntry.contains(kPin))
					pin = pinEntry[kPin].toString().toULongLong();

				STM32PinData pinData(pin);

				if (pinEntry.contains(kEnabled))
					pinData._enabled = pinEntry[kEnabled].toBool();

				if (pinEntry.contains(kName))
					pinData._pinLabel = pinEntry[kName].toString();

				if (pinEntry.contains(kToolTip))
					pinData._pinTooltip = pinEntry[kToolTip].toString();

				if (pinEntry.contains(kInverted))
					pinData._inverted = pinEntry[kInverted].toBool();

				if (pinEntry.contains(kCommand))
					pinData._pinCommand = pinEntry[kCommand].toString();

				if (pinEntry.contains(kCommandGroup))
					pinData._commandGroup = static_cast<CommandGroups>(pinEntry[kCommandGroup].toInt());

				if (pinEntry.contains(kRunPriority))
				{
					pinData._cellLocation = QPoint(-1, -1);
					QString pointString = pinEntry[kRunPriority].toString();
					if (pointString.size() > 0)
					{
						QPoint point = toQPoint(pointString);
						if (point.x() >= 0 && point.y() >= 0)
							pinData._cellLocation = point;
					}
				}

				if (pinEntry.contains(kTabName))
					pinData._tabName = pinEntry[kTabName].toString();

				_pinEntries[pinData._hash] = pinData;
			}
		}
	}

	return result;
}

void _STM32PlatformConfiguration::write(QJsonObject& parentLevel)
{
	_PlatformConfiguration::write(parentLevel);

	QJsonArray pinsArray;

	for (auto& pinEntry : _pinEntries)
	{
		QJsonObject pinData;

		pinData[kPin] = QString::number(pinEntry._pin);
		pinData[kEnabled] = pinEntry._enabled;
		pinData[kName] = pinEntry._pinLabel;
		pinData[kToolTip] = pinEntry._pinTooltip;
		pinData[kInverted] = pinEntry._inverted;
		pinData[kCommand] = pinEntry._pinCommand;
		pinData[kCommandGroup] = pinEntry._commandGroup;
		pinData[kRunPriority] = fromQPoint(pinEntry._cellLocation);

		pinData[kTabName] = pinEntry._tabName;

		pinsArray.append(pinData);
	}

	parentLevel[kPinEntries] = pinsArray;
}
