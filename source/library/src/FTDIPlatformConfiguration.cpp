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

#include <qtac/FTDIPlatformConfiguration.h>
#include <qtac/StringUtilities.h>

#include <algorithm>
#include <stdexcept>

// JSON key constants
static const char* kChipCount   = "chip_count";
static const char* kChipIndex   = "chip_index";
static const char* kBus         = "bus";
static const char* kPinNumber   = "pin_number";
static const char* kEnabled     = "enabled";
static const char* kInput       = "input";
static const char* kName        = "name";
static const char* kToolTip     = "help_hint";
static const char* kInitialValue = "initial_value";
static const char* kPriority    = "priority";
static const char* kInverted    = "inverted";
static const char* kCommand     = "command";
static const char* kCommandGroup = "command_group";
static const char* kRunPriority = "run_priority";
static const char* kTabName     = "group";
static const char* kBusFunction = "bus_function";
static const char* kPinEntries  = "pins";
static const char* kBusEntries  = "bus";

static const int kMaximumChipCount{4};

// -----------------------------------------------------------------------

_FTDIPlatformConfiguration::_FTDIPlatformConfiguration(uint16_t chipCount)
{
	initialize(chipCount);
}

void _FTDIPlatformConfiguration::initialize(uint16_t chipCount)
{
	if (chipCount > static_cast<uint16_t>(kMaximumChipCount))
		throw std::invalid_argument("Invalid chip count");

	_chipCount = static_cast<int>(chipCount);

	static const Bus kBuses[]  = {'A', 'B', 'C', 'D'};
	static const PinID kPins[] = {0, 1, 2, 3, 4, 5, 6, 7};

	for (int chipIdx = 0; chipIdx < _chipCount; ++chipIdx)
	{
		int busCount = (chipIdx == 0) ? 2 : 4; // chip 0: A,B; others: A,B,C,D
		for (int b = 0; b < busCount; ++b)
		{
			Bus bus = kBuses[b];
			for (PinID pin : kPins)
			{
				FTDIPinData pinData(static_cast<ChipIndex>(chipIdx), bus, pin);
				FTDIBusData busData(static_cast<ChipIndex>(chipIdx), bus,
				                   (bus == 'A' || bus == 'B') ? eBusFunctionVCP : eBusFunctionD2XX);

				pinData._initializationPriority = -1;
				pinData._commandGroup = eUnknownCommandGroup;
				pinData._cellLocation = qtac::Point(-1, -1);
				pinData._setPin = getSetPinIndex(chipIdx, bus, pin);

				_pinEntries.insert(pinData._hash, pinData);
				_busFunctions.insert(busData._hash, busData);
			}
		}
	}
}

// -----------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------

FTDIPinData _FTDIPlatformConfiguration::getPinData(ChipIndex chipIndex, Bus bus, PinID pin)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, bus, pin);
	return _pinEntries.value(hash, FTDIPinData());
}

FTDIPinList _FTDIPlatformConfiguration::getAllPins() const
{
	FTDIPinList result;
	for (const auto& kv : _pinEntries)
		result.append(kv.second);
	return result;
}

FTDIPinList _FTDIPlatformConfiguration::getActivePins() const
{
	FTDIPinList result;
	for (const auto& kv : _pinEntries)
		if (kv.second._enabled)
			result.append(kv.second);

	std::sort(result.begin(), result.end(), [](const FTDIPinData& a, const FTDIPinData& b) {
		if (a._chipIndex != b._chipIndex) return a._chipIndex < b._chipIndex;
		if (a._bus != b._bus)             return a._bus < b._bus;
		return a._chipPin < b._chipPin;
	});
	return result;
}

FTDIPinList _FTDIPlatformConfiguration::getActivePins(ChipIndex chipIndex, Bus bus) const
{
	FTDIPinList result;
	FTDIBusData bd = getBusFunction(chipIndex, bus);
	if (bd._busFunction != eBusFunctionD2XX)
		return result;

	for (const auto& kv : _pinEntries)
	{
		if (kv.second._enabled && kv.second._chipIndex == chipIndex && kv.second._bus == bus)
			result.append(kv.second);
	}
	return result;
}

int _FTDIPlatformConfiguration::getChipCount()
{
	return _chipCount;
}

Pins _FTDIPlatformConfiguration::getPins()
{
	Pins result;
	FTDIPinList list = getActivePins();
	for (const auto& fp : list)
	{
		PinEntry pe;
		pe._pin                    = fp._setPin;
		pe._enabled                = fp._enabled;
		pe._hash                   = fp._hash;
		pe._cellLocation           = fp._cellLocation;
		pe._commandGroup           = fp._commandGroup;
		pe._pinCommand             = fp._pinCommand;
		pe._initialValue           = fp._initialValue;
		pe._inverted               = fp._inverted;
		pe._pinLabel               = fp._pinLabel;
		pe._pinTooltip             = fp._pinTooltip;
		pe._tabName                = fp._tabName;
		pe._initializationPriority = fp._initializationPriority;
		result.append(pe);
	}

	std::sort(result.begin(), result.end(), [](const PinEntry& a, const PinEntry& b) {
		if (a._tabName != b._tabName)       return a._tabName < b._tabName;
		if (a._commandGroup != b._commandGroup) return a._commandGroup < b._commandGroup;
		return a._pin < b._pin;
	});
	return result;
}

// -----------------------------------------------------------------------
// Enable / Input / InitialValue / Priority / Inverted setters
// -----------------------------------------------------------------------

bool _FTDIPlatformConfiguration::getPinEnableState(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._enabled;
}
void _FTDIPlatformConfiguration::setPinEnableState(HashType hash, bool v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._enabled = v;
}

bool _FTDIPlatformConfiguration::getPinInputState(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._input;
}
void _FTDIPlatformConfiguration::setPinInputState(HashType hash, bool v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._input = v;
}

bool _FTDIPlatformConfiguration::getInitialPinValue(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._initialValue;
}
void _FTDIPlatformConfiguration::setInitialPinValue(HashType hash, bool v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._initialValue = v;
}

int _FTDIPlatformConfiguration::getPinInitializationPriority(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._initializationPriority;
}
void _FTDIPlatformConfiguration::setPinInitializationPriority(HashType hash, int p)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._initializationPriority = p;
}

bool _FTDIPlatformConfiguration::getPinInvertedState(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._inverted;
}
void _FTDIPlatformConfiguration::setPinInvertedState(HashType hash, bool v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._inverted = v;
}

qtac::String _FTDIPlatformConfiguration::getPinLabel(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._pinLabel;
}
void _FTDIPlatformConfiguration::setPinLabel(HashType hash, const qtac::String& v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._pinLabel = v;
}

qtac::String _FTDIPlatformConfiguration::getPinTooltip(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._pinTooltip;
}
void _FTDIPlatformConfiguration::setPinTooltip(HashType hash, const qtac::String& v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._pinTooltip = v;
}

qtac::String _FTDIPlatformConfiguration::getPinCommand(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._pinCommand;
}
void _FTDIPlatformConfiguration::setPinCommand(HashType hash, const qtac::String& v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._pinCommand = v;
}

CommandGroups _FTDIPlatformConfiguration::getPinGroup(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._commandGroup;
}
void _FTDIPlatformConfiguration::setPinGroup(HashType hash, CommandGroups v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._commandGroup = v;
}

qtac::String _FTDIPlatformConfiguration::getTabName(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._tabName;
}
void _FTDIPlatformConfiguration::setTabName(HashType hash, const qtac::String& v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._tabName = v;
}

qtac::Point _FTDIPlatformConfiguration::getPinCellLocation(ChipIndex ci, Bus bus, PinID pin) const
{
	return _pinEntries.value(FTDIPinData::makeFTDIHash(ci, bus, pin))._cellLocation;
}
void _FTDIPlatformConfiguration::setPinCellLocation(HashType hash, const qtac::Point& v)
{
	if (_pinEntries.contains(hash)) _pinEntries[hash]._cellLocation = v;
}

FTDIBusData _FTDIPlatformConfiguration::getBusFunction(ChipIndex ci, Bus bus) const
{
	return _busFunctions.value(FTDIBusData::makeFTDIHash(ci, bus), FTDIBusData());
}
void _FTDIPlatformConfiguration::setBusFunction(HashType hash, FTDIBusFunction bf)
{
	if (_busFunctions.contains(hash)) _busFunctions[hash]._busFunction = bf;
}
void _FTDIPlatformConfiguration::setBusFunction(ChipIndex ci, Bus bus, FTDIBusFunction bf)
{
	setBusFunction(FTDIBusData::makeFTDIHash(ci, bus), bf);
}

FTDIPinSets _FTDIPlatformConfiguration::getPinSet(ChipIndex chipIndex)
{
	FTDIPinSets result{NoOptions};

	auto check = [&](Bus bus, FTDIPinSet flag) {
		FTDIBusData bd = getBusFunction(chipIndex, bus);
		if (bd._bus == bus && bd._busFunction == eBusFunctionD2XX)
			result = result | flag;
	};

	check('A', eA);
	check('B', eB);
	check('C', eC);
	check('D', eD);

	return result;
}

// -----------------------------------------------------------------------
// Tab cascade operations
// -----------------------------------------------------------------------

void _FTDIPlatformConfiguration::cascadeTabDelete(const qtac::String& tabName)
{
    for (auto& pinEntry : _pinEntries)
    {
        if (pinEntry._tabName == tabName)
            pinEntry._tabName = qtac::String();
    }
}

void _FTDIPlatformConfiguration::cascadeTabRename(const qtac::String& oldName,
                                                   const qtac::String& newName)
{
    for (auto& pinEntry : _pinEntries)
    {
        if (pinEntry._tabName == oldName)
            pinEntry._tabName = newName;
    }
}

// -----------------------------------------------------------------------
// JSON I/O
// -----------------------------------------------------------------------

bool _FTDIPlatformConfiguration::read(json_t& j)
{
	if (j.contains(kChipCount))
		_chipCount = j[kChipCount].get<int>();

	if (j.contains(kPinEntries) && j[kPinEntries].is_array())
	{
		_pinEntries.clear();
		for (const auto& pe : j[kPinEntries])
		{
			ChipIndex chipIndex{0};
			Bus bus{0};
			PinID pinId{0};

			if (pe.contains(kChipIndex)) chipIndex = pe[kChipIndex].get<int>();
			if (pe.contains(kBus))
			{
				std::string busStr = pe[kBus].get<std::string>();
				if (!busStr.empty()) bus = busStr[0];
			}
			if (pe.contains(kPinNumber))
				pinId = static_cast<PinID>(std::stoi(pe[kPinNumber].get<std::string>()));

			FTDIPinData pinData(chipIndex, bus, pinId);
			pinData._setPin = getSetPinIndex(chipIndex, bus, pinId);

			if (pe.contains(kEnabled))       pinData._enabled = pe[kEnabled].get<bool>();
			if (pe.contains(kInput))         pinData._input   = pe[kInput].get<bool>();
			if (pe.contains(kName))          pinData._pinLabel   = pe[kName].get<std::string>();
			if (pe.contains(kToolTip))       pinData._pinTooltip = pe[kToolTip].get<std::string>();
			if (pe.contains(kInitialValue))  pinData._initialValue = pe[kInitialValue].get<bool>();
			if (pe.contains(kPriority))      pinData._initializationPriority = pe[kPriority].get<int>();
			if (pe.contains(kInverted))      pinData._inverted = pe[kInverted].get<bool>();
			if (pe.contains(kCommand))       pinData._pinCommand = pe[kCommand].get<std::string>();
			if (pe.contains(kCommandGroup))  pinData._commandGroup = static_cast<CommandGroups>(pe[kCommandGroup].get<int>());
			if (pe.contains(kTabName))       pinData._tabName = pe[kTabName].get<std::string>();

			if (pe.contains(kRunPriority))
			{
				std::string pointStr = pe[kRunPriority].get<std::string>();
				qtac::Point pt = toPoint(qtac::String(pointStr));
				if (pt.x() >= 0 && pt.y() >= 0)
					pinData._cellLocation = pt;
				else
					pinData._cellLocation = qtac::Point(-1, -1);
			}

			_pinEntries.insert(pinData._hash, pinData);
		}
	}

	if (j.contains(kBusEntries) && j[kBusEntries].is_array())
	{
		_busFunctions.clear();
		for (const auto& be : j[kBusEntries])
		{
			ChipIndex chipIndex{0};
			Bus bus{0};
			FTDIBusFunction bf{eBusFunctionUnknown};

			if (be.contains(kChipIndex)) chipIndex = be[kChipIndex].get<int>();
			if (be.contains(kBus))
			{
				std::string busStr = be[kBus].get<std::string>();
				if (!busStr.empty()) bus = busStr[0];
			}
			if (be.contains(kBusFunction))
				bf = static_cast<FTDIBusFunction>(be[kBusFunction].get<int>());

			FTDIBusData busData(chipIndex, bus, bf);
			_busFunctions.insert(busData._hash, busData);
		}
	}

	return true;
}

void _FTDIPlatformConfiguration::write(json_t& j)
{
	j[kChipCount] = _chipCount;

	json_t pinsArray = json_t::array();
	for (const auto& kv : _pinEntries)
	{
		const FTDIPinData& pe = kv.second;
		json_t pinData;
		pinData[kChipIndex]    = pe._chipIndex;
		pinData[kBus]          = std::string(1, pe._bus);
		pinData[kPinNumber]    = qtac::String::number(static_cast<uint64_t>(pe._chipPin)).toStdString();
		pinData[kEnabled]      = pe._enabled;
		pinData[kInput]        = pe._input;
		pinData[kName]         = pe._pinLabel.toStdString();
		pinData[kToolTip]      = pe._pinTooltip.toStdString();
		pinData[kInitialValue] = pe._initialValue;
		pinData[kPriority]     = pe._initializationPriority;
		pinData[kInverted]     = pe._inverted;
		pinData[kCommand]      = pe._pinCommand.toStdString();
		pinData[kCommandGroup] = static_cast<int>(pe._commandGroup);
		pinData[kRunPriority]  = fromPoint(pe._cellLocation).toStdString();
		pinData[kTabName]      = pe._tabName.toStdString();
		pinsArray.push_back(pinData);
	}
	j[kPinEntries] = pinsArray;

	json_t busArray = json_t::array();
	for (const auto& kv : _busFunctions)
	{
		const FTDIBusData& bf = kv.second;
		json_t busData;
		busData[kChipIndex]   = bf._chipIndex;
		busData[kBus]         = std::string(1, bf._bus);
		busData[kBusFunction] = static_cast<int>(bf._busFunction);
		busArray.push_back(busData);
	}
	j[kBusEntries] = busArray;
}

// -----------------------------------------------------------------------

PinID _FTDIPlatformConfiguration::getSetPinIndex(int chipIndex, Bus bus, PinID pinId)
{
	int pinOffset = 0;
	switch (bus)
	{
	case 'A': pinOffset =  0; break;
	case 'B': pinOffset =  8; break;
	case 'C': pinOffset = 16; break;
	case 'D': pinOffset = 24; break;
	}
	return static_cast<PinID>(chipIndex * 32 + pinOffset + static_cast<int>(pinId));
}
