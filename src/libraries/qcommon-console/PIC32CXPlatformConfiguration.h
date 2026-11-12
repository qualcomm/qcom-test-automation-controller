// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PIC32CXPLATFORMCONFIGURATION_H
#define PIC32CXPLATFORMCONFIGURATION_H

#include "CommandGroup.h"
#include "PlatformConfiguration.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

const PlatformID kMaxPIC32CXPlatformId(99999);

struct PIC32CXPinData
{
	PIC32CXPinData() = default;
	PIC32CXPinData(const PIC32CXPinData& copyMe) = default;
	PIC32CXPinData(PinID setPin)
	{
		_hash = PIC32CXPinData::makePIC32CXHash(setPin);
	}

	void clear()
	{
		*this = PIC32CXPinData();
	}

	static HashType makePIC32CXHash(PinID setPin)
	{
		return strHash(std::to_string(setPin));
	}

	HashType					_hash{0};

	PinID						_setPin{static_cast<PinID>(-1)};
	bool						_enabled{false};
	std::string					_pinLabel;
	std::string					_pinTooltip;
	bool						_inverted{false};
	std::string					_pinCommand;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	std::pair<int, int>			_cellLocation{-1, -1};
	std::string					_tabName{"General"};
};

typedef std::map<PinID, PIC32CXPinData> PIC32CXPinEntries;
typedef std::vector<PIC32CXPinData> PIC32CXPinList;

class _PIC32CXPlatformConfiguration;

class QCOMMONCONSOLE_EXPORT _PIC32CXPlatformConfiguration:
	public _PlatformConfiguration
{
public:
	_PIC32CXPlatformConfiguration();
	virtual ~_PIC32CXPlatformConfiguration();

	virtual Pins getPins();

	PIC32CXPinList getAllPins();
	PIC32CXPinList getActivePins();

	bool getPinEnableState(const PinID pinId) const;
	void setPinEnableState(const HashType hash, bool newState);

	bool getPinInvertedState(const PinID pinId) const;
	void setPinInvertedState(const HashType hash, bool newState);

	std::string getPinLabel(const PinID pinId) const;
	void setPinLabel(const HashType hash, const std::string& pinLabel);

	std::string getPinTooltip(const PinID pinId) const;
	void setPinTooltip(const HashType hash, const std::string& pinTooltip);

	std::string getPinCommand(const PinID pinId) const;
	void setPinCommand(const HashType hash, const std::string& pinCommand);

	CommandGroups getPinGroup(const PinID pinId) const;
	void setPinGroup(const HashType hash, const CommandGroups pinGroup);

	std::string getTabName(const PinID pinId) const;
	void setTabName(const HashType hash, const std::string& tabName);

	std::pair<int, int> getPinCellLocation(const PinID pinId) const;
	void setPinCellLocation(const PinID pinId, const std::pair<int, int>& cellLocation);

	PinID bitFromSetPin(PinID setPin);

protected:
	virtual void cascadeTabDelete(const std::string& deleteMe);
	virtual void cascadeTabRename(const std::string& oldName, const std::string& newName);

	virtual bool readConfig(const ConfigurationFile& config) override;
	virtual void writeConfig(ConfigurationFile& config) override;

private:
	void initialize();

	PIC32CXPinEntries               _pinEntries;
};
#endif // PIC32CXPLATFORMCONFIGURATION_H
