// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Authors: Biswajit Roy, Michael Simpson

#ifndef QTAC_FTDIPLATFORMCONFIGURATION_H
#define QTAC_FTDIPLATFORMCONFIGURATION_H

#include <qtac/String.h>
#include <qtac/ByteArray.h>
#include <qtac/Map.h>
#include <qtac/List.h>
#include <qtac/StringList.h>
#include <qtac/Point.h>
#include <qtac/PinEntry.h>
#include <qtac/FTDIPinSet.h>
#include <qtac/StringUtilities.h>
#include <qtac/CommandGroup.h>
#include <qtac/AlpacaScript.h>

#include <qtac/json_util.h>
#include <memory>
#include <string>

using ChipIndex = int16_t;

const int kMaxPinIndex{7};
const int kPinsPerBus{8};

// Bus is a single character: 'A', 'B', 'C', or 'D'
using Bus = char;

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
	FTDIBusData(const FTDIBusData&) = default;
	FTDIBusData(ChipIndex chipIndex, Bus bus, FTDIBusFunction busFunction)
		: _chipIndex(chipIndex), _bus(bus), _busFunction(busFunction)
	{
		_hash = makeFTDIHash(chipIndex, bus);
	}

	void clear() { *this = FTDIBusData(); }

	static HashType makeFTDIHash(ChipIndex chipIndex, Bus bus)
	{
		return strHash(qtac::String::number(chipIndex) + qtac::String(bus));
	}

	static qtac::String toString(FTDIBusFunction bf)
	{
		switch (bf)
		{
		case eBusFunctionVCP:  return "VCP";
		case eBusFunctionD2XX: return "D2XX";
		case eBusFunctionI2C:  return "I2C";
		default:               return qtac::String();
		}
	}

	static FTDIBusFunction fromString(const qtac::String& s)
	{
		qtac::String u = s.toUpper();
		if (u == "VCP")  return eBusFunctionVCP;
		if (u == "D2XX") return eBusFunctionD2XX;
		if (u == "I2C")  return eBusFunctionI2C;
		return eBusFunctionUnknown;
	}

	HashType       _hash{0};
	ChipIndex      _chipIndex{1};
	Bus            _bus{0};
	FTDIBusFunction _busFunction{eBusFunctionUnknown};
};

struct FTDIPinData
{
	FTDIPinData() = default;
	FTDIPinData(const FTDIPinData&) = default;
	FTDIPinData(ChipIndex chipIndex, Bus bus, PinID pin)
		: _chipIndex(chipIndex), _bus(bus), _chipPin(pin)
	{
		_hash = makeFTDIHash(chipIndex, bus, pin);
	}

	void clear() { *this = FTDIPinData(); }

	static HashType makeFTDIHash(ChipIndex chipIndex, Bus bus, PinID pin)
	{
		return strHash(qtac::String::number(chipIndex)
		             + qtac::String(bus)
		             + qtac::String::number(static_cast<uint64_t>(pin)));
	}

	HashType        _hash{0};
	ChipIndex       _chipIndex{1};
	Bus             _bus{0};
	PinID           _chipPin{static_cast<PinID>(-1)};
	PinID           _setPin{static_cast<PinID>(-1)};
	bool            _enabled{false};
	bool            _input{false};
	qtac::String    _pinLabel;
	qtac::String    _pinTooltip;
	bool            _initialValue{false};
	int             _initializationPriority{-1};
	bool            _inverted{false};
	qtac::String    _pinCommand;
	CommandGroups   _commandGroup{eUnknownCommandGroup};
	qtac::Point     _cellLocation{-1, -1};
	qtac::String    _tabName{"General"};
};

using FTDIPinEntries = qtac::Map<HashType, FTDIPinData>;
using FTDIPinList    = qtac::List<FTDIPinData>;
using FTDIBusFunctions = qtac::Map<HashType, FTDIBusData>;


class _FTDIPlatformConfiguration
{
public:
	_FTDIPlatformConfiguration() = delete;
	explicit _FTDIPlatformConfiguration(uint16_t chipCount);
	_FTDIPlatformConfiguration(const _FTDIPlatformConfiguration&) = delete;
	_FTDIPlatformConfiguration& operator=(const _FTDIPlatformConfiguration&) = delete;
	~_FTDIPlatformConfiguration() = default;

	void initialize(uint16_t chipCount);

	FTDIPinData getPinData(ChipIndex chipIndex, Bus bus, PinID pin);

	FTDIPinList getAllPins() const;
	FTDIPinList getActivePins() const;
	FTDIPinList getActivePins(ChipIndex chipIndex, Bus bus) const;

	int getChipCount();

	Pins getPins();

	bool getPinEnableState(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setPinEnableState(HashType hash, bool newState);

	bool getPinInputState(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setPinInputState(HashType hash, bool newState);

	bool getInitialPinValue(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setInitialPinValue(HashType hash, bool newState);

	int getPinInitializationPriority(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setPinInitializationPriority(HashType hash, int priority);

	bool getPinInvertedState(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setPinInvertedState(HashType hash, bool newState);

	qtac::String getPinLabel(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setPinLabel(HashType hash, const qtac::String& pinLabel);

	qtac::String getPinTooltip(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setPinTooltip(HashType hash, const qtac::String& pinTooltip);

	qtac::String getPinCommand(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setPinCommand(HashType hash, const qtac::String& pinCommand);

	CommandGroups getPinGroup(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setPinGroup(HashType hash, CommandGroups pinGroup);

	qtac::String getTabName(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setTabName(HashType hash, const qtac::String& tabName);

	qtac::Point getPinCellLocation(ChipIndex chipIndex, Bus bus, PinID pinId) const;
	void setPinCellLocation(HashType hash, const qtac::Point& cellLocation);

	FTDIBusData getBusFunction(ChipIndex chipIndex, Bus bus) const;
	void setBusFunction(HashType hash, FTDIBusFunction busFunction);
	void setBusFunction(ChipIndex chipIndex, Bus bus, FTDIBusFunction busFunction);

	FTDIPinSets getPinSet(ChipIndex chipIndex);

	void cascadeTabDelete(const qtac::String& tabName);
	void cascadeTabRename(const qtac::String& oldName, const qtac::String& newName);

	// --- Quick Settings buttons and script variables ---
	const qtac::ButtonEntries&   getButtons()   const { return _buttons;   }
	const qtac::VariableEntries& getVariables() const { return _variables; }
	const qtac::AlpacaScript&    getScript()    const { return _script;    }

	// --- Tab list from tcnf "tabs" array (visible tabs in ordinal order) ---
	const qtac::StringList& getTabs() const { return _tabs; }

	// --- Config file metadata ---
	qtac::String modificationDate() const { return _modificationDate; }
	int          fileVersion()      const { return _fileVersion;      }

	// Variable value setters (called from UI widgets)
	void setVariableValue(const qtac::String& name, const qtac::Variant& value)
	{
		auto it = _variables.find(name);
		if (it != _variables.end()) it->second._defaultValue = value;
	}

	bool read(json_t& parentLevel);
	void write(json_t& parentLevel);
	void loadDefaultScript(const qtac::String& scriptText);

private:
	PinID getSetPinIndex(int chipIndex, Bus bus, PinID pinId);

	int             _chipCount{1};
	FTDIPinEntries  _pinEntries;
	FTDIBusFunctions _busFunctions;
	qtac::ButtonEntries   _buttons;
	qtac::VariableEntries _variables;
	qtac::AlpacaScript    _script;
	qtac::StringList      _tabs;
	qtac::String          _modificationDate;
	int                   _fileVersion{0};
};

using FTDIPlatformConfiguration = std::shared_ptr<_FTDIPlatformConfiguration>;

#endif // QTAC_FTDIPLATFORMCONFIGURATION_H
