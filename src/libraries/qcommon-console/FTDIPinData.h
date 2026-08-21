#ifndef FTDIPINDATA_H
#define FTDIPINDATA_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "CommandGroup.h"
#include "PlatformConfiguration.h"
#include "StringUtilities.h"

const int kMaxPinIndex{7};
const int kPinsPerBus{8};

typedef QChar Bus;

enum FTDIBusFunction
{
	eBusFunctionUnknown = 0,
	eBusFunctionVCP,
	eBusFunctionD2XX,
	eBusFunctionI2C
};

struct FTDIBusData
{
	FTDIBusData() = default;
	FTDIBusData(const FTDIBusData& copyMe) = default;
	FTDIBusData(ChipIndex chipIndex, Bus bus, FTDIBusFunction busFunction)
	{
		_hash = FTDIBusData::makeFTDIHash(chipIndex, bus);
		_chipIndex = chipIndex;
		_bus = bus;
		_busFunction = busFunction;
	}

	void clear()
	{
		*this = FTDIBusData();
	}

	static HashType makeFTDIHash(ChipIndex chipIndex, Bus bus)
	{
		HashType result{0};

		QString chipStr = QString::number(chipIndex);
		QString busStr = QString(bus);

		result = strHash(chipStr + busStr);

		return result;
	}

	static QString toString(FTDIBusFunction busFunction)
	{
		switch(busFunction)
		{
		case eBusFunctionVCP:
			return "VCP";
		case eBusFunctionD2XX:
			return "D2XX";
		case eBusFunctionI2C:
			return "I2C";
		default:
			return QString();
		}
	}

	static FTDIBusFunction fromString(const QString& busFunction)
	{
		QString temp{busFunction.toUpper()};
		if (temp == "VCP")
			return eBusFunctionVCP;
		if (temp == "D2XX")
			return eBusFunctionD2XX;
		if (temp == "I2C")
			return eBusFunctionI2C;

		return eBusFunctionUnknown;
	}

	HashType					_hash{0};

	ChipIndex					_chipIndex{1};
	Bus							_bus;
	FTDIBusFunction				_busFunction{eBusFunctionUnknown};
};

typedef QMap<HashType, FTDIBusData> FTDIBusFunctions;

struct FTDIPinData
{
	FTDIPinData() = default;
	FTDIPinData(const FTDIPinData& copyMe) = default;
	FTDIPinData(ChipIndex chipIndex, Bus bus, PinID pin)
	{
		_hash = FTDIPinData::makeFTDIHash(chipIndex, bus, pin);
		_chipIndex = chipIndex;
		_bus = bus;
		_chipPin = pin;
	}

	void clear()
	{
		*this = FTDIPinData();
	}

	static HashType makeFTDIHash(ChipIndex chipIndex, Bus bus, PinID pin)
	{
		HashType result{0};

		QString chipStr = QString::number(chipIndex);
		QString busStr = QString(bus);
		QString pinStr = QString::number(pin);

		result = strHash(chipStr + busStr + pinStr);

		return result;
	}

	HashType					_hash{0};

	ChipIndex					_chipIndex{1};
	Bus							_bus;
	PinID						_chipPin{static_cast<PinID>(-1)};
	PinID						_setPin{static_cast<PinID>(-1)};
	bool						_enabled{false};
	bool						_input{false};
	QString						_pinLabel;
	QString						_pinTooltip;
	bool						_initialValue{false};
	int							_initializationPriority{-1};
	bool						_inverted{false};
	QString						_pinCommand;
	CommandGroups				_commandGroup{eUnknownCommandGroup};
	QPoint						_cellLocation{QPoint(-1,-1)};
	QString						_tabName{"General"};
};

typedef QMap<HashType, FTDIPinData> FTDIPinEntries;
typedef QList<FTDIPinData> FTDIPinList;


#endif // FTDIPINDATA_H
