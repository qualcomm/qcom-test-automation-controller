#ifndef STM32PINDATA_H
#define STM32PINDATA_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommonConsole
#include "CommandGroup.h"
#include "PinID.h"
#include "StringUtilities.h"

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
		HashType result{0};

		QString gpioStr = QString::number(gpioIndex);

		result = strHash(gpioStr);

		return result;
	}

	HashType					_hash{0};

	PinID						_pin{static_cast<PinID>(-1)};
	bool						_enabled{false};
	bool						_inverted{false};
	QString						_pinLabel;
	QString						_pinTooltip;
	QString						_pinCommand;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	QPoint						_cellLocation{QPoint(-1,-1)};
	QString						_tabName{"General"};
};

typedef QMap<HashType, STM32PinData> STM32PinEntries;
typedef QList<STM32PinData> STM32PinList;


#endif // STM32PINDATA_H
