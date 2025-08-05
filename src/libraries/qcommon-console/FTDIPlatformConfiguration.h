#ifndef FTDIPLATFORMCONFIGURATION_H
#define FTDIPLATFORMCONFIGURATION_H
/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

#include "PlatformConfiguration.h"

// QCommon
#include "CommandGroup.h"
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

class QCOMMONCONSOLE_EXPORT _FTDIPlatformConfiguration:
	public _PlatformConfiguration
{
public:
	_FTDIPlatformConfiguration() = delete;
	_FTDIPlatformConfiguration(quint16 chipCount);

	void initialize(quint16 chipCount);

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

	QString getPinLabel(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinLabel(HashType hash, const QString& pinLabel);
	void setPinLabel(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QString& pinLabel);

	QString getPinTooltip(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinTooltip(HashType hash, const QString& pinTooltip);
	void setPinTooltip(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QString& pinTooltip);

	QString getPinCommand(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinCommand(HashType hash, const QString& pinCommand);
	void setPinCommand(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QString& pinCommand);

	CommandGroups getPinGroup(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinGroup(HashType hash, const CommandGroups pinGroup);
	void setPinGroup(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const CommandGroups pinGroup);

	QString getTabName(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setTabName(HashType hash, const QString& tabName);
	void setTabName(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QString& tabName);

	QPoint getPinCellLocation(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const;
	void setPinCellLocation(HashType hash, const QPoint& cellLocation);
	void setPinCellLocation(const ChipIndex chipIndex, const Bus& busName, const PinID pinId, const QPoint& cellLocation);

	FTDIBusData getBusFunction(const ChipIndex chipIndex, const Bus& busName) const;
	void setBusFunction(HashType hash, const FTDIBusFunction busFunction);
	void setBusFunction(const ChipIndex chipIndex, const Bus& busName, const FTDIBusFunction busFunction);

	FTDIPinSets getPinSet(const ChipIndex chipIndex);

protected:
	virtual void cascadeTabDelete(const QString& deleteMe);
	virtual void cascadeTabRename(const QString& oldName, const QString& newName);

	virtual bool read(QJsonObject& parentLevel);
	virtual void write(QJsonObject& parentLevel);

private:

	PinID getSetPinIndex(const int chipIndex, const Bus &bus, PinID pinId);

	int							_chipCount{1};
	FTDIPinEntries				_pinEntries;
	FTDIBusFunctions            _busFunctions;
	QString						_usbDescriptorString;
};

#endif // FTDIPLATFORMCONFIGURATION_H
