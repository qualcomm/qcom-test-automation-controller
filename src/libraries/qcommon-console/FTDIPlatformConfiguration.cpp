// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "FTDIPlatformConfiguration.h"

// QCommon
#include "DebugBoardType.h"
#include "Range.h"
#include "PlatformConfigurationException.h"
#include "StringUtilities.h"

#include "ConfigurationFile.h"

#include <cstdint>
#include <string>

const std::string kChipCount("chip_count");
const std::string kChipIndex("chip_index");
const std::string kBus("bus");
const std::string kPinNumber("pin_number");
const std::string kEnabled("enabled");
const std::string kInput("input");
const std::string kName("name");
const std::string kToolTip("help_hint");
const std::string kInitialValue("initial_value");
const std::string kPriority("priority");
const std::string kInverted("inverted");
const std::string kCommand("command");
const std::string kCommandGroup("command_group");
const std::string kRunPriority("run_priority");
const std::string kTabName("group");
const std::string kBusFunction("bus_function");

const std::string kPinEntries("pins");
const std::string kBusEntries("bus");

const int kMaximumChipCount{4};

_FTDIPlatformConfiguration::_FTDIPlatformConfiguration
(
	uint16_t chipCount
)
{
	_resetActive = false;

	// Add default tabs to _editorTabs
	Tab generalTab, deviceInfoTab, fusionTab;

	generalTab._name = "General";
	generalTab._visible = true;
	generalTab._moveable = false;
	generalTab._configurable = true;
	generalTab._ordinal = 0;
	generalTab._userTab = false;

	deviceInfoTab._name = "Device Info";
	deviceInfoTab._visible = true;
	deviceInfoTab._moveable = false;
	deviceInfoTab._configurable = false;
	deviceInfoTab._ordinal = 1;
	deviceInfoTab._userTab = false;

	fusionTab._name = "Fusion";
	fusionTab._visible = false;
	fusionTab._moveable = true;
	fusionTab._configurable = false;
	fusionTab._ordinal = 3;
	fusionTab._userTab = false;

	_tabs.push_back(generalTab);
	_tabs.push_back(deviceInfoTab);
	_tabs.push_back(fusionTab);

	initialize(chipCount);
}

void _FTDIPlatformConfiguration::initialize
(
	uint16_t chipCount
)
{
	_platform = eFTDI;

	static std::vector<Bus> busList;
	static std::vector<uint32_t> pinList = {0, 1, 2, 3, 4, 5, 6, 7};

	if (chipCount > kMaximumChipCount)
	{
		throw PlatformConfigurationException("Invalid chip count");
	}

	_chipCount = chipCount;

	for (auto chipIndex: range(chipCount))
	{
		if (chipIndex == 0)
			busList = {Bus('A'), Bus('B')};
		else
			busList = {Bus('A'), Bus('B'), Bus('C'), Bus('D')};

		for (auto& busIndex: busList)
		{
			for (auto& pinIndex: pinList)
			{
				FTDIPinData pinData(chipIndex, busIndex, pinIndex);
				FTDIBusData busData(chipIndex, busIndex, (busIndex == 'A' || busIndex == 'B') ? eBusFunctionVCP: eBusFunctionD2XX);

				pinData._initializationPriority = -1;
				pinData._commandGroup = eUnknownCommandGroup;
				pinData._cellLocation = std::make_pair(-1, -1);

				_pinEntries[pinData._hash] = pinData;
				_busFunctions[busData._hash] = busData;
			}
		}
	}

	FTDIPinData pinData;
	FTDIBusData busData;

	// CDBUS0
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('C');
	pinData._chipPin= 0;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Force PS_HOLD High";
	pinData._pinCommand = "pshold";
	pinData._pinTooltip = "Force PS_HOLD to high";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 2);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// CDBUS1
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('C');
	pinData._chipPin= 1;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Disconnect UIM1";
	pinData._pinCommand = "uim1";
	pinData._pinTooltip = "Disconnects the UIM 1";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 0);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// CDBUS2
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('C');
	pinData._chipPin= 2;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Headset Disconnect";
	pinData._pinCommand = "headset";
	pinData._pinTooltip = "Disconnects headset";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 5);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// CDBUS3
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('C');
	pinData._chipPin= 3;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Disconnect UIM2";
	pinData._pinCommand = "uim2";
	pinData._pinTooltip = "Disconnects the UIM 2";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(1, 0);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// CDBUS4
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('C');
	pinData._chipPin= 4;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Disconnect SD Card";
	pinData._pinCommand = "sdcard";
	pinData._pinTooltip = "Disconnects the SD Card";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 3);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// CDBUS5
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('C');
	pinData._chipPin= 5;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Secondary Emergency Download Mode (EDL)";
	pinData._pinCommand = "sedl";
	pinData._pinTooltip = "Secondary Emergency Download Mode";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "Fusion";
	pinData._cellLocation = std::make_pair(0, 0);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// CDBUS6
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('C');
	pinData._chipPin= 6;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = true;
	pinData._initializationPriority = 2;
	pinData._pinLabel = "USB 1 (VBUS Only)";
	pinData._pinCommand = "usb1";
	pinData._pinTooltip = "Disconnects VBUS 1";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(2, 0);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// CDBUS7
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('C');
	pinData._chipPin= 7;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Secondary PM_RESIN_N_SEC";
	pinData._pinCommand = "sresn";
	pinData._pinTooltip = "Fusion Secondary PM_RESIN_N";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "Fusion";
	pinData._cellLocation = std::make_pair(0, 1);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// DDBUS0
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('D');
	pinData._chipPin= 0;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Volume Up";
	pinData._pinCommand = "volup";
	pinData._pinTooltip = "VOL_UP + PWR_ON = Held for boot to UEFI menu";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 1);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// DDBUS1
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('D');
	pinData._chipPin= 1;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = true;
	pinData._initializationPriority = 2;
	pinData._pinLabel = "Battery";
	pinData._pinCommand = "battery";
	pinData._pinTooltip = "Battery power off/on";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 0);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// DDBUS2
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('D');
	pinData._chipPin= 2;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Volume Down";
	pinData._pinCommand = "voldn";
	pinData._pinTooltip = "(PM_RESIN_N) (Held to boot to fastboot)";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 2);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// DDBUS3
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('D');
	pinData._chipPin= 3;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "Power Key";
	pinData._pinCommand = "pkey";
	pinData._pinTooltip = "Power On (VOL_UP + PWR_ON = Held for boot to UEFI menu)";
	pinData._commandGroup = eButtonGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 0);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// DDBUS4
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('D');
	pinData._chipPin= 4;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "EUD";
	pinData._pinCommand = "eud";
	pinData._pinTooltip = "Embedded USB Debug (EUD)";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 4);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// DDBUS5
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('D');
	pinData._chipPin= 5;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = false;
	pinData._inverted = false;
	pinData._initializationPriority = -1;
	pinData._pinLabel = "EDL";
	pinData._pinCommand = "pedl";
	pinData._pinTooltip = "Primary Emergency Download Mode";
	pinData._commandGroup = eSwitchGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(0, 1);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// DDBUS6
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('D');
	pinData._chipPin= 6;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = true;
	pinData._initialValue = true;
	pinData._inverted = false;
	pinData._initializationPriority = 1;
	pinData._pinLabel = "<type a label name>";
	pinData._pinCommand = "TC_READY_N";
	pinData._pinTooltip = "SW must program it to 1 to enable some output signals. False - Disables the control on some of the FTDI pins: DDBUS 0/2/4/7, CDBUS 0/2/4/7. True - Enable the control on all the FTDI pins.";
	pinData._commandGroup = eUnknownCommandGroup;
	pinData._tabName = "<select a group>";
	pinData._cellLocation = std::make_pair(-1, -1);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;

	// DDBUS7
	pinData.clear();
	busData.clear();

	pinData._chipIndex = 0;
	pinData._bus = Bus('D');
	pinData._chipPin= 7;
	pinData._setPin = getSetPinIndex(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._hash = FTDIPinData::makeFTDIHash(pinData._chipIndex, pinData._bus, pinData._chipPin);
	pinData._enabled = true;
	pinData._input = false;
	pinData._initialValue = false;
	pinData._inverted = true;
	pinData._initializationPriority = 2;
	pinData._pinLabel = "USB 0 (VBUS Only)";
	pinData._pinCommand = "usb0";
	pinData._pinTooltip = "Disconnects VBUS0";
	pinData._commandGroup = eConnectionGroup;
	pinData._tabName = "General";
	pinData._cellLocation = std::make_pair(1, 0);

	busData._hash = FTDIBusData::makeFTDIHash(pinData._chipIndex, pinData._bus);
	busData._chipIndex = pinData._chipIndex;
	busData._bus = pinData._bus;
	busData._busFunction = eBusFunctionD2XX;

	_pinEntries[pinData._hash] = pinData;
	_busFunctions[busData._hash] = busData;
}

_FTDIPlatformConfiguration::~_FTDIPlatformConfiguration()
{

}

FTDIPinData _FTDIPlatformConfiguration::getPinData(ChipIndex chipIndex, Bus bus, PinID pin)
{
	FTDIPinData result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, bus, pin);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		result = _pinEntries.at(hash);
	}

	return result;
}

FTDIPinList _FTDIPlatformConfiguration::getAllPins() const
{
	FTDIPinList result;

	for (const auto& entry : _pinEntries)
	{
		result.push_back(entry.second);
	}

	auto sortLambda = [] (FTDIPinData& p1, FTDIPinData& p2) -> bool
	{
		if (p1._chipIndex != p2._chipIndex)
			return p1._chipIndex < p2._chipIndex;

		if (p1._bus != p2._bus)
			return p1._bus < p2._bus;

		return p1._chipPin < p2._chipPin;
	};

	std::sort(result.begin(), result.end(), sortLambda);

	return result;
}

FTDIPinList _FTDIPlatformConfiguration::getActivePins() const
{
	FTDIPinList result;

	for (const auto& entry : _pinEntries)
	{
		if (entry.second._enabled == true)
			result.push_back(entry.second);
	}

	auto sortLambda = [] (FTDIPinData& p1, FTDIPinData& p2) -> bool
	{
		if (p1._chipIndex != p2._chipIndex)
			return p1._chipIndex < p2._chipIndex;

		if (p1._bus != p2._bus)
			return p1._bus < p2._bus;

		return p1._chipPin < p2._chipPin;
	};

	std::sort(result.begin(), result.end(), sortLambda);

	return result;
}

FTDIPinList _FTDIPlatformConfiguration::getActivePins
(
	ChipIndex chipIndex,
	Bus bus
) const
{
	FTDIPinList result;

	FTDIBusData busData = getBusFunction(chipIndex, bus);
	if (busData._busFunction == eBusFunctionD2XX)
	{
		for (const auto& entry : _pinEntries)
		{
			if (entry.second._enabled == true)
			{
				if (entry.second._chipIndex == chipIndex)
				{
					if (entry.second._bus == bus)
					{
						result.push_back(entry.second);
					}
				}
			}
		}
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

	FTDIPinList ftdiPinList = getActivePins();

	for (const auto& ftdiPin: ftdiPinList)
	{
		PinEntry pinEntry;

		pinEntry._pin = ftdiPin._setPin;
		pinEntry._enabled = ftdiPin._enabled;
		pinEntry._hash = ftdiPin._hash;
		pinEntry._cellLocation = ftdiPin._cellLocation;
		pinEntry._commandGroup = ftdiPin._commandGroup;
		pinEntry._pinCommand = ftdiPin._pinCommand;
		pinEntry._initialValue = ftdiPin._initialValue;
		pinEntry._inverted = ftdiPin._inverted;
		pinEntry._pinLabel = ftdiPin._pinLabel;
		pinEntry._pinTooltip = ftdiPin._pinTooltip;
		pinEntry._tabName = ftdiPin._tabName;

		result.push_back(pinEntry);
	}

	auto sortLambda = [] (PinEntry& p1, PinEntry& p2) -> bool
	{
		if (p1._tabName != p2._tabName)
			return p1._tabName < p2._tabName;

		if (p1._commandGroup != p2._commandGroup)
			return p1._commandGroup < p2._commandGroup;

		return (p1._pin < p2._pin);
	};

	std::sort(result.begin(), result.end(), sortLambda);

	return result;
}

bool _FTDIPlatformConfiguration::getPinEnableState
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	bool result{true};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._enabled;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinEnableState(HashType hash, bool newState)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._enabled != newState)
		{
			pinData._enabled = newState;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update state of an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setPinEnableState
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	bool newState
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinEnableState(hash, newState);
}

bool _FTDIPlatformConfiguration::getPinInputState
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	bool result{true};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._input;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinInputState(HashType hash, bool newState)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._input != newState)
		{
			pinData._input = newState;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update input state of an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setPinInputState
(
		const ChipIndex chipIndex,
		const Bus& busName,
		const PinID pinId,
		bool newState
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinInputState(hash, newState);
}

bool _FTDIPlatformConfiguration::getInitialPinValue
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	bool result{false};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._initialValue;
	}

	return result;
}

void _FTDIPlatformConfiguration::setInitialPinValue
(
	HashType hash,
	bool newState
)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		if (pinData._initialValue != newState)
		{
			pinData._initialValue = newState;

			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update initialization state for an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setInitialPinValue
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	bool newState
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setInitialPinValue(hash, newState);
}

int _FTDIPlatformConfiguration::getPinInitializationPriority
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	int result{0};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto pinData = _pinEntries.at(hash);
		result = pinData._initializationPriority;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinInitializationPriority(HashType hash, int priority)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._initializationPriority != priority)
		{
			pinData._initializationPriority = priority;

			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update priority of an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setPinInitializationPriority
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	int priority
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinInitializationPriority(hash, priority);
}

bool _FTDIPlatformConfiguration::getPinInvertedState
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	bool result{false};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._inverted;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinInvertedState(HashType hash, bool newState)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._inverted != newState)
		{
			pinData._inverted = newState;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to update state of an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setPinInvertedState
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	bool newState
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinInvertedState(hash, newState);
}

std::string _FTDIPlatformConfiguration::getPinLabel
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	std::string result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._pinLabel;
	}
	return result;
}

void _FTDIPlatformConfiguration::setPinLabel
(
	HashType hash,
	const std::string& pinLabel
)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		pinData._pinLabel = pinLabel;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add label to an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setPinLabel
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	const std::string& pinLabel
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinLabel(hash, pinLabel);
}

std::string _FTDIPlatformConfiguration::getPinTooltip
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	std::string result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._pinTooltip;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinTooltip
(
	HashType hash,
	const std::string& pinTooltip
)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		pinData._pinTooltip = pinTooltip;
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add tooltip to an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setPinTooltip
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	const std::string& pinTooltip
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinTooltip(hash, pinTooltip);
}

std::string _FTDIPlatformConfiguration::getPinCommand
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	std::string result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._pinCommand;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinCommand
(
	HashType hash,
	const std::string& pinCommand
)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		if (pinData._pinCommand != pinCommand)
		{
			pinData._pinCommand = pinCommand;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add command to an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setPinCommand
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	const std::string &pinCommand
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinCommand(hash, pinCommand);
}

CommandGroups _FTDIPlatformConfiguration::getPinGroup
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	CommandGroups result{eUnknownCommandGroup};

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._commandGroup;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinGroup
(
	HashType hash,
	const CommandGroups commandGroup
)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		if (pinData._commandGroup != commandGroup)
		{
			pinData._commandGroup = commandGroup;

			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add category to an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setPinGroup
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	const CommandGroups commandGroup
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinGroup(hash, commandGroup);
}

std::string _FTDIPlatformConfiguration::getTabName
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	std::string result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._tabName;
	}

	return result;
}

void _FTDIPlatformConfiguration::setTabName
(
	HashType hash,
	const std::string& tabName
)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];

		if (pinData._tabName != tabName)
		{
			pinData._tabName = tabName;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add tab name for an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setTabName
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	const std::string& tabName)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setTabName(hash, tabName);
}

std::pair<int, int> _FTDIPlatformConfiguration::getPinCellLocation(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	std::pair<int, int> result(-1, -1);

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries.at(hash);
		result = pinData._cellLocation;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinCellLocation(HashType hash, const std::pair<int, int>& cellLocation)
{
	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		auto& pinData = _pinEntries[hash];
		if (pinData._cellLocation != cellLocation)
		{
			pinData._cellLocation = cellLocation;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to set cell location for an invalid pin!");
	}
}

void _FTDIPlatformConfiguration::setPinCellLocation
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId,
	const std::pair<int, int>& cellLocation
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinCellLocation(hash, cellLocation);
}

FTDIBusData _FTDIPlatformConfiguration::getBusFunction
(
	const ChipIndex chipIndex,
	const Bus& busName
) const
{
	FTDIBusData result(chipIndex, busName, eBusFunctionUnknown);

	HashType hashType = FTDIBusData::makeFTDIHash(chipIndex, busName);

	if (_busFunctions.find(hashType) != _busFunctions.end())
	{
		result = _busFunctions.at(hashType);
	}
	return result;
}

void _FTDIPlatformConfiguration::setBusFunction(HashType hash, const FTDIBusFunction busFunction)
{
	if (_busFunctions.find(hash) != _busFunctions.end())
	{
		if (_busFunctions[hash]._busFunction != busFunction)
		{
			_busFunctions[hash]._busFunction = busFunction;
			_dirty = true;
		}
	}
	else
	{
		throw PlatformConfigurationException("Attempt to add platform function to an invalid bus!");
	}
}

void _FTDIPlatformConfiguration::setBusFunction(const ChipIndex chipIndex, const Bus& busName, const FTDIBusFunction busFunction)
{

	HashType hash = FTDIBusData::makeFTDIHash(chipIndex, busName);
	setBusFunction(hash, busFunction);
}

FTDIPinSets _FTDIPlatformConfiguration::getPinSet
(
	const ChipIndex chipIndex
)
{
	FTDIPinSets result{NoOptions};

	FTDIBusData ftdiBusData;

	ftdiBusData = getBusFunction(chipIndex, 'A');
	if (ftdiBusData._bus == 'A' && ftdiBusData._busFunction == eBusFunctionD2XX)
		result.setFlag(eA);

	ftdiBusData = getBusFunction(chipIndex, 'B');
	if (ftdiBusData._bus == 'B' && ftdiBusData._busFunction == eBusFunctionD2XX)
		result.setFlag(eB);

	ftdiBusData = getBusFunction(chipIndex, 'C');
	if (ftdiBusData._bus == 'C' && ftdiBusData._busFunction == eBusFunctionD2XX)
		result.setFlag(eC);

	ftdiBusData = getBusFunction(chipIndex, 'D');
	if (ftdiBusData._bus == 'D' && ftdiBusData._busFunction == eBusFunctionD2XX)
		result.setFlag(eD);

	return result;
}

void _FTDIPlatformConfiguration::cascadeTabDelete(const std::string &deleteMe)
{
	for (auto idx = _tabs.begin(); idx != _tabs.end(); )
	{
		if (idx->_name == deleteMe)
			idx = _tabs.erase(idx);
		else
			++idx;
	}

	for (auto& entry: _pinEntries)
	{
		if (entry.second._tabName == deleteMe)
			entry.second._tabName.clear();
	}
}

void _FTDIPlatformConfiguration::cascadeTabRename
(
	const std::string& oldName,
	const std::string& newName
)
{
	for (auto& entry: _pinEntries)
	{
		if (entry.second._tabName == oldName)
			entry.second._tabName = newName;
	}
}

bool _FTDIPlatformConfiguration::readConfig(const ConfigurationFile& config)
{
	bool result{false};

	result = _PlatformConfiguration::readConfig(config);
	if (result == true)
	{
		if (config.hasProperty(kChipCount))
			_chipCount = config.getInt(kChipCount);

		const ConfigurationTable& pinsArray = config.getTable(kPinEntries);
		if (!pinsArray.empty())
		{
			_pinEntries.clear();

			for (const auto& pinEntry : pinsArray)
			{
				ChipIndex chipIndex = ConfigurationFile::getRecordInt(pinEntry, kChipIndex);
				std::string busStr = ConfigurationFile::getRecordString(pinEntry, kBus);
				Bus bus = busStr.empty() ? '\0' : busStr[0];
				PinID pinId = ConfigurationFile::getRecordInt(pinEntry, kPinNumber);

				FTDIPinData pinData(chipIndex, bus, pinId);

				pinData._setPin = getSetPinIndex(chipIndex, bus, pinId);

				if (pinEntry.find(kEnabled) != pinEntry.end())
					pinData._enabled = ConfigurationFile::getRecordBool(pinEntry, kEnabled);

				if (pinEntry.find(kName) != pinEntry.end())
					pinData._pinLabel = ConfigurationFile::getRecordString(pinEntry, kName);

				if (pinEntry.find(kToolTip) != pinEntry.end())
					pinData._pinTooltip = ConfigurationFile::getRecordString(pinEntry, kToolTip);

				if (pinEntry.find(kInitialValue) != pinEntry.end())
					pinData._initialValue = ConfigurationFile::getRecordBool(pinEntry, kInitialValue);

				if (pinEntry.find(kPriority) != pinEntry.end())
					pinData._initializationPriority = ConfigurationFile::getRecordInt(pinEntry, kPriority);

				if (pinEntry.find(kInverted) != pinEntry.end())
					pinData._inverted = ConfigurationFile::getRecordBool(pinEntry, kInverted);

				if (pinEntry.find(kCommand) != pinEntry.end())
					pinData._pinCommand = ConfigurationFile::getRecordString(pinEntry, kCommand);

				if (pinEntry.find(kCommandGroup) != pinEntry.end())
					pinData._commandGroup = static_cast<CommandGroups>(ConfigurationFile::getRecordInt(pinEntry, kCommandGroup));

				if (pinEntry.find(kRunPriority) != pinEntry.end())
				{
					pinData._cellLocation = std::make_pair(-1, -1);
					std::string pointString = ConfigurationFile::getRecordString(pinEntry, kRunPriority);
					if (pointString.size() > 0)
					{
						std::pair<int, int> point = toQPoint(pointString);
						if (point.first >= 0 && point.second >= 0)
							pinData._cellLocation = point;
					}
				}

				if (pinEntry.find(kTabName) != pinEntry.end())
					pinData._tabName = ConfigurationFile::getRecordString(pinEntry, kTabName);

				_pinEntries[pinData._hash] = pinData;
			}
		}

		const ConfigurationTable& busArray = config.getTable(kBusEntries);
		if (!busArray.empty())
		{
			_busFunctions.clear();

			for (const auto& busEntry : busArray)
			{
				ChipIndex chipIndex = ConfigurationFile::getRecordInt(busEntry, kChipIndex);
				std::string busStr = ConfigurationFile::getRecordString(busEntry, kBus);
				Bus bus = busStr.empty() ? '\0' : busStr[0];
				FTDIBusFunction busFunction = static_cast<FTDIBusFunction>(ConfigurationFile::getRecordInt(busEntry, kBusFunction));

				FTDIBusData busData(chipIndex, bus, busFunction);

				_busFunctions[busData._hash] = busData;
			}
		}
	}

	return result;
}

void _FTDIPlatformConfiguration::writeConfig(ConfigurationFile& config)
{
	_PlatformConfiguration::writeConfig(config);

	config.setInt(kChipCount, _chipCount);

	ConfigurationTable pinsArray;

	for (const auto& entry : _pinEntries)
	{
		const FTDIPinData& pinEntry = entry.second;
		ConfigurationRecord pinData;

		pinData[kChipIndex] = std::to_string(pinEntry._chipIndex);
		pinData[kBus] = std::string(1, pinEntry._bus);
		pinData[kPinNumber] = std::to_string(pinEntry._chipPin);
		pinData[kEnabled] = pinEntry._enabled ? "true" : "false";
		pinData[kInput] = pinEntry._input ? "true" : "false";
		pinData[kName] = pinEntry._pinLabel;
		pinData[kToolTip] = pinEntry._pinTooltip;
		pinData[kInitialValue] = pinEntry._initialValue ? "true" : "false";
		pinData[kPriority] = std::to_string(pinEntry._initializationPriority);
		pinData[kInverted] = pinEntry._inverted ? "true" : "false";
		pinData[kCommand] = pinEntry._pinCommand;
		pinData[kCommandGroup] = std::to_string(static_cast<int>(pinEntry._commandGroup));
		pinData[kRunPriority] = fromQPoint(pinEntry._cellLocation);
		pinData[kTabName] = pinEntry._tabName;

		pinsArray.push_back(pinData);
	}

	config.setTable(kPinEntries, pinsArray);

	ConfigurationTable busArray;

	for (const auto& entry : _busFunctions)
	{
		const FTDIBusData& busFunction = entry.second;
		ConfigurationRecord busData;

		busData[kChipIndex] = std::to_string(busFunction._chipIndex);
		busData[kBus] = std::string(1, busFunction._bus);
		busData[kBusFunction] = std::to_string(static_cast<int>(busFunction._busFunction));

		busArray.push_back(busData);
	}

	config.setTable(kBusEntries, busArray);
}

PinID _FTDIPlatformConfiguration::getSetPinIndex
(
	const int chipIndex,
	const Bus &bus,
	PinID pinId
)
{
	int pinIndex{0};

	switch (bus)
	{
	case 'A': pinIndex = 0; break;
	case 'B': pinIndex = 8; break;
	case 'C': pinIndex = 16; break;
	case 'D': pinIndex = 24; break;
	}

	pinIndex += pinId;
	pinIndex *= (chipIndex + 1);

	return static_cast<PinID>(pinIndex);
}

