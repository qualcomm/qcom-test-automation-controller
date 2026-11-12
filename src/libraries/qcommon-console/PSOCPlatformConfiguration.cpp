// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PSOCPlatformConfiguration.h"

#include "PlatformConfigurationException.h"

// QCommon
#include "Range.h"
#include "RangedContainer.h"

#include "ConfigurationFile.h"
#include <cstdio>

const std::string kPlatformEntries("pins");
const std::string kTabOrderData("tabs");

const bool kDefaultInitialPinValue(false);
const bool kDefaultPinInvertedState(false);
const CommandGroups kDefaultCommandGroup{eUnknownCommandGroup};
const std::pair<int, int> kDefaultCellLocation(-1,-1);

const std::string kRunPriority("run_priority");
const std::string kPinNumber("pin_number");
const std::string kEnabled("enabled");
const std::string kIntialValue("initial_value");
const std::string kPriority("initialization_priority");
const std::string kInverted("inverted");
const std::string kName("name");
const std::string kToolTip("help_hint");
const std::string kCommand("command");
const std::string kCommmandGroup("command_group");
const std::string kClassicAction("classic_action");
const std::string kTabName("tab_name");
const std::string kMinFirmwareVersion("supportedFirmwareVer");
const std::string kVariant("variant");


PSOCPinEntries _PSOCPlatformConfiguration::_classicActions;
PSOCI2CEntries _PSOCPlatformConfiguration::_classicI2CActions;
PSOCI2CSlaves _PSOCPlatformConfiguration::_classicSlaveConfigs;

const std::string kI2CSlaves("slaves");
const std::string kRegAddress("reg_addr");
const std::string kSlaveVariant("variant");
const std::string kPortCount("port_count");

const std::string kI2CAddress("i2c_addr");
const std::string kSlaveAddress("slave_addr");
const std::string kConfigAddress("config_addr");
const std::string kWriteAddress("write_addr");


_PSOCPlatformConfiguration::_PSOCPlatformConfiguration(PSOCVariant psocVariant)
{
	_platform = ePSOC;
	_platformId = kMaxPSOCPlatformId;
	_variant = psocVariant;

	_resetActive = true;

	if (_classicActions.empty())
		_PSOCPlatformConfiguration::initialize(psocVariant);

	for (const auto& [pinNumber, pinData] : _PSOCPlatformConfiguration::_classicActions)
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

	_tabs.push_back(generalTab);
	_tabs.push_back(deviceInfoTab);

	if (psocVariant == ePSOCGPIOIIC)
	{
		refreshSlaveConfig();
	}

	_tabs.push_back(fusionTab);

	if (psocVariant == ePSOCGPIOIIC)
	{
		_supportedFirmwareVer.push_back(18);
		_supportedFirmwareVer.push_back(19);
	}
	else
		_supportedFirmwareVer.push_back(kDefaultFirmwareVersion);
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

	for (const auto& [pin, pinData]: _pinEntries)
	{
		pinList.push_back(pinData);
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

	for (const auto& [pin, pinData]: _pinEntries)
	{
		if (pinData._enabled == true)
			pinList.push_back(pinData);
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

uint64_t _PSOCPlatformConfiguration::getPinInitializationPriority(const PinID pinId) const
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

std::string _PSOCPlatformConfiguration::getPinLabel(const PinID pinId) const
{
	std::string result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._pinLabel;

	return result;
}

void _PSOCPlatformConfiguration::setPinLabel(const PinID pinId, const std::string& pinLabel)
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

std::string _PSOCPlatformConfiguration::getPinTooltip(const PinID pinId) const
{
	std::string result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._pinTooltip;

	return result;
}

void _PSOCPlatformConfiguration::setPinTooltip(const PinID pinId, const std::string& pinTooltip)
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

std::string _PSOCPlatformConfiguration::getPinCommand(const PinID pinId) const
{
	std::string result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._pinCommand;

	return result;
}

void _PSOCPlatformConfiguration::setPinCommand(const PinID pinId, const std::string& pinCommand)
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

std::string _PSOCPlatformConfiguration::getClassicAction(const PinID pinId) const
{
	std::string result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
		result = _pinEntries[pinId]._classicAction;

	return result;
}

void _PSOCPlatformConfiguration::setClassicAction(const PinID pinId, const std::string& classicAction)
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

std::pair<int, int> _PSOCPlatformConfiguration::getPinCellLocation(const PinID pinId) const
{
	std::pair<int, int> result{kDefaultCellLocation};

	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		result = _pinEntries[pinId]._cellLocation;
	}

	return result;
}

void _PSOCPlatformConfiguration::setPinCellLocation(const PinID pinId, const std::pair<int, int>& cellLocation)
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
		_slaveConfigs.push_back(sc);

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
	_slaveConfigs.push_back(slave);
	refreshSlaveConfig();
	_dirty = true;
}

void _PSOCPlatformConfiguration::removeSlaveConfig(PinID slaveAddress)
{
	for (size_t i = 0; i < _slaveConfigs.size(); ++i)
	{
		if (_slaveConfigs.at(i)._slaveAddress == slaveAddress)
		{
			_slaveConfigs.erase(_slaveConfigs.begin() + i);
			refreshSlaveConfig();
			_dirty = true;
			return;
		}
	}
}

std::string _PSOCPlatformConfiguration::i2cSlaveTabName(const PSOCI2CSlave& slave)
{
	char hexBuf[16];
	std::snprintf(hexBuf, sizeof(hexBuf), "%02x", static_cast<unsigned int>(slave._slaveAddress));
	return psocSlaveToString(slave._variant) + " (0x" + std::string(hexBuf) + ")";
}

void _PSOCPlatformConfiguration::refreshSlaveConfig()
{
	_i2cEntries.clear();

	for (const auto& [classicPin, classicEntry] : _classicI2CActions)
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

static bool isI2CTab(const std::string& tabName)
{
	static const std::string kKnownVariants[] = {psocSlaveToString(eKTS1622EUAATR), psocSlaveToString(eTCA9534APWR)};

	for (const auto& variant : kKnownVariants)
	{
		std::string prefix = variant + " (0x";
		if (tabName.size() >= prefix.size() && tabName.compare(0, prefix.size(), prefix) == 0 && tabName.back() == ')')
			return true;
	}

	return false;
}

void _PSOCPlatformConfiguration::rebuildI2CTabs()
{
	for (int idx = static_cast<int>(_tabs.size()) - 1; idx >= 0; --idx)
	{
		if (isI2CTab(_tabs.at(idx)._name))
			_tabs.erase(_tabs.begin() + idx);
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

		_tabs.push_back(i2cTab);
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

std::vector<PSOCI2CData> _PSOCPlatformConfiguration::getI2CEntriesForSlave(const PSOCI2CSlave& slave) const
{
	std::string tabName = i2cSlaveTabName(slave);

	std::vector<PSOCI2CData> result;
	for (const auto& [hash, i2cData] : _i2cEntries)
	{
		if (i2cData._tabName == tabName && i2cData._slaveAddress == slave._slaveAddress)
			result.push_back(i2cData);
	}

	auto sortByPin = [] (const PSOCI2CData& p1, const PSOCI2CData& p2) -> bool
	{
		return p1._pin < p2._pin;
	};
	std::sort(result.begin(), result.end(), sortByPin);

	size_t rowCount = static_cast<size_t>(slave._portCount * 8);
	if (result.size() > rowCount)
		result.resize(rowCount);

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

std::string _PSOCPlatformConfiguration::getI2CPinLabel(const HashType hash) const
{
	std::string result;

	if (_i2cEntries.find(hash) != _i2cEntries.end())
		result = _i2cEntries[hash]._pinLabel;

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinLabel(const HashType hash, const std::string& pinLabel)
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

std::string _PSOCPlatformConfiguration::getI2CPinTooltip(const HashType hash) const
{
	std::string result;

	if (_i2cEntries.find(hash) != _i2cEntries.end())
		result = _i2cEntries[hash]._pinTooltip;

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinTooltip(const HashType hash, const std::string& pinTooltip)
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

std::string _PSOCPlatformConfiguration::getI2CPinCommand(const HashType hash) const
{
	std::string result;

	if (_i2cEntries.find(hash) != _i2cEntries.end())
		result = _i2cEntries[hash]._pinCommand;

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinCommand(const HashType hash, const std::string& pinCommand)
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

std::string _PSOCPlatformConfiguration::getI2CTabName(const HashType hash) const
{
	std::string result;

	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		result = _i2cEntries[hash]._tabName;
	}

	return result;
}

void _PSOCPlatformConfiguration::setI2CTabName(const HashType hash, const std::string& tabName)
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

std::pair<int, int> _PSOCPlatformConfiguration::getI2CPinCellLocation(const HashType hash) const
{
	std::pair<int, int> result{kDefaultCellLocation};

	if (_i2cEntries.find(hash) != _i2cEntries.end())
	{
		result = _i2cEntries[hash]._cellLocation;
	}

	return result;
}

void _PSOCPlatformConfiguration::setI2CPinCellLocation(const HashType hash, const std::pair<int, int>& cellLocation)
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

void _PSOCPlatformConfiguration::cascadeTabDelete(const std::string &deleteMe)
{
	for (auto idx = _tabs.begin(); idx != _tabs.end(); )
	{
		if (idx->_name == deleteMe)
			idx = _tabs.erase(idx);
		else
			++idx;
	}

	for (auto& pinEntry: _pinEntries)
	{
		if (pinEntry._tabName == deleteMe)
			pinEntry._tabName.clear();
	}
}

void _PSOCPlatformConfiguration::cascadeTabRename
(
	const std::string& oldName,
	const std::string& newName
)
{
	for (auto& pinEntry: _pinEntries)
	{
		if (pinEntry._tabName == oldName)
			pinEntry._tabName = newName;
	}
}

std::string _PSOCPlatformConfiguration::getTabName(const PinID pinId) const
{
	std::string result;

	if (_pinEntries.find(pinId) != _pinEntries.end())
	{
		result = _pinEntries[pinId]._tabName;
	}

	return result;
}

void _PSOCPlatformConfiguration::setTabName(const PinID pinId, const std::string pinTab)
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

bool _PSOCPlatformConfiguration::readConfig(const ConfigurationFile &config)
{
	bool result{false};

	result = _PlatformConfiguration::readConfig(config);

	if (config.hasProperty(kVariant))
		setVariant(PSOCVariant(config.getInt(kVariant)));

	_supportedFirmwareVer.clear();
	if (config.hasIntList(kMinFirmwareVersion))
	{
		const std::vector<int>& fwList = config.getIntList(kMinFirmwareVersion);
		for (int fwVer : fwList)
			_supportedFirmwareVer.push_back(static_cast<uint32_t>(fwVer));
	}

	if (_supportedFirmwareVer.empty())
	{
		if (_variant == ePSOCGPIOIIC)
		{
			_supportedFirmwareVer.push_back(18);
			_supportedFirmwareVer.push_back(19);
		}
		else
			_supportedFirmwareVer.push_back(kDefaultFirmwareVersion);
	}

	const ConfigurationTable& pinDataArray = config.getTable(kPlatformEntries);

	for (const auto& pinEntry : pinDataArray)
	{
		PSOCPinData pinData;

		if (pinEntry.find(kPinNumber) != pinEntry.end())
			pinData._pin = ConfigurationFile::getRecordUInt64(pinEntry, kPinNumber);

		if (pinEntry.find(kEnabled) != pinEntry.end())
			pinData._enabled = ConfigurationFile::getRecordBool(pinEntry, kEnabled);

		if (pinEntry.find(kIntialValue) != pinEntry.end())
			pinData._initialValue = ConfigurationFile::getRecordBool(pinEntry, kIntialValue);

		if (pinEntry.find(kPriority) != pinEntry.end())
			pinData._initializationPriority = ConfigurationFile::getRecordInt(pinEntry, kPriority);

		if (pinEntry.find(kInverted) != pinEntry.end())
			pinData._inverted = ConfigurationFile::getRecordBool(pinEntry, kInverted);

		if (pinEntry.find(kName) != pinEntry.end())
			pinData._pinLabel = ConfigurationFile::getRecordString(pinEntry, kName);

		if (pinEntry.find(kToolTip) != pinEntry.end())
			pinData._pinTooltip = ConfigurationFile::getRecordString(pinEntry, kToolTip);

		if (pinEntry.find(kCommand) != pinEntry.end())
			pinData._pinCommand = ConfigurationFile::getRecordString(pinEntry, kCommand);

		if (pinEntry.find(kCommmandGroup) != pinEntry.end())
			pinData._commandGroup = CommandGroups(ConfigurationFile::getRecordInt(pinEntry, kCommmandGroup));

		if (pinEntry.find(kClassicAction) != pinEntry.end())
			pinData._classicAction = ConfigurationFile::getRecordString(pinEntry, kClassicAction);

		if (pinEntry.find(kTabName) != pinEntry.end())
			pinData._tabName = ConfigurationFile::getRecordString(pinEntry, kTabName);

		if (pinEntry.find(kRunPriority) != pinEntry.end())
		{
			pinData._cellLocation = toQPoint(ConfigurationFile::getRecordString(pinEntry, kRunPriority));
		}

		_pinEntries.insert(pinData._pin, pinData);
	}

	// Read i2c_slaves (slave config descriptors)
	const ConfigurationTable& slaveConfigArray = config.getTable(kI2CSlaves);
	if (!slaveConfigArray.empty())
	{
		_slaveConfigs.clear();
		for (const auto& slaveData : slaveConfigArray)
		{
			PSOCI2CSlave slave;
			slave._variant = PSOCIICVariant(ConfigurationFile::getRecordInt(slaveData, kSlaveVariant));
			slave._slaveAddress = static_cast<PinID>(ConfigurationFile::getRecordInt(slaveData, kRegAddress));
			slave._configAddress = static_cast<PinID>(ConfigurationFile::getRecordInt(slaveData, kConfigAddress));
			slave._portCount = ConfigurationFile::getRecordInt(slaveData, kPortCount);
			_slaveConfigs.push_back(slave);
		}

		rebuildI2CTabs();
	}

	// Read i2c_addr (per-pin I2C entries)
	const ConfigurationTable& i2cDataArray = config.getTable(kI2CAddress);
	if (!i2cDataArray.empty())
	{
		_i2cEntries.clear();
		for (const auto& i2cData : i2cDataArray)
		{
			PSOCI2CData i2cEntry;

			if (i2cData.find(kPinNumber) != i2cData.end())
				i2cEntry._pin = ConfigurationFile::getRecordUInt64(i2cData, kPinNumber);
			if (i2cData.find(kSlaveAddress) != i2cData.end())
				i2cEntry._slaveAddress = static_cast<PinID>(ConfigurationFile::getRecordInt(i2cData, kSlaveAddress));
			if (i2cData.find(kWriteAddress) != i2cData.end())
				i2cEntry._writeAddress = static_cast<PinID>(ConfigurationFile::getRecordInt(i2cData, kWriteAddress));
			if (i2cData.find(kEnabled) != i2cData.end())
				i2cEntry._enabled = ConfigurationFile::getRecordBool(i2cData, kEnabled);
			if (i2cData.find(kName) != i2cData.end())
				i2cEntry._pinLabel = ConfigurationFile::getRecordString(i2cData, kName);
			if (i2cData.find(kToolTip) != i2cData.end())
				i2cEntry._pinTooltip = ConfigurationFile::getRecordString(i2cData, kToolTip);
			if (i2cData.find(kCommand) != i2cData.end())
				i2cEntry._pinCommand = ConfigurationFile::getRecordString(i2cData, kCommand);
			if (i2cData.find(kCommmandGroup) != i2cData.end())
				i2cEntry._commandGroup = CommandGroups(ConfigurationFile::getRecordInt(i2cData, kCommmandGroup));
			if (i2cData.find(kClassicAction) != i2cData.end())
				i2cEntry._classicAction = ConfigurationFile::getRecordString(i2cData, kClassicAction);
			if (i2cData.find(kTabName) != i2cData.end())
				i2cEntry._tabName = ConfigurationFile::getRecordString(i2cData, kTabName);
			if (i2cData.find(kRunPriority) != i2cData.end())
			{
				i2cEntry._cellLocation = toQPoint(ConfigurationFile::getRecordString(i2cData, kRunPriority));
			}
			if (i2cData.find(kInverted) != i2cData.end())
				i2cEntry._inverted = ConfigurationFile::getRecordBool(i2cData, kInverted);

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

void _PSOCPlatformConfiguration::writeConfig(ConfigurationFile &config)
{
	_PlatformConfiguration::writeConfig(config);

	std::vector<int> firmwareVersions;
	for (uint32_t fwVer : std::as_const(_supportedFirmwareVer))
	{
		firmwareVersions.push_back(static_cast<int>(fwVer));
	}
	config.setIntList(kMinFirmwareVersion, firmwareVersions);

	// Writing platform configuration mapping
	ConfigurationTable jsonPlatformPinData;

	for (const auto& pinData : std::as_const(_pinEntries))
	{
		ConfigurationRecord psocPinData;

		psocPinData[kPinNumber] = std::to_string(pinData._pin);
		psocPinData[kEnabled] = pinData._enabled ? "true" : "false";
		psocPinData[kIntialValue] = pinData._initialValue ? "true" : "false";
		psocPinData[kPriority] = std::to_string(pinData._initializationPriority);
		psocPinData[kInverted] = pinData._inverted ? "true" : "false";
		psocPinData[kName] = pinData._pinLabel;
		psocPinData[kToolTip] = pinData._pinTooltip;
		psocPinData[kCommand] = pinData._pinCommand;
		psocPinData[kCommmandGroup] = std::to_string(pinData._commandGroup);
		psocPinData[kClassicAction] = pinData._classicAction;
		// run_prority indicates cell_location (the location of the PinLED within TAC)
		psocPinData[kRunPriority] = fromQPoint(pinData._cellLocation);
		psocPinData[kTabName] = pinData._tabName;

		jsonPlatformPinData.push_back(psocPinData);
	}

	config.setTable(kPlatformEntries, jsonPlatformPinData);

	if (_variant == ePSOCGPIOIIC)
	{
		// Write i2c_slaves (slave config descriptors)
		ConfigurationTable jsonSlaveConfigs;
		for (const auto& sc : std::as_const(_slaveConfigs))
		{
			ConfigurationRecord slaveData;
			slaveData[kSlaveVariant] = std::to_string(static_cast<int>(sc._variant));
			slaveData[kRegAddress] = std::to_string(static_cast<int>(sc._slaveAddress));
			slaveData[kConfigAddress] = std::to_string(static_cast<int>(sc._configAddress));
			slaveData[kPortCount] = std::to_string(sc._portCount);
			jsonSlaveConfigs.push_back(slaveData);
		}
		config.setTable(kI2CSlaves, jsonSlaveConfigs);

		// Write i2c_addr (per-pin I2C entries)
		ConfigurationTable jsonI2CData;
		for (const auto& i2cEntry : std::as_const(_i2cEntries))
		{
			ConfigurationRecord i2cData;
			i2cData[kPinNumber] = std::to_string(i2cEntry._pin);
			i2cData[kSlaveAddress] = std::to_string(static_cast<int>(i2cEntry._slaveAddress));
			i2cData[kWriteAddress] = std::to_string(static_cast<int>(i2cEntry._writeAddress));
			i2cData[kEnabled] = i2cEntry._enabled ? "true" : "false";
			i2cData[kName] = i2cEntry._pinLabel;
			i2cData[kToolTip] = i2cEntry._pinTooltip;
			i2cData[kCommand] = i2cEntry._pinCommand;
			i2cData[kCommmandGroup] = std::to_string(i2cEntry._commandGroup);
			i2cData[kClassicAction] = i2cEntry._classicAction;
			i2cData[kTabName] = i2cEntry._tabName;
			i2cData[kRunPriority] = fromQPoint(i2cEntry._cellLocation);
			i2cData[kInverted] = i2cEntry._inverted ? "true" : "false";
			jsonI2CData.push_back(i2cData);
		}
		config.setTable(kI2CAddress, jsonI2CData);
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
	pinData._cellLocation = std::make_pair(0, 0);
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
	pinData._cellLocation = std::make_pair(1, 0);
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
	pinData._cellLocation = std::make_pair(2, 0);
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
	pinData._cellLocation = std::make_pair(0, 0);
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
	pinData._cellLocation = std::make_pair(0, 1);
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
	pinData._cellLocation = std::make_pair(0, 2);
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
	pinData._cellLocation = std::make_pair(0, 0);
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
	pinData._cellLocation = std::make_pair(1, 0);
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
	pinData._cellLocation = std::make_pair(0, 1);
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
	pinData._cellLocation = std::make_pair(0, 2);
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
	pinData._cellLocation = std::make_pair(0, 3);
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
	pinData._cellLocation = std::make_pair(0, 4);
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
	pinData._cellLocation = std::make_pair(0, 5);
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
	pinData._cellLocation = std::make_pair(0, 0);
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
	pinData._cellLocation = std::make_pair(0, 1);
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
	pinData._cellLocation = std::make_pair(-1, -1);
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
		i2cData._cellLocation = std::make_pair(0, 0);
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
		i2cData._cellLocation = std::make_pair(1, 0);
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
		i2cData._cellLocation = std::make_pair(0, 1);
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
		i2cData._cellLocation = std::make_pair(1, 1);
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
		i2cData._cellLocation = std::make_pair(0, 2);
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
		i2cData._cellLocation = std::make_pair(1, 2);
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
		i2cData._cellLocation = std::make_pair(0, 3);
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
		i2cData._cellLocation = std::make_pair(1, 3);
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
		i2cData._cellLocation = std::make_pair(0, 4);
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
		i2cData._cellLocation = std::make_pair(1, 4);
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
		i2cData._cellLocation = std::make_pair(0, 5);
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
		i2cData._cellLocation = std::make_pair(1, 5);
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
		i2cData._cellLocation = std::make_pair(0, 6);
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
		i2cData._cellLocation = std::make_pair(1, 6);
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
		i2cData._cellLocation = std::make_pair(0, 0);
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
		i2cData._cellLocation = std::make_pair(1, 0);
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
		i2cData._cellLocation = std::make_pair(0, 1);
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
		i2cData._cellLocation = std::make_pair(1, 1);
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

		if (_classicSlaveConfigs.empty())
		{
			PSOCI2CSlave slave;

			slave._variant = eKTS1622EUAATR;
			slave._slaveAddress = 0x20;
			slave._configAddress = 0x02;
			slave._portCount = 2;
			_classicSlaveConfigs.push_back(slave);

			slave._variant = eTCA9534APWR;
			slave._slaveAddress = 0x38;
			slave._configAddress = 0x01;
			slave._portCount = 1;
			_classicSlaveConfigs.push_back(slave);
		}
	}
}
