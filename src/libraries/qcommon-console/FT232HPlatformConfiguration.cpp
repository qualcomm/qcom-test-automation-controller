// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "FT232HPlatformConfiguration.h"

// QCommon
#include "DebugBoardType.h"
#include "Range.h"
#include "PlatformConfigurationException.h"
#include "StringUtilities.h"

// QT
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>

const QString kChipIndex(QStringLiteral("chip_index"));
const QString kBus(QStringLiteral("bus"));
const QString kPinNumber(QStringLiteral("pin_number"));
const QString kEnabled(QStringLiteral("enabled"));
const QString kInput(QStringLiteral("input"));
const QString kName(QStringLiteral("name"));
const QString kToolTip(QStringLiteral("help_hint"));
const QString kInitialValue(QStringLiteral("initial_value"));
const QString kPriority(QStringLiteral("priority"));
const QString kInverted(QStringLiteral("inverted"));
const QString kCommand(QStringLiteral("command"));
const QString kCommandGroup(QStringLiteral("command_group"));
const QString kRunPriority(QStringLiteral("run_priority"));
const QString kTabName(QStringLiteral("group"));
const QString kBusFunction(QStringLiteral("bus_function"));

const QString kPinEntries(QStringLiteral("pins"));
const QString kBusEntries(QStringLiteral("bus"));


_FT232HPlatformConfiguration::_FT232HPlatformConfiguration()
{
	_resetActive = false;

	// Add default tabs to _editorTabs
	Tab generalTab, deviceInfoTab, fusionTab, terminalTab;

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


	terminalTab._name = "Terminal";
	terminalTab._visible = true;
	terminalTab._moveable = true;
	terminalTab._configurable = false;
	terminalTab._ordinal = 4;
	terminalTab._userTab = false;

	_tabs.append(generalTab);
	_tabs.append(deviceInfoTab);
	_tabs.append(terminalTab);

	initialize();
}

void _FT232HPlatformConfiguration::initialize()
{
	_platform = eFT232H;
	_platformId = ALPACA_BUGHOPPER_ID;

	static QList<Bus> busList;
	static QList<quint32> pinList = {0, 1, 2, 3, 4, 5, 6, 7};

	busList = {Bus('C'), Bus('D')};

	for (auto& busIndex: busList)
	{
		for (auto& pinIndex: pinList)
		{
			FTDIPinData pinData(0, busIndex, pinIndex);
			FTDIBusData busData(0, busIndex, (busIndex == 'D') ? FTDIBusFunction::eBusFunctionVCP: FTDIBusFunction::eBusFunctionD2XX);

			pinData._initializationPriority = -1;
			pinData._commandGroup = eUnknownCommandGroup;
			pinData._cellLocation = QPoint(-1,-1);

			_pinEntries[pinData._hash] = pinData;
			_busFunctions[busData._hash] = busData;
		}
	}

	FTDIPinData pinData;
	FTDIBusData busData;

	// CDBUS0
	pinData.clear();
	busData.clear();

	pinData._bus = Bus('C');
	pinData._chipPin = 0;
	pinData._setPin = getSetPinIndex(0, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(0, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = false;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = 0;
	pinData._pinLabel = "EDL";
	pinData._pinCommand = "edl";
	pinData._pinTooltip = "Boot mode select. High = Normal boot, Low = USB / EDL boot";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,0);

	busData._hash = FTDIBusData::makeFTDIHash(0, pinData._bus);
	busData._chipIndex = 0;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// CDBUS1
	pinData.clear();
	busData.clear();

	pinData._bus = Bus('C');
	pinData._chipPin= 1;
	pinData._setPin = getSetPinIndex(0, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(0, pinData._bus, pinData._chipPin);
	pinData._enabled = false;
	pinData._input = false;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = 0;
	pinData._pinLabel = "Power Key";
	pinData._pinCommand = "pkey";
	pinData._pinTooltip = "Reset control";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,0);

	busData._hash = FTDIBusData::makeFTDIHash(0, pinData._bus);
	busData._chipIndex = 0;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// CDBUS2
	pinData.clear();
	busData.clear();

	pinData._bus = Bus('C');
	pinData._chipPin= 2;
	pinData._setPin = getSetPinIndex(0, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(0, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = false;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = 1;
	pinData._pinLabel = "Power Disable";
	pinData._pinCommand = "battery";
	pinData._pinTooltip = "Disable power supply";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = QPoint(0,0);

	busData._hash = FTDIBusData::makeFTDIHash(0, pinData._bus);
	busData._chipIndex = 0;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;


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

	button._label = "Boot to EDL";
	button._tab = "General";
	button._cellLocation = QPoint(2, 0);
	button._commandGroup = eQuickSettingsGroup;
	button._toolTip = "Boots the device to EDL";
	button._command = "bootToEDL";
	button._hash = Button::makeHash(button);

	_buttons[button._hash] = button;

	_alpacaScript = "def powerOff()\n"
					"logComment ====== powerOff sequence start ======\n"
					"battery 1\n"
					"logComment ====== powerOff sequence finish ======\n\n"
					"def powerOn()\n"
					"logComment ====== powerOn sequence start ======\n"
					"edl 0\n"
					"battery 1\n"
					"delay $pon\n"
					"battery 0\n"
					"logComment ====== powerOn sequence finish ======\n\n"
					"def bootToEDL()\n"
					"logComment ====== bootToEDL sequence start ======\n"
					"edl 1\n"
					"battery 1\n"
					"delay $edl\n"
					"battery 0\n"
					"logComment ====== bootToEDL sequence finish ======";

	// Script variables
	_scriptVariables.clear();

	ScriptVariable variable;

	variable._name = "edl";
	variable._label = "EDL timing (ms)";
	variable._tooltip = "Configurable Boot to EDL timing in milliseconds";
	variable._type = eIntegerType;
	variable._defaultValue = 100;
	variable._cellLocation = QPoint(0,0);

	_scriptVariables.insert(variable._name, variable);

	variable._name = "pon";
	variable._label = "Power on delay (ms)";
	variable._tooltip = "Configurable power on delay in milliseconds";
	variable._type = eIntegerType;
	variable._defaultValue = 100;
	variable._cellLocation = QPoint(1,0);

	_scriptVariables.insert(variable._name, variable);
}

_FT232HPlatformConfiguration::~_FT232HPlatformConfiguration()
{

}

FTDIPinData _FT232HPlatformConfiguration::getPinData(ChipIndex chipIndex, Bus bus, PinID pin)
{
	FTDIPinData result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, bus, pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		result = _pinEntries[hash];
	}

	return result;
}

FTDIPinList _FT232HPlatformConfiguration::getAllPins() const
{
	FTDIPinList result;

	for (const auto& pinEntry : std::as_const(_pinEntries))
	{
		result.append(pinEntry);
	}

	auto sortLambda = [] (FTDIPinData& p1, FTDIPinData& p2) -> bool
	{
		if (p1._bus != p2._bus)
			return p1._bus < p2._bus;

		return p1._chipPin < p2._chipPin;
	};

	std::sort(result.begin(), result.end(), sortLambda);

	return result;
}

FTDIPinList _FT232HPlatformConfiguration::getActivePins() const
{
	FTDIPinList result;

	for (const auto& pinEntry : std::as_const(_pinEntries))
	{
		if (pinEntry._enabled == true)
			result.append(pinEntry);
	}

	auto sortLambda = [] (FTDIPinData& p1, FTDIPinData& p2) -> bool
	{
		if (p1._bus != p2._bus)
			return p1._bus < p2._bus;

		return p1._chipPin < p2._chipPin;
	};

	std::sort(result.begin(), result.end(), sortLambda);

	return result;
}

FTDIPinList _FT232HPlatformConfiguration::getActivePins(ChipIndex chipIndex, Bus bus) const
{
	FTDIPinList result;

	FTDIBusData busData = getBusFunction(chipIndex, bus);
	if (busData._busFunction == eBusFunctionD2XX)
	{
		for (const auto& pinEntry : std::as_const(_pinEntries))
		{
			if (pinEntry._enabled == true)
			{
				if (pinEntry._bus == bus)
				{
					result.append(pinEntry);
				}
			}
		}
	}

	return result;
}

Pins _FT232HPlatformConfiguration::getPins()
{
	Pins result;

	FTDIPinList FTDIPinList = getActivePins();

	for (const auto& ftdiPin: FTDIPinList)
	{
		PinEntry pinEntry;

		pinEntry._pin = ftdiPin._setPin;
		pinEntry._enabled = ftdiPin._enabled;
		pinEntry._hash = ftdiPin._hash;
		pinEntry._cellLocation = ftdiPin._cellLocation;
		pinEntry._commandGroup = ftdiPin._commandGroup;
		pinEntry._pinCommand = ftdiPin._pinCommand;
		pinEntry._initialValue = ftdiPin._initialValue;
		pinEntry._inverted = ftdiPin._inverted;
		pinEntry._pinLabel = ftdiPin._pinLabel;
		pinEntry._pinTooltip = ftdiPin._pinTooltip;
		pinEntry._tabName = ftdiPin._tabName;

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

bool _FT232HPlatformConfiguration::getPinEnableState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	bool result{true};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._enabled;
	}

	return result;
}

void _FT232HPlatformConfiguration::setPinEnableState(HashType hash, bool newState)
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

void _FT232HPlatformConfiguration::setPinEnableState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, bool newState)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinEnableState(hash, newState);
}

bool _FT232HPlatformConfiguration::getPinInputState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	bool result{true};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._input;
	}

	return result;
}

void _FT232HPlatformConfiguration::setPinInputState(HashType hash, bool newState)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._input != newState)
		{
			pinData._input = newState;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update input state of an invalid pin!");
	}
}

void _FT232HPlatformConfiguration::setPinInputState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, bool newState)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinInputState(hash, newState);
}

bool _FT232HPlatformConfiguration::getInitialPinValue(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	bool result{false};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._initialValue;
	}

	return result;
}

void _FT232HPlatformConfiguration::setInitialPinValue(HashType hash, bool newState)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		if (pinData._initialValue != newState)
		{
			pinData._initialValue = newState;

			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update initialization state for an invalid pin!");
	}
}

void _FT232HPlatformConfiguration::setInitialPinValue(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, bool newState)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setInitialPinValue(hash, newState);
}

int _FT232HPlatformConfiguration::getPinInitializationPriority(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	int result{0};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto pinData = _pinEntries[hash];
		result = pinData._initializationPriority;
	}

	return result;
}

void _FT232HPlatformConfiguration::setPinInitializationPriority(HashType hash, int priority)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._initializationPriority != priority)
		{
			pinData._initializationPriority = priority;

			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update priority of an invalid pin!");
	}
}

void _FT232HPlatformConfiguration::setPinInitializationPriority(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, int priority)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinInitializationPriority(hash, priority);
}

bool _FT232HPlatformConfiguration::getPinInvertedState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	bool result{false};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._inverted;
	}

	return result;
}

void _FT232HPlatformConfiguration::setPinInvertedState(HashType hash, bool newState)
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

void _FT232HPlatformConfiguration::setPinInvertedState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, bool newState)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinInvertedState(hash, newState);
}

QString _FT232HPlatformConfiguration::getPinLabel(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	QString result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinLabel;
	}
	return result;
}

void _FT232HPlatformConfiguration::setPinLabel(HashType hash, const QString& pinLabel)
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

void _FT232HPlatformConfiguration::setPinLabel(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QString& pinLabel)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinLabel(hash, pinLabel);
}

QString _FT232HPlatformConfiguration::getPinTooltip(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	QString result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinTooltip;
	}

	return result;
}

void _FT232HPlatformConfiguration::setPinTooltip(HashType hash, const QString& pinTooltip)
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

void _FT232HPlatformConfiguration::setPinTooltip(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QString& pinTooltip)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinTooltip(hash, pinTooltip);
}

QString _FT232HPlatformConfiguration::getPinCommand(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	QString result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinCommand;
	}

	return result;
}

void _FT232HPlatformConfiguration::setPinCommand(HashType hash, const QString& pinCommand)
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

void _FT232HPlatformConfiguration::setPinCommand(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QString &pinCommand)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinCommand(hash, pinCommand);
}

CommandGroups _FT232HPlatformConfiguration::getPinGroup(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	CommandGroups result{eUnknownCommandGroup};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._commandGroup;
	}

	return result;
}

void _FT232HPlatformConfiguration::setPinGroup(HashType hash, const CommandGroups commandGroup)
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

void _FT232HPlatformConfiguration::setPinGroup(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const CommandGroups commandGroup)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinGroup(hash, commandGroup);
}

QString _FT232HPlatformConfiguration::getTabName(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	QString result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._tabName;
	}

	return result;
}

void _FT232HPlatformConfiguration::setTabName(HashType hash, const QString& tabName)
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

void _FT232HPlatformConfiguration::setTabName(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QString& tabName)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setTabName(hash, tabName);
}

QPoint _FT232HPlatformConfiguration::getPinCellLocation(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	QPoint result(-1,-1);

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._cellLocation;
	}

	return result;
}

void _FT232HPlatformConfiguration::setPinCellLocation(HashType hash, const QPoint& cellLocation)
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

void _FT232HPlatformConfiguration::setPinCellLocation(
		const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QPoint& cellLocation
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinCellLocation(hash, cellLocation);
}

FTDIBusData _FT232HPlatformConfiguration::getBusFunction(const ChipIndex chipIndex, const Bus& busName) const
{
	FTDIBusData result(chipIndex, busName, eBusFunctionUnknown);

	HashType hashType = FTDIBusData::makeFTDIHash(chipIndex, busName);

	if (_busFunctions.find(hashType) != _busFunctions.end())
	{
		result = _busFunctions[hashType];
	}
	return result;
}

void _FT232HPlatformConfiguration::setBusFunction(HashType hash, const FTDIBusFunction busFunction)
{
	if (_busFunctions.find(hash) != _busFunctions.end())
	{
		if (_busFunctions[hash]._busFunction != busFunction)
		{
			_busFunctions[hash]._busFunction = busFunction;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add platform function to an invalid bus!");
	}
}

void _FT232HPlatformConfiguration::setBusFunction(const ChipIndex chipIndex, const Bus& busName, const FTDIBusFunction busFunction)
{

	HashType hash = FTDIBusData::makeFTDIHash(chipIndex, busName);
	setBusFunction(hash, busFunction);
}

FTDIPinSets _FT232HPlatformConfiguration::getPinSet(const ChipIndex chipIndex)
{
	FTDIPinSets result{NoOptions};

	FTDIBusData FTDIBusData;

	FTDIBusData = getBusFunction(chipIndex, 'A');
	if (FTDIBusData._bus == 'A' && FTDIBusData._busFunction == eBusFunctionD2XX)
		result.setFlag(eA);

	FTDIBusData = getBusFunction(chipIndex, 'B');
	if (FTDIBusData._bus == 'B' && FTDIBusData._busFunction == eBusFunctionD2XX)
		result.setFlag(eB);

	FTDIBusData = getBusFunction(chipIndex, 'C');
	if (FTDIBusData._bus == 'C' && FTDIBusData._busFunction == eBusFunctionD2XX)
		result.setFlag(eC);

	FTDIBusData = getBusFunction(chipIndex, 'D');
	if (FTDIBusData._bus == 'D' && FTDIBusData._busFunction == eBusFunctionD2XX)
		result.setFlag(eD);

	return result;
}

void _FT232HPlatformConfiguration::cascadeTabDelete(const QString &deleteMe)
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

void _FT232HPlatformConfiguration::cascadeTabRename(const QString& oldName, const QString& newName)
{
	for (auto& pinEntry: _pinEntries)
	{
		if (pinEntry._tabName == oldName)
			pinEntry._tabName = newName;
	}
}

bool _FT232HPlatformConfiguration::read(QJsonObject& parentLevel)
{
	bool result{false};

	result = _PlatformConfiguration::read(parentLevel);
	if (result == true)
	{
		QJsonValue jsonValue;

		if (parentLevel.contains(kPinEntries) && parentLevel[kPinEntries].isArray())
		{
			QJsonArray pinsArray = parentLevel[kPinEntries].toArray();

			_pinEntries.clear();

			for (auto pinIndex: range(pinsArray.size()))
			{
				QJsonObject pinEntry = pinsArray[pinIndex].toObject();

				ChipIndex chipIndex{0};
				Bus bus;
				PinID pinId{0};

				if (pinEntry.contains(kChipIndex))
					chipIndex = pinEntry[kChipIndex].toInt();

				if (pinEntry.contains(kBus))
					bus = pinEntry[kBus].toString().at(0);

				if (pinEntry.contains(kPinNumber))
					pinId = pinEntry[kPinNumber].toString().toInt();

				FTDIPinData pinData(chipIndex, bus, pinId);

				pinData._setPin = getSetPinIndex(chipIndex, bus, pinId);

				if (pinEntry.contains(kEnabled))
					pinData._enabled = pinEntry[kEnabled].toBool();

				if (pinEntry.contains(kName))
					pinData._pinLabel = pinEntry[kName].toString();

				if (pinEntry.contains(kToolTip))
					pinData._pinTooltip = pinEntry[kToolTip].toString();

				if (pinEntry.contains(kInitialValue))
					pinData._initialValue = pinEntry[kInitialValue].toBool();

				if (pinEntry.contains(kPriority))
					pinData._initializationPriority = pinEntry[kPriority].toInt();

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

		if (parentLevel.contains(kBusEntries) && parentLevel[kBusEntries].isArray())
		{
			QJsonArray busArray = parentLevel[kBusEntries].toArray();

			_busFunctions.clear();

			for (auto pinIndex: range(busArray.size()))
			{
				QJsonObject busEntry = busArray[pinIndex].toObject();

				ChipIndex chipIndex{0};
				Bus bus;
				FTDIBusFunction busFunction{eBusFunctionUnknown};

				if (busEntry.contains(kChipIndex))
					chipIndex = busEntry[kChipIndex].toInt();

				if (busEntry.contains(kBus))
					bus = busEntry[kBus].toString().at(0);

				if (busEntry.contains(kBusFunction))
					busFunction = FTDIBusFunction(busEntry[kBusFunction].toInt());

				FTDIBusData busData(chipIndex, bus, busFunction);

				_busFunctions[busData._hash] = busData;
			}
		}
	}

	return result;
}

void _FT232HPlatformConfiguration::write(QJsonObject& parentLevel)
{
	_PlatformConfiguration::write(parentLevel);


	QJsonArray pinsArray;

	for (auto& pinEntry : _pinEntries)
	{
		QJsonObject pinData;

		pinData[kChipIndex] = 0;
		pinData[kBus] = QVariant(pinEntry._bus).toString();
		pinData[kPinNumber] = QString::number(pinEntry._chipPin);
		pinData[kEnabled] = pinEntry._enabled;
		pinData[kInput] = pinEntry._input;
		pinData[kName] = pinEntry._pinLabel;
		pinData[kToolTip] = pinEntry._pinTooltip;
		pinData[kInitialValue] = pinEntry._initialValue;
		pinData[kPriority] = pinEntry._initializationPriority;
		pinData[kInverted] = pinEntry._inverted;
		pinData[kCommand] = pinEntry._pinCommand;
		pinData[kCommandGroup] = pinEntry._commandGroup;
		pinData[kRunPriority] = fromQPoint(pinEntry._cellLocation);

		pinData[kTabName] = pinEntry._tabName;

		pinsArray.append(pinData);
	}

	parentLevel[kPinEntries] = pinsArray;

	QJsonArray busArray;

	for (const auto& busFunction : std::as_const(_busFunctions))
	{
		QJsonObject busData;

		busData[kChipIndex] = busFunction._chipIndex;
		busData[kBus] = QVariant(busFunction._bus).toString();
		busData[kBusFunction] = busFunction._busFunction;

		busArray.append(busData);
	}

	parentLevel[kBusEntries] = busArray;
}

PinID _FT232HPlatformConfiguration::getSetPinIndex(const int chipIndex, const Bus &bus, PinID pinId)
{
	int pinIndex{0};

	switch (bus.toLatin1())
	{
	case 'A': pinIndex = 0; break;
	case 'B': pinIndex = 8; break;
	case 'C': pinIndex = 16; break;
	case 'D': pinIndex = 24; break;
	}

	pinIndex += pinId;
	pinIndex *= (chipIndex + 1);

	return static_cast<PinID>(pinIndex);
}
