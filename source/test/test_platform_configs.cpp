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

#ifndef NLOHMANN_JSON_NAMESPACE_NO_VERSION
#  define NLOHMANN_JSON_NAMESPACE_NO_VERSION 1
#endif

#include <qtac/PSOCPlatformConfiguration.h>
#include <qtac/PIC32CXPlatformConfiguration.h>
#include <qtac/FTDIPlatformConfiguration.h>

#include <cassert>
#include <cstdio>
#include <stdexcept>

// ===========================================================================
// PSOC
// ===========================================================================

static void test_psoc_factory_defaults()
{
    _PSOCPlatformConfiguration cfg;

    // Known active pins from initialize()
    PSOCPinList active = cfg.getActivePins();
    assert(!active.isEmpty());

    // Battery pin (53) must exist and be enabled
    assert(cfg.getPinEnableState(53));
    assert(cfg.getPinLabel(53) == "Battery");
    assert(cfg.getPinCommand(53) == "battery");
    assert(cfg.getPinGroup(53) == eConnectionGroup);
    assert(cfg.getClassicAction(53) == "TAC_POWER_OFF");
    assert(cfg.getPinInvertedState(53) == true);  // inverted=true in factory defaults

    // Power Key (50) must exist and be enabled
    assert(cfg.getPinEnableState(50));
    assert(cfg.getPinLabel(50) == "Power Key");
    assert(cfg.getPinGroup(50) == eButtonGroup);

    // Pin 29 is defined but not enabled
    assert(!cfg.getPinEnableState(29));

    // getAllPins includes disabled pins
    PSOCPinList all = cfg.getAllPins();
    assert(all.size() >= active.size());
    assert(all.size() > 0);
}

static void test_psoc_getters_setters()
{
    _PSOCPlatformConfiguration cfg;

    // Enable state
    cfg.setPinEnableState(29, true);
    assert(cfg.getPinEnableState(29));
    cfg.setPinEnableState(29, false);
    assert(!cfg.getPinEnableState(29));

    // Initial value
    assert(!cfg.getInitialPinValue(50));
    cfg.setInitialPinValue(50, true);
    assert(cfg.getInitialPinValue(50));
    cfg.setInitialPinValue(50, false);

    // Initialization priority
    cfg.setPinInitializationPriority(50, 5);
    assert(cfg.getPinInitializationPriority(50) == 5);

    // Inverted
    cfg.setPinInvertedState(50, true);
    assert(cfg.getPinInvertedState(50));
    cfg.setPinInvertedState(50, false);
    assert(!cfg.getPinInvertedState(50));

    // Label
    cfg.setPinLabel(50, qtac::String("MyPowerKey"));
    assert(cfg.getPinLabel(50) == "MyPowerKey");

    // Tooltip
    cfg.setPinTooltip(50, qtac::String("My tooltip"));
    assert(cfg.getPinTooltip(50) == "My tooltip");

    // Command
    cfg.setPinCommand(50, qtac::String("newcmd"));
    assert(cfg.getPinCommand(50) == "newcmd");

    // Group
    cfg.setPinGroup(50, eSwitchGroup);
    assert(cfg.getPinGroup(50) == eSwitchGroup);

    // Classic action
    cfg.setClassicAction(50, qtac::String("MY_ACTION"));
    assert(cfg.getClassicAction(50) == "MY_ACTION");

    // Tab name
    cfg.setTabName(50, qtac::String("Advanced"));
    assert(cfg.getTabName(50) == "Advanced");

    // Cell location
    cfg.setPinCellLocation(50, qtac::Point(2, 3));
    qtac::Point loc = cfg.getPinCellLocation(50);
    assert(loc.x() == 2 && loc.y() == 3);
}

static void test_psoc_invalid_pin()
{
    _PSOCPlatformConfiguration cfg;

    // Getters on non-existent pin return defaults
    assert(cfg.getPinEnableState(255) == true);   // default per getPinEnableState
    assert(!cfg.getPinInvertedState(255));
    assert(cfg.getPinLabel(255).isEmpty());
    assert(cfg.getPinGroup(255) == eUnknownCommandGroup);

    // Setters on non-existent pin throw
    bool threw = false;
    try { cfg.setPinLabel(255, qtac::String("x")); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
}

static void test_psoc_cascade_tab()
{
    _PSOCPlatformConfiguration cfg;

    // Set a custom tab on two pins
    cfg.setTabName(50, qtac::String("MyTab"));
    cfg.setTabName(34, qtac::String("MyTab"));
    cfg.setTabName(51, qtac::String("OtherTab"));

    // Delete MyTab — those pins should have empty tab name
    cfg.cascadeTabDelete(qtac::String("MyTab"));
    assert(cfg.getTabName(50).isEmpty());
    assert(cfg.getTabName(34).isEmpty());
    assert(cfg.getTabName(51) == "OtherTab");  // untouched

    // Rename
    cfg.setTabName(50, qtac::String("Alpha"));
    cfg.setTabName(34, qtac::String("Alpha"));
    cfg.cascadeTabRename(qtac::String("Alpha"), qtac::String("Beta"));
    assert(cfg.getTabName(50) == "Beta");
    assert(cfg.getTabName(34) == "Beta");
    assert(cfg.getTabName(51) == "OtherTab");
}

static void test_psoc_json_roundtrip()
{
    _PSOCPlatformConfiguration orig;

    // Modify a pin before serialising
    orig.setPinLabel(50, qtac::String("RoundtripKey"));
    orig.setPinEnableState(29, true);
    orig.setPinInitializationPriority(53, 7);

    json_t j;
    orig.write(j);

    _PSOCPlatformConfiguration restored;
    restored.read(j);

    assert(restored.getPinLabel(50) == "RoundtripKey");
    assert(restored.getPinEnableState(29));
    assert(restored.getPinInitializationPriority(53) == 7);
}

// ===========================================================================
// PIC32CX
// ===========================================================================

static void test_pic32cx_factory_defaults()
{
    _PIC32CXPlatformConfiguration cfg;

    // Battery pin (4) must be enabled
    assert(cfg.getPinEnableState(4));
    assert(cfg.getPinLabel(4) == "Battery");
    assert(cfg.getPinCommand(4) == "battery");
    assert(cfg.getPinGroup(4) == eConnectionGroup);

    // Pin 3 is disabled in factory defaults
    assert(!cfg.getPinEnableState(3));

    PIC32CXPinList active = cfg.getActivePins();
    assert(!active.isEmpty());

    PIC32CXPinList all = cfg.getAllPins();
    assert(all.size() >= active.size());
}

static void test_pic32cx_getters_setters()
{
    _PIC32CXPlatformConfiguration cfg;

    // Enable state
    cfg.setPinEnableState(PIC32CXPinData::makePIC32CXHash(3), true);
    assert(cfg.getPinEnableState(3));
    cfg.setPinEnableState(PIC32CXPinData::makePIC32CXHash(3), false);
    assert(!cfg.getPinEnableState(3));

    // Inverted
    cfg.setPinInvertedState(PIC32CXPinData::makePIC32CXHash(4), true);
    assert(cfg.getPinInvertedState(4));
    cfg.setPinInvertedState(PIC32CXPinData::makePIC32CXHash(4), false);

    // Label
    HashType h4 = PIC32CXPinData::makePIC32CXHash(4);
    cfg.setPinLabel(h4, qtac::String("NewBattery"));
    assert(cfg.getPinLabel(4) == "NewBattery");

    // Tooltip
    cfg.setPinTooltip(h4, qtac::String("my tip"));
    assert(cfg.getPinTooltip(4) == "my tip");

    // Command
    cfg.setPinCommand(h4, qtac::String("newbat"));
    assert(cfg.getPinCommand(4) == "newbat");

    // Group
    cfg.setPinGroup(h4, eSwitchGroup);
    assert(cfg.getPinGroup(4) == eSwitchGroup);

    // Tab name
    cfg.setTabName(h4, qtac::String("SpecialTab"));
    assert(cfg.getTabName(4) == "SpecialTab");

    // Cell location
    cfg.setPinCellLocation(h4, qtac::Point(1, 2));
    qtac::Point loc = cfg.getPinCellLocation(4);
    assert(loc.x() == 1 && loc.y() == 2);
}

static void test_pic32cx_invalid_pin()
{
    _PIC32CXPlatformConfiguration cfg;

    // Non-existent pin → defaults
    assert(cfg.getPinLabel(9999).isEmpty());
    assert(cfg.getPinGroup(9999) == eUnknownCommandGroup);

    // Setter on non-existent pin throws
    bool threw = false;
    try { cfg.setPinLabel(PIC32CXPinData::makePIC32CXHash(9999), qtac::String("x")); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
}

static void test_pic32cx_cascade_tab()
{
    _PIC32CXPlatformConfiguration cfg;

    HashType h4  = PIC32CXPinData::makePIC32CXHash(4);
    HashType h6  = PIC32CXPinData::makePIC32CXHash(6);
    HashType h7  = PIC32CXPinData::makePIC32CXHash(7);

    cfg.setTabName(h4, qtac::String("TabX"));
    cfg.setTabName(h6, qtac::String("TabX"));
    cfg.setTabName(h7, qtac::String("TabY"));

    cfg.cascadeTabDelete(qtac::String("TabX"));
    assert(cfg.getTabName(4).isEmpty());
    assert(cfg.getTabName(6).isEmpty());
    assert(cfg.getTabName(7) == "TabY");

    cfg.setTabName(h4, qtac::String("Old"));
    cfg.setTabName(h6, qtac::String("Old"));
    cfg.cascadeTabRename(qtac::String("Old"), qtac::String("New"));
    assert(cfg.getTabName(4) == "New");
    assert(cfg.getTabName(6) == "New");
    assert(cfg.getTabName(7) == "TabY");
}

static void test_pic32cx_json_roundtrip()
{
    _PIC32CXPlatformConfiguration orig;

    HashType h4 = PIC32CXPinData::makePIC32CXHash(4);
    orig.setPinLabel(h4, qtac::String("RoundtripBattery"));
    orig.setPinEnableState(PIC32CXPinData::makePIC32CXHash(3), true);

    json_t j;
    orig.write(j);

    _PIC32CXPlatformConfiguration restored;
    restored.read(j);

    assert(restored.getPinLabel(4) == "RoundtripBattery");
    assert(restored.getPinEnableState(3));
}

// ===========================================================================
// FTDI
// ===========================================================================

static void test_ftdi_init()
{
    _FTDIPlatformConfiguration cfg(1);
    assert(cfg.getChipCount() == 1);

    // Chip 0 has buses A and B only; all pins initialised
    FTDIPinList all = cfg.getAllPins();
    assert(!all.isEmpty());
    // chip0: 2 buses × 8 pins = 16
    assert(all.size() == 16);
}

static void test_ftdi_two_chips()
{
    _FTDIPlatformConfiguration cfg(2);
    assert(cfg.getChipCount() == 2);

    FTDIPinList all = cfg.getAllPins();
    // chip0: 2 buses × 8 = 16; chip1: 4 buses × 8 = 32 → 48 total
    assert(all.size() == 48);
}

static void test_ftdi_invalid_chip_count()
{
    bool threw = false;
    try { _FTDIPlatformConfiguration cfg(5); }
    catch (const std::invalid_argument&) { threw = true; }
    assert(threw);
}

static void test_ftdi_pin_data()
{
    _FTDIPlatformConfiguration cfg(1);

    // A pin that should exist
    FTDIPinData pd = cfg.getPinData(0, 'A', 0);
    assert(pd._chipIndex == 0);
    assert(pd._bus == 'A');
    assert(pd._chipPin == 0);
}

static void test_ftdi_getters_setters()
{
    _FTDIPlatformConfiguration cfg(1);

    HashType h = FTDIPinData::makeFTDIHash(0, 'A', 0);

    // Enable
    cfg.setPinEnableState(h, true);
    assert(cfg.getPinEnableState(0, 'A', 0));
    cfg.setPinEnableState(h, false);
    assert(!cfg.getPinEnableState(0, 'A', 0));

    // Input
    cfg.setPinInputState(h, true);
    assert(cfg.getPinInputState(0, 'A', 0));
    cfg.setPinInputState(h, false);
    assert(!cfg.getPinInputState(0, 'A', 0));

    // Initial value
    cfg.setInitialPinValue(h, true);
    assert(cfg.getInitialPinValue(0, 'A', 0));
    cfg.setInitialPinValue(h, false);

    // Priority
    cfg.setPinInitializationPriority(h, 3);
    assert(cfg.getPinInitializationPriority(0, 'A', 0) == 3);

    // Inverted
    cfg.setPinInvertedState(h, true);
    assert(cfg.getPinInvertedState(0, 'A', 0));
    cfg.setPinInvertedState(h, false);

    // Label
    cfg.setPinLabel(h, qtac::String("MyFTDIPin"));
    assert(cfg.getPinLabel(0, 'A', 0) == "MyFTDIPin");

    // Tooltip
    cfg.setPinTooltip(h, qtac::String("ftdi tip"));
    assert(cfg.getPinTooltip(0, 'A', 0) == "ftdi tip");

    // Command
    cfg.setPinCommand(h, qtac::String("ftdicmd"));
    assert(cfg.getPinCommand(0, 'A', 0) == "ftdicmd");

    // Group
    cfg.setPinGroup(h, eButtonGroup);
    assert(cfg.getPinGroup(0, 'A', 0) == eButtonGroup);

    // Tab name
    cfg.setTabName(h, qtac::String("FTDITab"));
    assert(cfg.getTabName(0, 'A', 0) == "FTDITab");

    // Cell location
    cfg.setPinCellLocation(h, qtac::Point(4, 5));
    qtac::Point loc = cfg.getPinCellLocation(0, 'A', 0);
    assert(loc.x() == 4 && loc.y() == 5);
}

static void test_ftdi_bus_function()
{
    _FTDIPlatformConfiguration cfg(1);

    // Default: bus A/B → VCP
    FTDIBusData bd = cfg.getBusFunction(0, 'A');
    assert(bd._busFunction == eBusFunctionVCP);

    // Set to D2XX via hash
    HashType bh = FTDIBusData::makeFTDIHash(0, 'A');
    cfg.setBusFunction(bh, eBusFunctionD2XX);
    FTDIBusData bd2 = cfg.getBusFunction(0, 'A');
    assert(bd2._busFunction == eBusFunctionD2XX);

    // Set back via (chipIndex, bus) overload
    cfg.setBusFunction(0, 'A', eBusFunctionVCP);
    assert(cfg.getBusFunction(0, 'A')._busFunction == eBusFunctionVCP);
}

static void test_ftdi_cascade_tab()
{
    _FTDIPlatformConfiguration cfg(1);

    HashType h0 = FTDIPinData::makeFTDIHash(0, 'A', 0);
    HashType h1 = FTDIPinData::makeFTDIHash(0, 'A', 1);
    HashType h2 = FTDIPinData::makeFTDIHash(0, 'A', 2);

    cfg.setTabName(h0, qtac::String("Alpha"));
    cfg.setTabName(h1, qtac::String("Alpha"));
    cfg.setTabName(h2, qtac::String("Beta"));

    cfg.cascadeTabDelete(qtac::String("Alpha"));
    assert(cfg.getTabName(0, 'A', 0).isEmpty());
    assert(cfg.getTabName(0, 'A', 1).isEmpty());
    assert(cfg.getTabName(0, 'A', 2) == "Beta");

    cfg.setTabName(h0, qtac::String("X"));
    cfg.setTabName(h1, qtac::String("X"));
    cfg.cascadeTabRename(qtac::String("X"), qtac::String("Y"));
    assert(cfg.getTabName(0, 'A', 0) == "Y");
    assert(cfg.getTabName(0, 'A', 1) == "Y");
    assert(cfg.getTabName(0, 'A', 2) == "Beta");
}

static void test_ftdi_json_roundtrip()
{
    _FTDIPlatformConfiguration orig(1);

    HashType h = FTDIPinData::makeFTDIHash(0, 'B', 3);
    orig.setPinLabel(h, qtac::String("RoundtripPin"));
    orig.setPinEnableState(h, true);
    orig.setPinCommand(h, qtac::String("testcmd"));

    json_t j;
    orig.write(j);

    _FTDIPlatformConfiguration restored(1);
    restored.read(j);

    assert(restored.getPinLabel(0, 'B', 3) == "RoundtripPin");
    assert(restored.getPinEnableState(0, 'B', 3));
    assert(restored.getPinCommand(0, 'B', 3) == "testcmd");
}

// ===========================================================================
// FTDIBusData / FTDIPinData static helpers
// ===========================================================================

static void test_bus_function_string_convert()
{
    assert(FTDIBusData::toString(eBusFunctionVCP)  == "VCP");
    assert(FTDIBusData::toString(eBusFunctionD2XX) == "D2XX");
    assert(FTDIBusData::toString(eBusFunctionI2C)  == "I2C");
    assert(FTDIBusData::toString(eBusFunctionUnknown).isEmpty());

    assert(FTDIBusData::fromString(qtac::String("VCP"))  == eBusFunctionVCP);
    assert(FTDIBusData::fromString(qtac::String("D2XX")) == eBusFunctionD2XX);
    assert(FTDIBusData::fromString(qtac::String("I2C"))  == eBusFunctionI2C);
    assert(FTDIBusData::fromString(qtac::String("vcp"))  == eBusFunctionVCP); // case-insensitive
    assert(FTDIBusData::fromString(qtac::String("???"))  == eBusFunctionUnknown);
}

int main()
{
    // PSOC
    test_psoc_factory_defaults();
    test_psoc_getters_setters();
    test_psoc_invalid_pin();
    test_psoc_cascade_tab();
    test_psoc_json_roundtrip();

    // PIC32CX
    test_pic32cx_factory_defaults();
    test_pic32cx_getters_setters();
    test_pic32cx_invalid_pin();
    test_pic32cx_cascade_tab();
    test_pic32cx_json_roundtrip();

    // FTDI
    test_ftdi_init();
    test_ftdi_two_chips();
    test_ftdi_invalid_chip_count();
    test_ftdi_pin_data();
    test_ftdi_getters_setters();
    test_ftdi_bus_function();
    test_ftdi_cascade_tab();
    test_ftdi_json_roundtrip();
    test_bus_function_string_convert();

    std::printf("All platform configuration tests passed!\n");
    return 0;
}
