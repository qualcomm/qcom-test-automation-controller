// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PSOCPlatformConfiguration.h"

#include "PlatformConfigurationException.h"

// QCommon
#include "Range.h"
#include "RangedContainer.h"

// Qt
#include <QJsonObject>
#include <QJsonArray>
#include <QHash>

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
const QString kVariant(QStringLiteral("variant"));


PSOCPinEntries _PSOCPlatformConfiguration::_classicActions;
PSOCI2CEntries _PSOCPlatformConfiguration::_classicI2CActions;
PSOCI2CSlaves _PSOCPlatformConfiguration::_classicSlaveConfigs;

const QString kI2CSlaves(QStringLiteral("slaves"));
const QString kRegAddress(QStringLiteral("reg_addr"));
const QString kSlaveVariant(QStringLiteral("variant"));
const QString kPortCount(QStringLiteral("port_count"));

const QString kI2CAddress(QStringLiteral("i2c_addr"));
const QString kSlaveAddress(QStringLiteral("slave_addr"));
const QString kConfigAddress(QStringLiteral("config_addr"));
const QString kWriteAddress(QStringLiteral("write_addr"));


_PSOCPlatformConfiguration::_PSOCPlatformConfiguration(PSOCVariant psocVariant)
{
	_platform = ePSOC;
	_platformId = kMaxPSOCPlatformId;
	_variant = psocVariant;

	_resetActive = true;

	if (_classicActions.empty())
		_PSOCPlatformConfiguration::initialize(psocVariant);

	for (auto [pinNumber, pinData] : RangedContainer(_PSOCPlatformConfiguration::_classicActions))
		_pinEntries[pinData._pin] = pinData;

	if (psocVariant == ePSOCGPIOIIC)
		_slaveConfigs = _PSOCPlatformConfiguration::_classicSlaveConfigs;

	// Add default tabs to _editorTabs
	Tab generalTab, deviceInfoTab, fusionTab;

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

	fusionTab._name = "Fusion";
	fusionTab._moveable = true;
	fusionTab._visible = false;
	fusionTab._configurable = true;
	fusionTab._ordinal = 2;
	fusionTab._userTab = true;

	_tabs.append(generalTab);
	_tabs.append(deviceInfoTab);

	if (psocVariant == ePSOCGPIOIIC)
	{
		refreshSlaveConfig();
	}

	_tabs.append(fusionTab);

	if (psocVariant == ePSOCGPIOIIC)
	{
		_supportedFirmwareVer.append(18);
		_supportedFirmwareVer.append(19);
	}
	else
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

Pins _PSOCPlatformConfiguration::getI2CPinEntries()
{
	Pins result;

	PSOCI2CEntries activeEntries = getActiveI2CEntries();

	for (const auto& i2cEntry: activeEntries)
	{
		PinEntry pinEntry;

		pinEntry._pin = i2cEntry._pin;
		pinEntry._enabled = i2cEntry._enabled;
		pinEntry._hash = i2cEntry._hash;
		pinEntry._cellLocation = i2cEntry._cellLocation;
		pinEntry._commandGroup = i2cEntry._commandGroup;
		pinEntry._pinCommand = i2cEntry._pinCommand;
		pinEntry._inverted = i2cEntry._inverted;
		pinEntry._pinLabel = i2cEntry._pinLabel;
		pinEntry._pinTooltip = i2cEntry._pinTooltip;
		pinEntry._tabName = i2cEntry._tabName;

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

PSOCVariant _PSOCPlatformConfiguration::variant()
{
	return _variant;
}

void _PSOCPlatformConfiguration::setVariant(PSOCVariant psocVariant)
{
	_variant = psocVariant;
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

PSOCI2CSlaves _PSOCPlatformConfiguration::getSlaveConfigs() const
{
	return _slaveConfigs;
}

void _PSOCPlatformConfiguration::setSlaveConfigs(PSOCI2CSlaves& slaves)
{
	_slaveConfigs.clear();

	for (auto& sc : slaves)
		_slaveConfigs.append(sc);

	refreshSlaveConfig();
	_dirty = true;
}

void _PSOCPlatformConfiguration::addSlaveConfig(const PSOCI2CSlave& slave)
{
	for (auto& sc : _slaveConfigs)
	{
		if (sc._slaveAddress == slave._slaveAddress)
		{
			sc = slave;
			refreshSlaveConfig();
			_dirty = true;
			return;
		}
	}
	_slaveConfigs.append(slave);
	refreshSlaveConfig();
	_dirty = true;
}

void _PSOCPlatformConfiguration::removeSlaveConfig(PinID slaveAddress)
{
	for (int i = 0; i < _slaveConfigs.size(); ++i)
	{
		if (_slaveConfigs.at(i)._slaveAddress == slaveAddress)
		{
			_slaveConfigs.removeAt(i);
			refreshSlaveConfig();
			_dirty = true;
			return;
		}
	}
}

QString _PSOCPlatformConfiguration::i2cSlaveTabName(const PSOCI2CSlave& slave)
{
	return QString("%1 (0x%2)").arg(psocSlaveToString(slave._variant)).arg(slave._slaveAddress, 2, 16, QChar('0'));
}

void _PSOCPlatformConfiguration::refreshSlaveConfig()
{
	_i2cEntries.clear();

	for (auto [classicPin, classicEntry] : RangedContainer(_classicI2CActions))
	{
		for (const auto& slave : std::as_const(_slaveConfigs))
		{
			if (psocSlaveToString(slave._variant) != classicEntry._tabName)
				continue;

			PSOCI2CData entry = classicEntry;

			entry._slaveAddress = slave._slaveAddress;
			entry._writeAddress = slave._configAddress;
			entry._hash = entry.makeHash();
			entry._tabName = i2cSlaveTabName(slave);

			_i2cEntries.insert(entry._hash, entry);
		}
	}

	rebuildI2CTabs();
}

void _PSOCPlatformConfiguration::rebuildI2CTabs()
{
	QStringList knownVariants;
	knownVariants << psocSlaveToString(eKTS1622EUAATR) << psocSlaveToString(eTCA9534APWR);

	auto isI2CTab = [&knownVariants] (const QString& tabName) -> bool
	{
		for (const auto& variant : std::as_const(knownVariants))
		{
			if (tabName.startsWith(variant + " (0x") && tabName.endsWith(')'))
				return true;
		}

		return false;
	};

	for (int idx = _tabs.size() - 1; idx >= 0; --idx)
	{
		if (isI2CTab(_tabs.at(idx)._name))
			_tabs.removeAt(idx);
	}

	int ordinal = 2;
	for (const auto& slave : std::as_const(_slaveConfigs))
	{
		Tab i2cTab;

		i2cTab._name = i2cSlaveTabName(slave);
		i2cTab._moveable = true;
		i2cTab._visible = true;
		i2cTab._configurable = true;
		i2cTab._ordinal = ordinal++;
		i2cTab._userTab = true;

		_tabs.append(i2cTab);
	}
}

PSOCI2CEntries _PSOCPlatformConfiguration::getI2CEntries() const
{
	return _i2cEntries;
}

PSOCI2CEntries _PSOCPlatformConfiguration::getActiveI2CEntries() const
{
	PSOCI2CEntries result;
	for (auto it = _i2cEntries.constBegin(); it != _i2cEntries.constEnd(); ++it)
	{
		if (it.value()._enabled)
			result.insert(it.key(), it.value());
	}
	return result;
}

QList<PSOCI2CData> _PSOCPlatformConfiguration::getI2CEntriesForSlave(const PSOCI2CSlave& slave) const
{
	QString tabName = i2cSlaveTabName(slave);

	QList<PSOCI2CData> result;
	for (const auto& i2cData : std::as_const(_i2cEntries))
	{
		if (i2cData._tabName == tabName && i2cData._slaveAddress == slave._slaveAddress)
			result.append(i2cData);
	}

	auto sortByPin = [] (const PSOCI2CData& p1, const PSOCI2CData& p2) -> bool
	{
		return p1._pin < p2._pin;
	};
	std::sort(result.begin(), result.end(), sortByPin);

	int rowCount = slave._portCount * 8;
	if (result.count() > rowCount)
		result = result.mid(0, rowCount);

	return result;
}

bool _PSOCPlatformConfiguration::addI2CSlave(const PSOCI2CData& i2cData)
{
	PSOCI2CData entry = i2cData;
	entry._hash = entry.makeHash();
	_i2cEntries.insert(entry._hash, entry);
	_dirty = true;
	return true;
}

void _PSOCPlatformConfiguration::removeI2CSlave(HashType hash)
{
	if (_i2cEntries.remove(hash) > 0)
		_dirty = true;
}

PSOCI2CData _PSOCPlatformConfiguration::getI2CSlave(HashType hash) const
{
	return _i2cEntries.value(hash);
}

bool _PSOCPlatformConfiguration::getI2CPinEnableState(const HashType hash) const
{
	bool result{true};
	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		result = _i2cEntries.value(hash)._enabled;
	}

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinEnableState(const HashType hash, bool newState)
{
	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		if (_i2cEntries[hash]._enabled != newState)
		{
			_i2cEntries[hash]._enabled = newState;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update state of an invalid I2C pin!");
	}
}

bool _PSOCPlatformConfiguration::getI2CPinInvertedState(const HashType hash) const
{
	bool result{kDefaultPinInvertedState};

	if (_i2cEntries.find(hash) != _i2cEntries.end())
		result = _i2cEntries[hash]._inverted;

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinInvertedState(const HashType hash, bool newState)
{
	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		_i2cEntries[hash]._inverted = newState;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set invert state for an invalid I2C pin!");
	}
}

QString _PSOCPlatformConfiguration::getI2CPinLabel(const HashType hash) const
{
	QString result;

	if (_i2cEntries.find(hash) != _i2cEntries.end())
		result = _i2cEntries[hash]._pinLabel;

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinLabel(const HashType hash, const QString& pinLabel)
{
	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		_i2cEntries[hash]._pinLabel = pinLabel;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set pin label for an invalid I2C pin!");
	}
}

QString _PSOCPlatformConfiguration::getI2CPinTooltip(const HashType hash) const
{
	QString result;

	if (_i2cEntries.find(hash) != _i2cEntries.end())
		result = _i2cEntries[hash]._pinTooltip;

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinTooltip(const HashType hash, const QString& pinTooltip)
{
	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		_i2cEntries[hash]._pinTooltip = pinTooltip;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set tooltip for an invalid I2C pin!");
	}
}

QString _PSOCPlatformConfiguration::getI2CPinCommand(const HashType hash) const
{
	QString result;

	if (_i2cEntries.find(hash) != _i2cEntries.end())
		result = _i2cEntries[hash]._pinCommand;

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinCommand(const HashType hash, const QString& pinCommand)
{
	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		_i2cEntries[hash]._pinCommand = pinCommand;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set command for an invalid I2C pin!");
	}
}

CommandGroups _PSOCPlatformConfiguration::getI2CPinGroup(const HashType hash) const
{
	CommandGroups result{kDefaultCommandGroup};

	if (_i2cEntries.find(hash) != _i2cEntries.end())
		result = _i2cEntries[hash]._commandGroup;

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinGroup(const HashType hash, const CommandGroups commandGroup)
{
	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		_i2cEntries[hash]._commandGroup = commandGroup;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set pin category for an invalid I2C pin!");
	}
}

QString _PSOCPlatformConfiguration::getI2CTabName(const HashType hash) const
{
	QString result;

	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		result = _i2cEntries[hash]._tabName;
	}

	return result;
}

void _PSOCPlatformConfiguration::setI2CTabName(const HashType hash, const QString& tabName)
{
	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		_i2cEntries[hash]._tabName = tabName;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set tab name for an invalid I2C pin!");
	}
}

QPoint _PSOCPlatformConfiguration::getI2CPinCellLocation(const HashType hash) const
{
	QPoint result{kDefaultCellLocation};

	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		result = _i2cEntries[hash]._cellLocation;
	}

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinCellLocation(const HashType hash, const QPoint& cellLocation)
{
	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		_i2cEntries[hash]._cellLocation = cellLocation;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set cell location for an invalid I2C pin!");
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

	jsonValue = parentLevel.value(kVariant);

	if (jsonValue.isNull() == false)
		setVariant(PSOCVariant(jsonValue.toInt()));

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
		{
			if (_variant == ePSOCGPIOIIC)
			{
				_supportedFirmwareVer.append(18);
				_supportedFirmwareVer.append(19);
			}
			else
				_supportedFirmwareVer.append(kDefaultFirmwareVersion);
		}
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

	// Read i2c_slaves (slave config descriptors)
	QJsonArray slaveConfigArray = parentLevel.value(kI2CSlaves).toArray();
	if (!slaveConfigArray.isEmpty())
	{
		_slaveConfigs.clear();
		for (auto slaveIndex: range(slaveConfigArray.count()))
		{
			QJsonObject slaveData = slaveConfigArray.at(slaveIndex).toObject();
			PSOCI2CSlave slave;
			slave._variant = PSOCIICVariant(slaveData.value(kSlaveVariant).toInt());
			slave._slaveAddress = static_cast<PinID>(slaveData.value(kRegAddress).toInt());
			slave._configAddress = static_cast<PinID>(slaveData.value(kConfigAddress).toInt());
			slave._portCount = slaveData.value(kPortCount).toInt();
			_slaveConfigs.append(slave);
		}

		rebuildI2CTabs();
	}

	// Read i2c_addr (per-pin I2C entries)
	QJsonArray i2cDataArray = parentLevel.value(kI2CAddress).toArray();
	if (!i2cDataArray.isEmpty())
	{
		_i2cEntries.clear();
		for (auto i2cIndex: range(i2cDataArray.count()))
		{
			QJsonObject i2cData = i2cDataArray.at(i2cIndex).toObject();
			PSOCI2CData i2cEntry;

			if (i2cData.contains(kPinNumber))
				i2cEntry._pin = i2cData.value(kPinNumber).toString().toULongLong();
			if (i2cData.contains(kSlaveAddress))
				i2cEntry._slaveAddress = static_cast<PinID>(i2cData.value(kSlaveAddress).toInt());
			if (i2cData.contains(kWriteAddress))
				i2cEntry._writeAddress = static_cast<PinID>(i2cData.value(kWriteAddress).toInt());
			if (i2cData.contains(kEnabled))
				i2cEntry._enabled = i2cData.value(kEnabled).toBool();
			if (i2cData.contains(kName))
				i2cEntry._pinLabel = i2cData.value(kName).toString();
			if (i2cData.contains(kToolTip))
				i2cEntry._pinTooltip = i2cData.value(kToolTip).toString();
			if (i2cData.contains(kCommand))
				i2cEntry._pinCommand = i2cData.value(kCommand).toString();
			if (i2cData.contains(kCommmandGroup))
				i2cEntry._commandGroup = CommandGroups(i2cData.value(kCommmandGroup).toInt());
			if (i2cData.contains(kClassicAction))
				i2cEntry._classicAction = i2cData.value(kClassicAction).toString();
			if (i2cData.contains(kTabName))
				i2cEntry._tabName = i2cData.value(kTabName).toString();
			if (i2cData.contains(kRunPriority))
				i2cEntry._cellLocation = toQPoint(i2cData.value(kRunPriority).toString());
			if (i2cData.contains(kInverted))
				i2cEntry._inverted = i2cData.value(kInverted).toBool();

			i2cEntry._hash = i2cEntry.makeHash();
			_i2cEntries.insert(i2cEntry._hash, i2cEntry);
		}

		// Older config files stored _tabName as the bare variant string (e.g. "TCA9534APWR")
		// rather than the per-slave name (e.g. "TCA9534APWR (0x38)"). Migrate those entries so
		// getI2CEntriesForSlave() (which now matches on the per-slave name) still finds them.
		for (auto& i2cEntry : _i2cEntries)
		{
			for (const auto& slave : std::as_const(_slaveConfigs))
			{
				if (i2cEntry._slaveAddress == slave._slaveAddress && i2cEntry._tabName == psocSlaveToString(slave._variant))
				{
					i2cEntry._tabName = i2cSlaveTabName(slave);
					break;
				}
			}
		}
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

	if (_variant == ePSOCGPIOIIC)
	{
		// Write i2c_slaves (slave config descriptors)
		QJsonArray jsonSlaveConfigs;
		for (const auto& sc : std::as_const(_slaveConfigs))
		{
			QJsonObject slaveData;
			slaveData.insert(kSlaveVariant, static_cast<int>(sc._variant));
			slaveData.insert(kRegAddress, static_cast<int>(sc._slaveAddress));
			slaveData.insert(kConfigAddress, static_cast<int>(sc._configAddress));
			slaveData.insert(kPortCount, sc._portCount);
			jsonSlaveConfigs.append(slaveData);
		}
		parentLevel[kI2CSlaves] = jsonSlaveConfigs;

		// Write i2c_addr (per-pin I2C entries)
		QJsonArray jsonI2CData;
		for (const auto& i2cEntry : std::as_const(_i2cEntries))
		{
			QJsonObject i2cData;
			i2cData.insert(kPinNumber, QString::number(i2cEntry._pin));
			i2cData.insert(kSlaveAddress, static_cast<int>(i2cEntry._slaveAddress));
			i2cData.insert(kWriteAddress, static_cast<int>(i2cEntry._writeAddress));
			i2cData.insert(kEnabled, i2cEntry._enabled);
			i2cData.insert(kName, i2cEntry._pinLabel);
			i2cData.insert(kToolTip, i2cEntry._pinTooltip);
			i2cData.insert(kCommand, i2cEntry._pinCommand);
			i2cData.insert(kCommmandGroup, i2cEntry._commandGroup);
			i2cData.insert(kClassicAction, i2cEntry._classicAction);
			i2cData.insert(kTabName, i2cEntry._tabName);
			i2cData.insert(kRunPriority, fromQPoint(i2cEntry._cellLocation));
			i2cData.insert(kInverted, i2cEntry._inverted);
			jsonI2CData.append(i2cData);
		}
		parentLevel[kI2CAddress] = jsonI2CData;
	}
}

void _PSOCPlatformConfiguration ::initialize(PSOCVariant psocVariant)
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

	//--------------

	if (psocVariant == ePSOCGPIOIIC)
	{
		PSOCI2CData i2cData;

		i2cData.clear();
		i2cData._pin = 0;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x02;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "CSI0 MUX Select";
		i2cData._pinCommand = "csi0";
		i2cData._classicAction = "CSI0 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "High routes CSI to Camera B2B connector, low routes to 22-pin camera connector";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(0, 0);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 1;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x02;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "CSI1 MUX Select";
		i2cData._pinCommand = "csi1";
		i2cData._classicAction = "CSI1 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Chip-select mux for CSI1 camera path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(1, 0);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 2;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x02;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "CSI2 MUX Select";
		i2cData._pinCommand = "csi2";
		i2cData._classicAction = "CSI2 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Chip-select mux for CSI2 camera path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(0, 1);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 3;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x02;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "CSI3 MUX Select";
		i2cData._pinCommand = "csi3";
		i2cData._classicAction = "CSI3 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Chip-select mux for CSI3 camera path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(1, 1);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 4;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x02;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "DSI0 MUX Select";
		i2cData._pinCommand = "dsi0";
		i2cData._classicAction = "DSI0 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "High routes DSI to Display B2B connector, low routes to 22-pin display connector";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(0, 2);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 5;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x02;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "DSI1 MUX Select";
		i2cData._pinCommand = "dsi1";
		i2cData._classicAction = "DSI1 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Display mux select for DSI1 path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(1, 2);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 6;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x02;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "HDMI I2S MUX Select";
		i2cData._pinCommand = "hdmi";
		i2cData._classicAction = "HDMI I2S MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "High routes I2S to eARC TX IC, low routes to eARC RX IC";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(0, 3);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 7;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x02;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "PCIE MUX Select";
		i2cData._pinCommand = "pcie";
		i2cData._classicAction = "PCIE MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "High routes PCIe to M.2 KEY-M connector, low routes to M.2 KEY-B connector";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(1, 3);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 8;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x03;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "I2S0 MUX Select";
		i2cData._pinCommand = "i2s0";
		i2cData._classicAction = "I2S0 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Routes I2S0 between Codec path and DSI/HDMI bridge IC path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(0, 4);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 9;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x03;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "I2S1 MUX Select";
		i2cData._pinCommand = "i2s1";
		i2cData._classicAction = "I2S1 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Routes I2S1 between DSI/HDMI bridge IC path and HDMI eARC path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(1, 4);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 10;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x03;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "I2S2 MUX Select";
		i2cData._pinCommand = "i2s2";
		i2cData._classicAction = "I2S2 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Routes I2S2 between DMIC path and HAT connector path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(0, 5);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 11;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x03;
		i2cData._hash = i2cData.makeHash();
		i2cData._enabled = false;
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 12;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x03;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "USB0 MUX Select";
		i2cData._pinCommand = "usb0i2c";
		i2cData._classicAction = "USB0 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Routes USB0 between TYPE-C path and USB B2B connector path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(1, 5);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 13;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x03;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "USB1 MUX Select";
		i2cData._pinCommand = "usb1i2c";
		i2cData._classicAction = "USB1 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Routes USB1 between TYPE-C path and USB B2B connector path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(0, 6);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 14;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x03;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "SIM MUX Select";
		i2cData._pinCommand = "sim";
		i2cData._classicAction = "SIM MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Routes SIM lines between M.2 KEY-B path and SIM connector path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(1, 6);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 15;
		i2cData._slaveAddress = 0x20;
		i2cData._writeAddress = 0x03;
		i2cData._hash = i2cData.makeHash();
		i2cData._enabled = false;
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "KTS1622EUAATR";
		i2cData._inverted = false;
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 16;
		i2cData._slaveAddress = 0x38;
		i2cData._writeAddress = 0x01;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "SPIO MUX Select";
		i2cData._pinCommand = "spi0";
		i2cData._classicAction = "SPIO MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Routes SPI0 between camera/sensor path and Display B2B connector path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "TCA9534APWR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(0, 0);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 17;
		i2cData._slaveAddress = 0x38;
		i2cData._writeAddress = 0x01;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "SPI1 MUX Select";
		i2cData._pinCommand = "spi1";
		i2cData._classicAction = "SPI1 MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Routes SPI1 between GMSL camera path and HAT connector path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "TCA9534APWR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(1, 0);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 18;
		i2cData._slaveAddress = 0x38;
		i2cData._writeAddress = 0x01;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "NFC MUX Select";
		i2cData._pinCommand = "nfc";
		i2cData._classicAction = "NFC MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Routes NFC between HAT connector path and NFC B2B connector path";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "TCA9534APWR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(0, 1);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		i2cData.clear();
		i2cData._pin = 19;
		i2cData._slaveAddress = 0x38;
		i2cData._writeAddress = 0x01;
		i2cData._hash = i2cData.makeHash();
		i2cData._pinLabel = "EARC TX_RX MUX Select";
		i2cData._pinCommand = "earc";
		i2cData._classicAction = "EARC TX_RX MUX Select";
		i2cData._enabled = true;
		i2cData._pinTooltip = "Selects eARC TX or RX mode";
		i2cData._commandGroup = eSwitchGroup;
		i2cData._tabName = "TCA9534APWR";
		i2cData._inverted = false;
		i2cData._cellLocation = QPoint(1, 1);
		_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;

		for (int p = 20; p < 24; ++p)
		{
			i2cData.clear();
			i2cData._pin = static_cast<PinID>(p);
			i2cData._slaveAddress = 0x38;
			i2cData._writeAddress = 0x01;
			i2cData._hash = i2cData.makeHash();
			i2cData._enabled = false;
			i2cData._commandGroup = eSwitchGroup;
			i2cData._tabName = "TCA9534APWR";
			i2cData._inverted = false;
			_PSOCPlatformConfiguration::_classicI2CActions[i2cData._pin] = i2cData;
		}

		if (_classicSlaveConfigs.isEmpty())
		{
			PSOCI2CSlave slave;

			slave._variant = eKTS1622EUAATR;
			slave._slaveAddress = 0x20;
			slave._configAddress = 0x02;
			slave._portCount = 2;
			_classicSlaveConfigs.append(slave);

			slave._variant = eTCA9534APWR;
			slave._slaveAddress = 0x38;
			slave._configAddress = 0x01;
			slave._portCount = 1;
			_classicSlaveConfigs.append(slave);
		}
	}
}
