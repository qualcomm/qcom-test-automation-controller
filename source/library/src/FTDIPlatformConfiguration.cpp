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
static const char* kTabs           = "tabs";

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
static const char* kButtons          = "buttons";
static const char* kButtonToolTip    = "tooltip";
static const char* kModificationDate = "modification_date";
static const char* kFileVersion      = "fileVersion";
static const char* kVariables   = "variables";
static const char* kScript      = "script";
static const char* kLabel       = "label";
static const char* kTab         = "tab";
static const char* kCellLocation = "cellLocation";
static const char* kDefaultValue = "default_value";
static const char* kType        = "type";

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

	// When chipCount == 1 (generic ALPACA-LITE board with no tcnf), apply the
	// same hardcoded default pin mappings that the Qt build uses.  This ensures
	// the device is usable before its EEPROM has been programmed with a specific
	// USB descriptor.
	if (chipCount != 1)
		return;

	// Helper lambda to insert a pin + D2XX bus entry
	auto addPin = [&](PinID chipPin, Bus bus,
	                  bool enabled, bool input, bool initialValue, bool inverted,
	                  int priority, const char* label, const char* cmd,
	                  const char* tooltip, CommandGroups group,
	                  const char* tab, qtac::Point cell)
	{
		FTDIPinData pd(static_cast<ChipIndex>(0), bus, chipPin);
		pd._setPin                = getSetPinIndex(0, bus, chipPin);
		pd._enabled               = enabled;
		pd._input                 = input;
		pd._initialValue          = initialValue;
		pd._inverted              = inverted;
		pd._initializationPriority = priority;
		pd._pinLabel              = label;
		pd._pinCommand            = cmd;
		pd._pinTooltip            = tooltip;
		pd._commandGroup          = group;
		pd._tabName               = tab;
		pd._cellLocation          = cell;
		_pinEntries.insert(pd._hash, pd);

		FTDIBusData bd(static_cast<ChipIndex>(0), bus, eBusFunctionD2XX);
		_busFunctions.insert(bd._hash, bd);
	};

	// C-bus pins
	addPin(0, 'C', true, true, false, false, -1,
	       "Force PS_HOLD High", "pshold", "Force PS_HOLD to high",
	       eSwitchGroup, "General", {0, 2});
	addPin(1, 'C', true, true, false, false, -1,
	       "Disconnect UIM1", "uim1", "Disconnects the UIM 1",
	       eSwitchGroup, "General", {0, 0});
	addPin(2, 'C', true, true, false, false, -1,
	       "Headset Disconnect", "headset", "Disconnects headset",
	       eSwitchGroup, "General", {0, 5});
	addPin(3, 'C', true, true, false, false, -1,
	       "Disconnect UIM2", "uim2", "Disconnects the UIM 2",
	       eSwitchGroup, "General", {1, 0});
	addPin(4, 'C', true, true, false, false, -1,
	       "Disconnect SD Card", "sdcard", "Disconnects the SD Card",
	       eSwitchGroup, "General", {0, 3});
	addPin(5, 'C', true, true, false, false, -1,
	       "Secondary Emergency Download Mode (EDL)", "sedl", "Secondary Emergency Download Mode",
	       eSwitchGroup, "Fusion", {0, 0});
	addPin(6, 'C', true, true, false, true, 2,
	       "USB 1 (VBUS Only)", "usb1", "Disconnects VBUS 1",
	       eConnectionGroup, "General", {2, 0});
	addPin(7, 'C', true, true, false, false, -1,
	       "Secondary PM_RESIN_N_SEC", "sresn", "Fusion Secondary PM_RESIN_N",
	       eSwitchGroup, "Fusion", {0, 1});

	// D-bus pins
	addPin(0, 'D', true, true, false, false, -1,
	       "Volume Up", "volup", "VOL_UP + PWR_ON = Held for boot to UEFI menu",
	       eButtonGroup, "General", {0, 1});
	addPin(1, 'D', true, true, false, true, 2,
	       "Battery", "battery", "Battery power off/on",
	       eConnectionGroup, "General", {0, 0});
	addPin(2, 'D', true, true, false, false, -1,
	       "Volume Down", "voldn", "(PM_RESIN_N) (Held to boot to fastboot)",
	       eButtonGroup, "General", {0, 2});
	addPin(3, 'D', true, true, false, false, -1,
	       "Power Key", "pkey", "Power On (VOL_UP + PWR_ON = Held for boot to UEFI menu)",
	       eButtonGroup, "General", {0, 0});
	addPin(4, 'D', true, true, false, false, -1,
	       "EUD", "eud", "Embedded USB Debug (EUD)",
	       eSwitchGroup, "General", {0, 4});
	addPin(5, 'D', true, true, false, false, -1,
	       "EDL", "pedl", "Primary Emergency Download Mode",
	       eSwitchGroup, "General", {0, 1});
	addPin(6, 'D', true, true, true, false, 1,
	       "<type a label name>", "TC_READY_N",
	       "SW must program it to 1 to enable some output signals. False - Disables the control on some "
	       "of the FTDI pins: DDBUS 0/2/4/7, CDBUS 0/2/4/7. True - Enable the control on all the FTDI pins.",
	       eUnknownCommandGroup, "<select a group>", {-1, -1});
	addPin(7, 'D', true, false, false, true, 2,
	       "USB 0 (VBUS Only)", "usb0", "Disconnects VBUS0",
	       eConnectionGroup, "General", {1, 0});

	// Inject the classic default quick-command buttons for generic ALPACA-LITE boards.
	// These match the fallback injected by the tcnf loader when no buttons are defined.
	static const struct { const char* name; const char* cmd; const char* tip; int col; int row; }
	kDefaultButtons[] = {
		{ "Power On",              "powerOn",              "Powers on the MTP/Device",                        0, 0 },
		{ "Power Off",             "powerOff",             "Powers off the MTP/Device",                       1, 0 },
		{ "Boot to EDL",           "bootToEDL",            "Boots the device to emergency download",          2, 0 },
		{ "Boot to Fastboot",      "bootToFastboot",       "Boots the device to fastboot",                    0, 1 },
		{ "Boot to UEFI",          "bootToUEFI",           "Boots the device to UEFI Menu",                   1, 1 },
		{ "Boot to Secondary EDL", "bootToSecondaryEDL",   "Boots the device to secondary emergency download",2, 1 },
	};
	for (const auto& d : kDefaultButtons)
	{
		qtac::ButtonEntry btn;
		btn._name         = d.name;
		btn._command      = d.cmd;
		btn._tooltip      = d.tip;
		btn._commandGroup = 4;  // eQuickSettingsGroup
		btn._tab          = "General";
		btn._cellX        = d.col;
		btn._cellY        = d.row;
		_buttons.push_back(btn);
	}

	// Sort by strHash(name + tab) to match the Qt QMap iteration order.
	std::sort(_buttons.begin(), _buttons.end(),
		[](const qtac::ButtonEntry& a, const qtac::ButtonEntry& b) {
			return strHash(a._name + a._tab) < strHash(b._name + b._tab);
		});
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
    for (auto& kv : _pinEntries)
    {
        if (kv.second._tabName == tabName)
            kv.second._tabName = qtac::String();
    }
}

void _FTDIPlatformConfiguration::cascadeTabRename(const qtac::String& oldName,
                                                   const qtac::String& newName)
{
    for (auto& kv : _pinEntries)
    {
        if (kv.second._tabName == oldName)
            kv.second._tabName = newName;
    }
}

// -----------------------------------------------------------------------
// JSON I/O
// -----------------------------------------------------------------------

bool _FTDIPlatformConfiguration::read(json_t& j)
{
	const auto* jobj = j.if_object();
	if (!jobj) return false;

	if (jobj->contains(kChipCount))
		_chipCount = qtac::json_util::toInt((*jobj)[kChipCount]);

	if (jobj->contains(kModificationDate) && (*jobj)[kModificationDate].is_string())
		_modificationDate = qtac::String(qtac::json_util::toString((*jobj)[kModificationDate]));
	if (jobj->contains(kFileVersion) && ((*jobj)[kFileVersion].is_int64() || (*jobj)[kFileVersion].is_uint64()))
		_fileVersion = qtac::json_util::toInt((*jobj)[kFileVersion]);

	// --- Parse tabs array: collect visible tabs sorted by ordinal ---
	// Skip the fixed app tabs (General, Device Info, Terminal) — those are always present.
	_tabs.clear();
	if (jobj->contains(kTabs) && (*jobj)[kTabs].is_array())
	{
		using TabEntry = std::pair<int, std::string>;
		std::vector<TabEntry> tabEntries;
		for (const auto& jtVal : (*jobj)[kTabs].as_array())
		{
			const auto* jt = jtVal.if_object();
			if (!jt) continue;
			bool visible = true;
			if (jt->contains("visible")) visible = qtac::json_util::toBool((*jt)["visible"]);
			if (!visible) continue;

			std::string name;
			if (jt->contains("name")) name = qtac::json_util::toString((*jt)["name"]);
			if (name.empty() || name == "General" || name == "Device Info" || name == "Terminal")
				continue;

			int ordinal = 999;
			if (jt->contains("ordinal")) ordinal = qtac::json_util::toInt((*jt)["ordinal"]);
			tabEntries.push_back(std::make_pair(ordinal, name));
		}
		std::sort(tabEntries.begin(), tabEntries.end());
		for (const auto& te : tabEntries)
			_tabs.append(qtac::String(te.second));
	}

	if (jobj->contains(kPinEntries) && (*jobj)[kPinEntries].is_array())
	{
		_pinEntries.clear();
		for (const auto& peVal : (*jobj)[kPinEntries].as_array())
		{
			const auto* pe = peVal.if_object();
			if (!pe) continue;
			ChipIndex chipIndex{0};
			Bus bus{0};
			PinID pinId{0};

			if (pe->contains(kChipIndex)) chipIndex = qtac::json_util::toInt((*pe)[kChipIndex]);
			if (pe->contains(kBus))
			{
				std::string busStr = qtac::json_util::toString((*pe)[kBus]);
				if (!busStr.empty()) bus = busStr[0];
			}
			if (pe->contains(kPinNumber))
				pinId = static_cast<PinID>(std::stoi(qtac::json_util::toString((*pe)[kPinNumber])));

			FTDIPinData pinData(chipIndex, bus, pinId);
			pinData._setPin = getSetPinIndex(chipIndex, bus, pinId);

			if (pe->contains(kEnabled))       pinData._enabled  = qtac::json_util::toBool((*pe)[kEnabled]);
			if (pe->contains(kInput))         pinData._input    = qtac::json_util::toBool((*pe)[kInput]);
			if (pe->contains(kName))          pinData._pinLabel    = qtac::json_util::toString((*pe)[kName]);
			if (pe->contains(kToolTip))       pinData._pinTooltip  = qtac::json_util::toString((*pe)[kToolTip]);
			if (pe->contains(kInitialValue))  pinData._initialValue = qtac::json_util::toBool((*pe)[kInitialValue]);
			if (pe->contains(kPriority))      pinData._initializationPriority = qtac::json_util::toInt((*pe)[kPriority]);
			if (pe->contains(kInverted))      pinData._inverted = qtac::json_util::toBool((*pe)[kInverted]);
			if (pe->contains(kCommand))       pinData._pinCommand = qtac::json_util::toString((*pe)[kCommand]);
			if (pe->contains(kCommandGroup))  pinData._commandGroup = static_cast<CommandGroups>(qtac::json_util::toInt((*pe)[kCommandGroup]));
			if (pe->contains(kTabName))       pinData._tabName = qtac::json_util::toString((*pe)[kTabName]);

			if (pe->contains(kRunPriority))
			{
				std::string pointStr = qtac::json_util::toString((*pe)[kRunPriority]);
				qtac::Point pt = toPoint(qtac::String(pointStr));
				if (pt.x() >= 0 && pt.y() >= 0)
					pinData._cellLocation = pt;
				else
					pinData._cellLocation = qtac::Point(-1, -1);
			}

			_pinEntries.insert(pinData._hash, pinData);
		}
	}

	if (jobj->contains(kBusEntries) && (*jobj)[kBusEntries].is_array())
	{
		_busFunctions.clear();
		for (const auto& beVal : (*jobj)[kBusEntries].as_array())
		{
			const auto* be = beVal.if_object();
			if (!be) continue;
			ChipIndex chipIndex{0};
			Bus bus{0};
			FTDIBusFunction bf{eBusFunctionUnknown};

			if (be->contains(kChipIndex)) chipIndex = qtac::json_util::toInt((*be)[kChipIndex]);
			if (be->contains(kBus))
			{
				std::string busStr = qtac::json_util::toString((*be)[kBus]);
				if (!busStr.empty()) bus = busStr[0];
			}
			if (be->contains(kBusFunction))
				bf = static_cast<FTDIBusFunction>(qtac::json_util::toInt((*be)[kBusFunction]));

			FTDIBusData busData(chipIndex, bus, bf);
			_busFunctions.insert(busData._hash, busData);
		}
	}

	// --- Load buttons ---
	_buttons.clear();
	if (jobj->contains(kButtons) && (*jobj)[kButtons].is_array())
	{
		for (const auto& jbVal : (*jobj)[kButtons].as_array())
		{
			const auto* jb = jbVal.if_object();
			if (!jb) continue;
			qtac::ButtonEntry btn;
			if (jb->contains(kName))         btn._name         = qtac::json_util::toString((*jb)[kName]);
			if (jb->contains(kCommand))      btn._command      = qtac::json_util::toString((*jb)[kCommand]);
			if (jb->contains(kCommandGroup)) btn._commandGroup = qtac::json_util::toInt((*jb)[kCommandGroup]);
			if (jb->contains(kTab))          btn._tab          = qtac::json_util::toString((*jb)[kTab]);
			if (jb->contains(kButtonToolTip)) btn._tooltip     = qtac::json_util::toString((*jb)[kButtonToolTip]);
			if (jb->contains(kCellLocation))
			{
				qtac::Point pt = toPoint(qtac::String(qtac::json_util::toString((*jb)[kCellLocation])));
				btn._cellX = pt.x();
				btn._cellY = pt.y();
			}

			if (!btn._command.isEmpty() && !btn._name.isEmpty())
				_buttons.push_back(btn);
		}
	}

	// If no buttons were defined in the tcnf, inject the classic default Quick Settings.
	if (_buttons.empty())
	{
		static const struct { const char* name; const char* cmd; const char* tip; int col; int row; }
		kDefaultButtons[] = {
			{ "Power On",              "powerOn",              "Powers on the MTP/Device",                        0, 0 },
			{ "Power Off",             "powerOff",             "Powers off the MTP/Device",                       1, 0 },
			{ "Boot to EDL",           "bootToEDL",            "Boots the device to emergency download",          2, 0 },
			{ "Boot to Fastboot",      "bootToFastboot",       "Boots the device to fastboot",                    0, 1 },
			{ "Boot to UEFI",          "bootToUEFI",           "Boots the device to UEFI Menu",                   1, 1 },
			{ "Boot to Secondary EDL", "bootToSecondaryEDL",   "Boots the device to secondary emergency download",2, 1 },
		};
		for (const auto& d : kDefaultButtons)
		{
			qtac::ButtonEntry btn;
			btn._name         = d.name;
			btn._command      = d.cmd;
			btn._tooltip      = d.tip;
			btn._commandGroup = 4;  // eQuickSettingsGroup
			btn._tab          = "General";
			btn._cellX        = d.col;
			btn._cellY        = d.row;
			_buttons.push_back(btn);
		}
	}

	// --- Load variables ---
	_variables.clear();
	if (jobj->contains(kVariables) && (*jobj)[kVariables].is_array())
	{
		for (const auto& jvVal : (*jobj)[kVariables].as_array())
		{
			const auto* jv = jvVal.if_object();
			if (!jv) continue;
			qtac::VariableEntry var;
			if (jv->contains(kName))    var._name    = qtac::json_util::toString((*jv)[kName]);
			if (jv->contains(kLabel))   var._label   = qtac::json_util::toString((*jv)[kLabel]);
			if (jv->contains(kToolTip)) var._tooltip = qtac::json_util::toString((*jv)[kToolTip]);
			if (jv->contains(kType))    var._type    = static_cast<qtac::VariableType>(qtac::json_util::toInt((*jv)[kType]));
			if (jv->contains(kDefaultValue))
			{
				std::string dv = qtac::json_util::toString((*jv)[kDefaultValue]);
				if (var._type == qtac::VariableType::Boolean)
					var._defaultValue = (dv == "1" || dv == "true");
				else if (var._type == qtac::VariableType::Float)
				{
					try { var._defaultValue = std::stof(dv); } catch (...) {}
				}
				else
				{
					try { var._defaultValue = static_cast<unsigned int>(std::stoul(dv)); }
					catch (...) { var._defaultValue = 0u; }
				}
			}
			if (jv->contains(kCellLocation))
			{
				qtac::Point pt = toPoint(qtac::String(qtac::json_util::toString((*jv)[kCellLocation])));
				var._cellX = pt.x();
				var._cellY = pt.y();
			}

			if (!var._name.isEmpty())
				_variables[var._name] = var;
		}
	}

	// Inject defaults for script timing variables if not defined in the tcnf.
	auto ensureVar = [&](const char* name, const char* label, const char* tooltip,
	                     unsigned int defaultMs, int cellX, int cellY)
	{
		if (_variables.find(qtac::String(name)) == _variables.end())
		{
			qtac::VariableEntry v;
			v._name         = name;
			v._label        = label;
			v._tooltip      = tooltip;
			v._type         = qtac::VariableType::Integer;
			v._defaultValue = defaultMs;
			v._cellX        = cellX;
			v._cellY        = cellY;
			_variables[v._name] = v;
		}
	};
	ensureVar("edl",      "EDL timing (ms)",      "Configurable Boot to EDL timing in milliseconds",      1300, 0, 0);
	ensureVar("uefi",     "UEFI timing (ms)",      "Configurable Boot to UEFI timing in milliseconds",     8000, 0, 1);
	ensureVar("fastboot", "Fastboot timing (ms)",  "Configurable Boot to fastboot timing in milliseconds", 8000, 1, 0);

	// --- Load and parse script ---
	if (jobj->contains(kScript) && (*jobj)[kScript].is_string())
	{
		qtac::String scriptText = qtac::json_util::toString((*jobj)[kScript]);

		TACCommands cmds;
		FTDIPinList activePins = getActivePins();
		for (const auto& fp : activePins)
		{
			if (fp._pinCommand.isEmpty()) continue;
			TACCommand tc;
			tc._pin      = fp._setPin;
			tc._command  = fp._pinCommand;
			cmds.append(tc);
		}

		_script.parseScript(scriptText, _variables, cmds);
	}

	return true;
}

void _FTDIPlatformConfiguration::loadDefaultScript(const qtac::String& scriptText)
{
	if (scriptText.isEmpty() || !_script.isEmpty())
		return;

	TACCommands cmds;
	FTDIPinList activePins = getActivePins();
	for (const auto& fp : activePins)
	{
		if (fp._pinCommand.isEmpty()) continue;
		TACCommand tc;
		tc._pin     = fp._setPin;
		tc._command = fp._pinCommand;
		cmds.append(tc);
	}
	_script.parseScript(scriptText, _variables, cmds);
}

void _FTDIPlatformConfiguration::write(json_t& j)
{
	auto& jobj = j.as_object();
	jobj[kChipCount] = _chipCount;

	boost::json::array pinsArray;
	for (const auto& kv : _pinEntries)
	{
		const FTDIPinData& pe = kv.second;
		boost::json::object pinData;
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
	jobj[kPinEntries] = pinsArray;

	boost::json::array busArray;
	for (const auto& kv : _busFunctions)
	{
		const FTDIBusData& bf = kv.second;
		boost::json::object busData;
		busData[kChipIndex]   = bf._chipIndex;
		busData[kBus]         = std::string(1, bf._bus);
		busData[kBusFunction] = static_cast<int>(bf._busFunction);
		busArray.push_back(busData);
	}
	jobj[kBusEntries] = busArray;
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
