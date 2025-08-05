// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/


#include "FTDIPlatformConfiguration.h"

// QCommon
#include "DebugBoardType.h"
#include "Range.h"
#include "PlatformConfigurationException.h"
#include "StringUtilities.h"

// QT
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>

const QString kChipCount(QStringLiteral("chip_count"));
const QString kChipIndex(QStringLiteral("chip_index"));
const QString kBus(QStringLiteral("bus"));
const QString kPinNumber(QStringLiteral("pin_number"));
const QString kEnabled(QStringLiteral("enabled"));
const QString kInput(QStringLiteral("input"));
const QString kName(QStringLiteral("name"));
const QString kToolTip(QStringLiteral("help_hint"));
const QString kInitialValue(QStringLiteral("initial_value"));
const QString kPriority(QStringLiteral("priority"));
const QString kInverted(QStringLiteral("inverted"));
const QString kCommand(QStringLiteral("command"));
const QString kCommandGroup(QStringLiteral("command_group"));
const QString kRunPriority(QStringLiteral("run_priority"));
const QString kTabName(QStringLiteral("group"));
const QString kBusFunction(QStringLiteral("bus_function"));

const QString kPinEntries(QStringLiteral("pins"));
const QString kBusEntries(QStringLiteral("bus"));

const int kMaximumChipCount{4};

_FTDIPlatformConfiguration::_FTDIPlatformConfiguration
(
	quint16 chipCount
)
{
	_resetActive = false;

	// Add default tabs to _editorTabs
	Tab generalTab, deviceInfoTab, fusionTab, terminalTab;

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
	fusionTab._visible = true;
	fusionTab._moveable = true;
	fusionTab._configurable = false;
	fusionTab._ordinal = 3;
	fusionTab._userTab = false;

	terminalTab._name = "Terminal";
	terminalTab._visible = true;
	terminalTab._moveable = true;
	terminalTab._configurable = false;
	terminalTab._ordinal = 4;
	terminalTab._userTab = false;

	_tabs.append(generalTab);
	_tabs.append(deviceInfoTab);
	_tabs.append(fusionTab);
	_tabs.append(terminalTab);

	initialize(chipCount);
}

void _FTDIPlatformConfiguration::initialize
(
	quint16 chipCount
)
{
	_platform = eFTDI;

	static QList<Bus> busList;
	static QList<quint32> pinList = {0, 1, 2, 3, 4, 5, 6, 7};

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
				pinData._cellLocation = QPoint(-1,-1);

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
	pinData._cellLocation = QPoint(0,2);

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
	pinData._cellLocation = QPoint(0,0);

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
	pinData._cellLocation = QPoint(0,5);

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
	pinData._cellLocation = QPoint(1,0);

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
	pinData._cellLocation = QPoint(0,3);

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
	pinData._cellLocation = QPoint(0,0);

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
	pinData._cellLocation = QPoint(2,0);

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
	pinData._cellLocation = QPoint(0,1);

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
	pinData._cellLocation = QPoint(0,1);

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
	pinData._cellLocation = QPoint(0,0);

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
	pinData._cellLocation = QPoint(0,2);

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
	pinData._cellLocation = QPoint(0,0);

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
	pinData._cellLocation = QPoint(0,4);

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
	pinData._cellLocation = QPoint(0,1);

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
	pinData._cellLocation = QPoint(-1,-1);

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
	pinData._cellLocation = QPoint(1,0);

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
		result = _pinEntries[hash];
	}

	return result;
}

FTDIPinList _FTDIPlatformConfiguration::getAllPins() const
{
	FTDIPinList result;

	for (const auto& pinEntry : std::as_const(_pinEntries))
	{
		result.append(pinEntry);
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

	for (const auto& pinEntry : std::as_const(_pinEntries))
	{
		if (pinEntry._enabled == true)
			result.append(pinEntry);
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
		for (const auto& pinEntry : std::as_const(_pinEntries))
		{
			if (pinEntry._enabled == true)
			{
				if (pinEntry._chipIndex == chipIndex)
				{
					if (pinEntry._bus == bus)
					{
						result.append(pinEntry);
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
		const auto pinData = _pinEntries[hash];
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
		const auto pinData = _pinEntries[hash];
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
		const auto pinData = _pinEntries[hash];
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
		auto pinData = _pinEntries[hash];
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
		const auto pinData = _pinEntries[hash];
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

QString _FTDIPlatformConfiguration::getPinLabel
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	QString result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinLabel;
	}
	return result;
}

void _FTDIPlatformConfiguration::setPinLabel
(
	HashType hash,
	const QString& pinLabel
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
	const QString& pinLabel
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinLabel(hash, pinLabel);
}

QString _FTDIPlatformConfiguration::getPinTooltip
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	QString result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinTooltip;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinTooltip
(
	HashType hash,
	const QString& pinTooltip
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
	const QString& pinTooltip
)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setPinTooltip(hash, pinTooltip);
}

QString _FTDIPlatformConfiguration::getPinCommand
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	QString result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._pinCommand;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinCommand
(
	HashType hash,
	const QString& pinCommand
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
	const QString &pinCommand
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
		const auto pinData = _pinEntries[hash];
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

QString _FTDIPlatformConfiguration::getTabName
(
	const ChipIndex chipIndex,
	const Bus& busName,
	const PinID pinId
) const
{
	QString result;

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._tabName;
	}

	return result;
}

void _FTDIPlatformConfiguration::setTabName
(
	HashType hash,
	const QString& tabName
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
	const QString& tabName)
{
	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);
	setTabName(hash, tabName);
}

QPoint _FTDIPlatformConfiguration::getPinCellLocation(const ChipIndex chipIndex, const Bus& busName, const PinID pinId) const
{
	QPoint result(-1,-1);

	HashType hash = FTDIPinData::makeFTDIHash(chipIndex, busName, pinId);

	if (_pinEntries.find(hash) != _pinEntries.end())
	{
		const auto pinData = _pinEntries[hash];
		result = pinData._cellLocation;
	}

	return result;
}

void _FTDIPlatformConfiguration::setPinCellLocation(HashType hash, const QPoint& cellLocation)
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
	const QPoint& cellLocation
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
		result = _busFunctions[hashType];
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

void _FTDIPlatformConfiguration::cascadeTabDelete(const QString &deleteMe)
{
	for (int idx=0; idx<_tabs.length(); idx++)
	{
		if (_tabs.at(idx)._name == deleteMe)
		{
			_tabs.removeAt(idx);
		}
	}

	for (auto& pinEntry: _pinEntries)
	{
		if (pinEntry._tabName == deleteMe)
			pinEntry._tabName = QString();
	}
}

void _FTDIPlatformConfiguration::cascadeTabRename
(
	const QString& oldName,
	const QString& newName
)
{
	for (auto& pinEntry: _pinEntries)
	{
		if (pinEntry._tabName == oldName)
			pinEntry._tabName = newName;
	}
}

bool _FTDIPlatformConfiguration::read(QJsonObject& parentLevel)
{
	bool result{false};

	result = _PlatformConfiguration::read(parentLevel);
	if (result == true)
	{
		QJsonValue jsonValue;

		jsonValue = parentLevel.value(kChipCount);
		if (jsonValue.isNull() == false)
			_chipCount = jsonValue.toInt();

		if (parentLevel.contains(kPinEntries) && parentLevel[kPinEntries].isArray())
		{
			QJsonArray pinsArray = parentLevel[kPinEntries].toArray();

			_pinEntries.clear();

			for (auto pinIndex: range(pinsArray.size()))
			{
				QJsonObject pinEntry = pinsArray[pinIndex].toObject();

				ChipIndex chipIndex{0};
				Bus bus;
				PinID pinId{0};

				if (pinEntry.contains(kChipIndex))
					chipIndex = pinEntry[kChipIndex].toInt();

				if (pinEntry.contains(kBus))
					bus = pinEntry[kBus].toString().at(0);

				if (pinEntry.contains(kPinNumber))
					pinId = pinEntry[kPinNumber].toString().toInt();

				FTDIPinData pinData(chipIndex, bus, pinId);

				pinData._setPin = getSetPinIndex(chipIndex, bus, pinId);

				if (pinEntry.contains(kEnabled))
					pinData._enabled = pinEntry[kEnabled].toBool();

				if (pinEntry.contains(kName))
					pinData._pinLabel = pinEntry[kName].toString();

				if (pinEntry.contains(kToolTip))
					pinData._pinTooltip = pinEntry[kToolTip].toString();

				if (pinEntry.contains(kInitialValue))
					pinData._initialValue = pinEntry[kInitialValue].toBool();

				if (pinEntry.contains(kPriority))
					pinData._initializationPriority = pinEntry[kPriority].toInt();

				if (pinEntry.contains(kInverted))
					pinData._inverted = pinEntry[kInverted].toBool();

				if (pinEntry.contains(kCommand))
					pinData._pinCommand = pinEntry[kCommand].toString();

				if (pinEntry.contains(kCommandGroup))
					pinData._commandGroup = static_cast<CommandGroups>(pinEntry[kCommandGroup].toInt());

				if (pinEntry.contains(kRunPriority))
				{
					pinData._cellLocation = QPoint(-1, -1);
					QString pointString = pinEntry[kRunPriority].toString();
					if (pointString.size() > 0)
					{
						QPoint point = toQPoint(pointString);
						if (point.x() >= 0 && point.y() >= 0)
							pinData._cellLocation = point;
					}
				}

				if (pinEntry.contains(kTabName))
					pinData._tabName = pinEntry[kTabName].toString();

				_pinEntries[pinData._hash] = pinData;
			}
		}

		if (parentLevel.contains(kBusEntries) && parentLevel[kBusEntries].isArray())
		{
			QJsonArray busArray = parentLevel[kBusEntries].toArray();

			_busFunctions.clear();

			for (auto pinIndex: range(busArray.size()))
			{
				QJsonObject busEntry = busArray[pinIndex].toObject();

				ChipIndex chipIndex{0};
				Bus bus;
				FTDIBusFunction busFunction{eBusFunctionUnknown};

				if (busEntry.contains(kChipIndex))
					chipIndex = busEntry[kChipIndex].toInt();

				if (busEntry.contains(kBus))
					bus = busEntry[kBus].toString().at(0);

				if (busEntry.contains(kBusFunction))
					busFunction = FTDIBusFunction(busEntry[kBusFunction].toInt());

				FTDIBusData busData(chipIndex, bus, busFunction);

				_busFunctions[busData._hash] = busData;
			}
		}
	}

	return result;
}

void _FTDIPlatformConfiguration::write(QJsonObject& parentLevel)
{
	_PlatformConfiguration::write(parentLevel);

	parentLevel[kChipCount] = _chipCount;

	QJsonArray pinsArray;

	for (auto& pinEntry : _pinEntries)
	{
		QJsonObject pinData;

		pinData[kChipIndex] = pinEntry._chipIndex;
		pinData[kBus] = QVariant(pinEntry._bus).toString();
		pinData[kPinNumber] = QString::number(pinEntry._chipPin);
		pinData[kEnabled] = pinEntry._enabled;
		pinData[kInput] = pinEntry._input;
		pinData[kName] = pinEntry._pinLabel;
		pinData[kToolTip] = pinEntry._pinTooltip;
		pinData[kInitialValue] = pinEntry._initialValue;
		pinData[kPriority] = pinEntry._initializationPriority;
		pinData[kInverted] = pinEntry._inverted;
		pinData[kCommand] = pinEntry._pinCommand;
		pinData[kCommandGroup] = pinEntry._commandGroup;
		pinData[kRunPriority] = fromQPoint(pinEntry._cellLocation);

		pinData[kTabName] = pinEntry._tabName;

		pinsArray.append(pinData);
	}

	parentLevel[kPinEntries] = pinsArray;

	QJsonArray busArray;

	for (const auto& busFunction : std::as_const(_busFunctions))
	{
		QJsonObject busData;

		busData[kChipIndex] = busFunction._chipIndex;
		busData[kBus] = QVariant(busFunction._bus).toString();
		busData[kBusFunction] = busFunction._busFunction;

		busArray.append(busData);
	}

	parentLevel[kBusEntries] = busArray;
}

PinID _FTDIPlatformConfiguration::getSetPinIndex
(
	const int chipIndex,
	const Bus &bus,
	PinID pinId
)
{
	int pinIndex{0};

	switch (bus.toLatin1())
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

