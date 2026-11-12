// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PSOCPLATFORMCONFIGURATION_H
#define PSOCPLATFORMCONFIGURATION_H

#include "PlatformConfiguration.h"
#include "CommandGroup.h"

#include <string>
#include <utility>

const PlatformID kMaxPSOCPlatformId(255);

struct PSOCPinData
{
	PSOCPinData() = default;
	PSOCPinData(const PSOCPinData& copyMe) = default;
	PSOCPinData(PinID pin)
	{
		_pin = pin;
		_hash = _pin;
	}

	void clear()
	{
		*this = PSOCPinData();
	}

	// TODO: PinID made compatible with HashType, under condition that PSOC will
	// have a default pin as 0 instead of -1 previously.
	PinID						_pin{0};
	HashType					_hash{0};
	bool						_enabled{true};
	std::string					_pinLabel;
	std::string					_pinTooltip;
	bool						_initialValue{false};
	int							_initializationPriority{0};
	bool						_inverted{false};
	std::string					_pinCommand;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	std::string					_classicAction;
	std::pair<int, int>			_cellLocation{-1, -1};
	std::string					_tabName;
};

struct PSOCI2CSlave
{
	PSOCI2CSlave() = default;

	void clear() { *this = PSOCI2CSlave(); }

	PSOCIICVariant				_variant{ePSOCIICUnknown};
	PinID						_slaveAddress{0};
	PinID						_configAddress{0};
	int							_portCount{0};
};


struct PSOCI2CData
{
	PSOCI2CData() = default;
	PSOCI2CData(const PSOCI2CData& copyMe) = default;
	PSOCI2CData(PinID slaveAddress, PinID writeAddress, PinID pin)
	{
		_slaveAddress = slaveAddress;
		_writeAddress = writeAddress;
		_pin = pin;
		_hash = makeHash();
	}

	void clear()
	{
		*this = PSOCI2CData();
	}

	HashType makeHash()
	{
		return (static_cast<HashType>(_slaveAddress) << 40) ^
			   (static_cast<HashType>(_writeAddress) << 20) ^
				static_cast<HashType>(_pin);
	}

	PinID						_pin{0};
	PinID						_slaveAddress{0};
	PinID						_writeAddress{0};
	HashType					_hash{0};
	bool						_enabled{false};
	std::string					_pinLabel;
	std::string					_pinTooltip;
	bool						_inverted{false};
	std::string					_pinCommand;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	std::string					_classicAction;
	std::pair<int, int>			_cellLocation{-1, -1};
	std::string					_tabName;
};

typedef QMap<PinID, PSOCPinData> PSOCPinEntries;
typedef QMap<HashType, PSOCI2CData> PSOCI2CEntries;
typedef QList<PSOCPinData> PSOCPinList;
typedef QList<PSOCI2CSlave> PSOCI2CSlaves;

class _PSOCPlatformConfiguration;

class QCOMMONCONSOLE_EXPORT _PSOCPlatformConfiguration:
	public _PlatformConfiguration
{
public:
	_PSOCPlatformConfiguration() = delete;
	_PSOCPlatformConfiguration(PSOCVariant psocVariant);
	virtual ~_PSOCPlatformConfiguration();

	virtual Pins getPins();
	Pins getI2CPinEntries();

	PSOCPinList getAllPins();
	PSOCPinList getActivePins();

	PSOCVariant variant();
	void setVariant(PSOCVariant psocVariant);

	bool getPinEnableState(const PinID pinId) const;
	void setPinEnableState(const PinID pinId, bool newState);

	bool getInitialPinValue(const PinID pinId) const;
	void setInitialPinValue(const PinID pinId, bool newState);

	uint64_t getPinInitializationPriority(const PinID pinId) const;
	void setPinInitializationPriority(const PinID pinId, const int priority);

	bool getPinInvertedState(const PinID pinId) const;
	void setPinInvertedState(const PinID pinId, bool newState);

	std::string getPinLabel(const PinID pinId) const;
	void setPinLabel(const PinID pinId, const std::string& pinLabel);

	std::string getPinTooltip(const PinID pinId) const;
	void setPinTooltip(const PinID pinId, const std::string& pinTooltip);

	std::string getPinCommand(const PinID pinId) const;
	void setPinCommand(const PinID pinId, const std::string& pinCommand);

	CommandGroups getPinGroup(const PinID pinId) const;
	void setPinGroup(const PinID pinId, const CommandGroups commandGroup);

	std::string getTabName(const PinID pinId) const;
	void setTabName(const PinID pinId, const std::string tabName);

	std::string getClassicAction(const PinID pinId) const;
	void setClassicAction(const PinID pinId, const std::string& classicAction);

	std::pair<int, int> getPinCellLocation(const PinID pinId) const;
	void setPinCellLocation(const PinID pinId, const std::pair<int, int>& cellLocation);

	PSOCI2CSlaves getSlaveConfigs() const;
	void setSlaveConfigs(PSOCI2CSlaves& slaves);

	void addSlaveConfig(const PSOCI2CSlave& slave);
	void removeSlaveConfig(PinID slaveAddress);

	static std::string i2cSlaveTabName(const PSOCI2CSlave& slave);

	PSOCI2CEntries getI2CEntries() const;
	PSOCI2CEntries getActiveI2CEntries() const;
	QList<PSOCI2CData> getI2CEntriesForSlave(const PSOCI2CSlave& slave) const;
	bool addI2CSlave(const PSOCI2CData& i2cData);
	void removeI2CSlave(HashType hash);
	PSOCI2CData getI2CSlave(HashType hash) const;

	bool getI2CPinEnableState(const HashType hash) const;
	void setI2CPinEnableState(const HashType hash, bool newState);

	bool getI2CPinInvertedState(const HashType hash) const;
	void setI2CPinInvertedState(const HashType hash, bool newState);

	std::string getI2CPinLabel(const HashType hash) const;
	void setI2CPinLabel(const HashType hash, const std::string& pinLabel);

	std::string getI2CPinTooltip(const HashType hash) const;
	void setI2CPinTooltip(const HashType hash, const std::string& pinTooltip);

	std::string getI2CPinCommand(const HashType hash) const;
	void setI2CPinCommand(const HashType hash, const std::string& pinCommand);

	CommandGroups getI2CPinGroup(const HashType hash) const;
	void setI2CPinGroup(const HashType hash, const CommandGroups commandGroup);

	std::string getI2CTabName(const HashType hash) const;
	void setI2CTabName(const HashType hash, const std::string& tabName);

	std::pair<int, int> getI2CPinCellLocation(const HashType hash) const;
	void setI2CPinCellLocation(const HashType hash, const std::pair<int, int>& cellLocation);

protected:
	virtual void cascadeTabDelete(const std::string& deleteMe);
	virtual void cascadeTabRename(const std::string& oldName, const std::string& newName);

	virtual bool read(QJsonObject& parentLevel);
	virtual void write(QJsonObject& parentLevel);

private:
	PSOCPinEntries				_pinEntries;
	PSOCI2CEntries				_i2cEntries;
	PSOCI2CSlaves				_slaveConfigs;
	PSOCVariant					_variant{ePSOCGPIO};

	static void initialize(PSOCVariant psocVariant);
	void refreshSlaveConfig();
	void rebuildI2CTabs();

	static PSOCPinEntries		_classicActions;
	static PSOCI2CEntries		_classicI2CActions;
	static PSOCI2CSlaves		_classicSlaveConfigs;
};

#endif // PSOCPLATFORMCONFIGURATION_H
