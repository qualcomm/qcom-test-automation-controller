#ifndef FTDIPLATFORMCONFIGURATION_H
#define FTDIPLATFORMCONFIGURATION_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "QCommonConsoleGlobal.h"

#include "CommandGroup.h"
#include "PlatformConfiguration.h"
#include "FTDIPinData.h"
#include "StringUtilities.h"

#include <cstdint>

class QCOMMONCONSOLE_EXPORT _FTDIPlatformConfiguration:
	public _PlatformConfiguration
{
public:
	_FTDIPlatformConfiguration() = delete;
	_FTDIPlatformConfiguration(uint16_t chipCount);

	void initialize(uint16_t chipCount);

	virtual ~_FTDIPlatformConfiguration();

	FTDIPinData getPinData(ChipIndex chipIndex, Bus bus, PinID pin);

	FTDIPinList getAllPins() const;
	FTDIPinList getActivePins() const;
	FTDIPinList getActivePins(ChipIndex chipIndex, Bus bus) const;

	int getChipCount();

	virtual Pins getPins();

	bool getPinEnableState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinEnableState(HashType hash, bool newState);
	void setPinEnableState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, bool newState);

	bool getPinInputState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinInputState(HashType hash, bool newState);
	void setPinInputState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, bool newState);

	bool getInitialPinValue(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setInitialPinValue(HashType hash, bool newState);
	void setInitialPinValue(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, bool newState);

	int getPinInitializationPriority(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinInitializationPriority(HashType hash, int priority);
	void setPinInitializationPriority(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, int priority);

	bool getPinInvertedState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinInvertedState(HashType hash, bool newState);
	void setPinInvertedState(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, bool newState);

	std::string getPinLabel(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinLabel(HashType hash, const std::string& pinLabel);
	void setPinLabel(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const std::string& pinLabel);

	std::string getPinTooltip(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinTooltip(HashType hash, const std::string& pinTooltip);
	void setPinTooltip(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const std::string& pinTooltip);

	std::string getPinCommand(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinCommand(HashType hash, const std::string& pinCommand);
	void setPinCommand(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const std::string& pinCommand);

	CommandGroups getPinGroup(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinGroup(HashType hash, const CommandGroups pinGroup);
	void setPinGroup(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const CommandGroups pinGroup);

	std::string getTabName(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setTabName(HashType hash, const std::string& tabName);
	void setTabName(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const std::string& tabName);

	std::pair<int, int> getPinCellLocation(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinCellLocation(HashType hash, const std::pair<int, int>& cellLocation);
	void setPinCellLocation(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const std::pair<int, int>& cellLocation);

	FTDIBusData getBusFunction(const ChipIndex chipIndex, const Bus& busName) const;
	void setBusFunction(HashType hash, const FTDIBusFunction busFunction);
	void setBusFunction(const ChipIndex chipIndex, const Bus& busName, const FTDIBusFunction busFunction);

	FTDIPinSets getPinSet(const ChipIndex chipIndex);

protected:
	virtual void cascadeTabDelete(const std::string& deleteMe);
	virtual void cascadeTabRename(const std::string& oldName, const std::string& newName);

	virtual bool readConfig(const ConfigurationFile& config) override;
	virtual void writeConfig(ConfigurationFile& config) override;

private:

	PinID getSetPinIndex(const int chipIndex, const Bus &bus, PinID pinId);

	int							_chipCount{1};
	FTDIPinEntries				_pinEntries;
	FTDIBusFunctions            _busFunctions;
	std::string					_usbDescriptorString;
};

#endif // FTDIPLATFORMCONFIGURATION_H
