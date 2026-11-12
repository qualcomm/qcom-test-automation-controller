#ifndef STM32PINDATA_H
#define STM32PINDATA_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommonConsole
#include "CommandGroup.h"
#include "PinID.h"
#include "StringUtilities.h"

#include <map>
#include <string>
#include <utility>
#include <vector>

const int kMaxSTM32GpioIndex{3};

struct STM32PinData
{
	STM32PinData() = default;
	STM32PinData(const STM32PinData& copyMe) = default;
	STM32PinData(PinID pin)
	{
		_hash = STM32PinData::makeSTM32Hash(pin);
		_pin = pin;
	}

	void clear()
	{
		*this = STM32PinData();
	}

	static HashType makeSTM32Hash(PinID gpioIndex)
	{
		return strHash(std::to_string(gpioIndex));
	}

	HashType					_hash{0};

	PinID						_pin{static_cast<PinID>(-1)};
	bool						_enabled{false};
	bool						_inverted{false};
	std::string					_pinLabel;
	std::string					_pinTooltip;
	std::string					_pinCommand;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	std::pair<int, int>			_cellLocation{-1, -1};
	std::string					_tabName{"General"};
};

typedef std::map<HashType, STM32PinData> STM32PinEntries;
typedef std::vector<STM32PinData> STM32PinList;


#endif // STM32PINDATA_H
