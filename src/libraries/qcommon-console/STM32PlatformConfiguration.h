// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef STM32PLATFORMCONFIGURATION_H
#define STM32PLATFORMCONFIGURATION_H

#include "QCommonConsoleGlobal.h"

#include "PlatformConfiguration.h"
#include "STM32PinData.h"


class QCOMMONCONSOLE_EXPORT _STM32PlatformConfiguration: public _PlatformConfiguration
{
public:
	_STM32PlatformConfiguration();
	~_STM32PlatformConfiguration();

	void initialize();

	STM32PinData getPinData(PinID pin);

	STM32PinList getAllPins() const;
	STM32PinList getActivePins() const;

	virtual Pins getPins();

	bool getPinEnableState(const PinID pin) const;
	void setPinEnableState(HashType hash, bool newState);

	bool getPinInvertedState(const PinID pin) const;
	void setPinInvertedState(HashType hash, bool newState);

	std::string getPinLabel(const PinID pin) const;
	void setPinLabel(HashType hash, const std::string& pinLabel);

	std::string getPinTooltip(const PinID pin) const;
	void setPinTooltip(HashType hash, const std::string& pinTooltip);

	std::string getPinCommand(const PinID pin) const;
	void setPinCommand(HashType hash, const std::string& pinCommand);

	CommandGroups getPinGroup(const PinID pin) const;
	void setPinGroup(HashType hash, const CommandGroups pinGroup);

	std::string getTabName(const PinID pin) const;
	void setTabName(HashType hash, const std::string& tabName);

	std::pair<int, int> getPinCellLocation(const PinID pin) const;
	void setPinCellLocation(HashType hash, const std::pair<int, int>& cellLocation);

protected:
	virtual void cascadeTabDelete(const std::string& deleteMe);
	virtual void cascadeTabRename(const std::string& oldName, const std::string& newName);

	virtual bool read(QJsonObject& parentLevel);
	virtual void write(QJsonObject& parentLevel);

private:
	STM32PinEntries					_pinEntries;
};

#endif // STM32PLATFORMCONFIGURATION_H
