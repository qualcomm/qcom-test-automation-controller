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

#include <qtac/STM32PlatformConfiguration.h>
#include <qtac/PSOCPlatformConfiguration.h>
#include <qtac/Notification.h>

#include <cassert>
#undef assert
#define assert(expr) \
    do { if (!(expr)) { \
        std::fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", \
                     #expr, __FILE__, __LINE__); \
        std::exit(1); \
    } } while(0)

#include <cstdio>
#include <thread>
#include <chrono>

// ===========================================================================
// STM32PlatformConfiguration
// ===========================================================================

static void test_stm32_pin_count()
{
    _STM32PlatformConfiguration cfg;

    // BugHopper V2 has exactly 3 pins (EDL/0, Battery/2, VolumeDown/3),
    // all enabled.
    STM32PinList all    = cfg.getAllPins();
    STM32PinList active = cfg.getActivePins();
    assert(all.size()    == 3);
    assert(active.size() == 3);
}

static void test_stm32_pin_data_edl()
{
    _STM32PlatformConfiguration cfg;

    // Pin 0 = EDL: enabled, not inverted, command "edl", group eSwitchGroup
    assert(cfg.getPinEnableState(0)    == true);
    assert(cfg.getPinInvertedState(0)  == false);
    assert(cfg.getPinLabel(0)          == "EDL");
    assert(cfg.getPinCommand(0)        == "edl");
    assert(cfg.getTabName(0)           == "General");
    assert(cfg.getPinGroup(0)          == eSwitchGroup);
}

static void test_stm32_pin_data_battery()
{
    _STM32PlatformConfiguration cfg;

    // Pin 2 = Battery: enabled, INVERTED (physical off = battery on),
    // group eConnectionGroup
    assert(cfg.getPinEnableState(2)    == true);
    assert(cfg.getPinInvertedState(2)  == true);
    assert(cfg.getPinLabel(2)          == "Battery");
    assert(cfg.getPinCommand(2)        == "battery");
    assert(cfg.getPinGroup(2)          == eConnectionGroup);
}

static void test_stm32_pin_data_voldn()
{
    _STM32PlatformConfiguration cfg;

    // Pin 3 = Volume Down: enabled, not inverted, group eSwitchGroup
    assert(cfg.getPinEnableState(3)    == true);
    assert(cfg.getPinInvertedState(3)  == false);
    assert(cfg.getPinLabel(3)          == "Volume Down");
    assert(cfg.getPinCommand(3)        == "voldn");
    assert(cfg.getPinGroup(3)          == eSwitchGroup);
}

static void test_stm32_pin_not_found()
{
    _STM32PlatformConfiguration cfg;

    // Pin 99 does not exist — accessors return sensible defaults.
    assert(cfg.getPinEnableState(99)   == true);   // default
    assert(cfg.getPinInvertedState(99) == false);  // default
    assert(cfg.getPinLabel(99).isEmpty());
    assert(cfg.getPinCommand(99).isEmpty());
    assert(cfg.getPinCellLocation(99)  == qtac::Point(-1, -1));
}

static void test_stm32_buttons()
{
    _STM32PlatformConfiguration cfg;

    // Exactly 5 buttons: Power On, Power Off, Reset, Boot to EDL,
    // Force USB-C Host Mode.
    const auto& buttons = cfg.getButtons();
    assert(buttons.size() == 5);

    // Spot-check the first and last by name.
    assert(buttons[0]._name    == "Power On");
    assert(buttons[0]._command == "powerOn");
    assert(buttons[4]._name    == "Force USB-C Host Mode");
    assert(buttons[4]._command == "forceUsbcHostMode");
}

static void test_stm32_variables()
{
    _STM32PlatformConfiguration cfg;

    const auto& vars = cfg.getVariables();
    assert(vars.size() == 1);

    auto it = vars.find("edl");
    assert(it != vars.end());
    assert(it->second._label       == "EDL timing (ms)");
    assert(it->second._type        == qtac::VariableType::Integer);
    assert(it->second._defaultValue.toUInt() == 100u);
}

static void test_stm32_script_has_expected_commands()
{
    _STM32PlatformConfiguration cfg;

    const qtac::AlpacaScript& script = cfg.getScript();
    assert(script.hasCommand("powerOn"));
    assert(script.hasCommand("powerOff"));
    assert(script.hasCommand("reset"));
    assert(script.hasCommand("bootToEDL"));
    assert(script.hasCommand("forceUsbcHostMode"));
}

static void test_stm32_getPins_returns_sorted_entries()
{
    _STM32PlatformConfiguration cfg;

    Pins pins = cfg.getPins();
    assert(pins.size() == 3);

    // Verify the PinEntry fields are populated for pin 2 (Battery).
    bool foundBattery = false;
    for (const auto& pe : pins)
    {
        if (pe._pinCommand == "battery")
        {
            assert(pe._inverted == true);
            assert(pe._pinLabel == "Battery");
            foundBattery = true;
        }
    }
    assert(foundBattery);
}

static void test_stm32_reinitialize()
{
    // Calling initialize() a second time must produce the same result
    // (idempotent — clears and repopulates).
    _STM32PlatformConfiguration cfg;
    cfg.initialize();

    assert(cfg.getAllPins().size()    == 3);
    assert(cfg.getButtons().size()   == 5);
    assert(cfg.getVariables().size() == 1);
}

// ===========================================================================
// PSOCPlatformConfiguration — I2C variant (ePSOCGPIOIIC)
// ===========================================================================

static void test_psoc_iic_variant_is_set()
{
    _PSOCPlatformConfiguration cfg(ePSOCGPIOIIC);
    assert(cfg.variant() == ePSOCGPIOIIC);
}

static void test_psoc_gpio_variant_default()
{
    // Default constructor (and explicit ePSOCGPIO) must not populate I2C entries.
    _PSOCPlatformConfiguration cfg;
    assert(cfg.variant() == ePSOCGPIO);
    assert(cfg.getI2CEntries().isEmpty());
    assert(cfg.getSlaveConfigs().isEmpty());
}

static void test_psoc_iic_slave_configs_populated()
{
    _PSOCPlatformConfiguration cfg(ePSOCGPIOIIC);

    // Should have exactly 2 slave configs: KTS1622EUAATR (0x20) and TCA9534APWR (0x38).
    PSOCI2CSlaves slaves = cfg.getSlaveConfigs();
    assert(slaves.size() == 2);

    bool foundKTS = false, foundTCA = false;
    for (const auto& s : slaves)
    {
        if (s._variant == eKTS1622EUAATR)
        {
            assert(s._slaveAddress == 0x20);
            assert(s._portCount    == 2);
            foundKTS = true;
        }
        if (s._variant == eTCA9534APWR)
        {
            assert(s._slaveAddress == 0x38);
            assert(s._portCount    == 1);
            foundTCA = true;
        }
    }
    assert(foundKTS);
    assert(foundTCA);
}

static void test_psoc_iic_entries_populated()
{
    _PSOCPlatformConfiguration cfg(ePSOCGPIOIIC);

    // KTS1622EUAATR has 2 ports × 8 pins = 16 entries.
    // TCA9534APWR has 1 port × 8 pins = 8 entries.
    // Total = 24 I2C entries.
    PSOCI2CEntries entries = cfg.getI2CEntries();
    assert(entries.size() == 24);
}

static void test_psoc_iic_active_entries_all_enabled()
{
    _PSOCPlatformConfiguration cfg(ePSOCGPIOIIC);

    // 24 total entries; 6 are disabled by default (reserved/unused pins).
    // Active (enabled) = 18.
    PSOCI2CEntries active = cfg.getActiveI2CEntries();
    assert(active.size() == 18);
}

static void test_psoc_iic_entries_for_kts_slave()
{
    _PSOCPlatformConfiguration cfg(ePSOCGPIOIIC);

    PSOCI2CSlaves slaves = cfg.getSlaveConfigs();
    PSOCI2CSlave kts;
    for (const auto& s : slaves)
        if (s._variant == eKTS1622EUAATR) { kts = s; break; }

    // KTS1622EUAATR: portCount=2, so 16 entries capped at 16.
    auto entries = cfg.getI2CEntriesForSlave(kts);
    assert(entries.size() == 16);
}

static void test_psoc_iic_slave_tab_name()
{
    _PSOCPlatformConfiguration cfg(ePSOCGPIOIIC);

    PSOCI2CSlaves slaves = cfg.getSlaveConfigs();
    PSOCI2CSlave kts, tca;
    for (const auto& s : slaves)
    {
        if (s._variant == eKTS1622EUAATR) kts = s;
        if (s._variant == eTCA9534APWR)   tca = s;
    }

    // Tab name format: "<variant name> (0x<hex address>)"
    qtac::String ktsTab = _PSOCPlatformConfiguration::i2cSlaveTabName(kts);
    qtac::String tcaTab = _PSOCPlatformConfiguration::i2cSlaveTabName(tca);

    assert(ktsTab.contains("KTS1622EUAATR"));
    assert(ktsTab.contains("0x20"));
    assert(tcaTab.contains("TCA9534APWR"));
    assert(tcaTab.contains("0x38"));
}

static void test_psoc_iic_add_remove_slave()
{
    _PSOCPlatformConfiguration cfg(ePSOCGPIOIIC);
    int initialCount = cfg.getSlaveConfigs().size(); // 2

    PSOCI2CSlave extra;
    extra._variant      = eTCA9534APWR;
    extra._slaveAddress = 0x39;
    extra._portCount    = 1;
    cfg.addSlaveConfig(extra);
    assert(cfg.getSlaveConfigs().size() == initialCount + 1);

    cfg.removeSlaveConfig(0x39);
    assert(cfg.getSlaveConfigs().size() == initialCount);
}

// ===========================================================================
// Notification
// ===========================================================================

static void test_notification_construction()
{
    qtac::Notification n(qtac::String("disk full"), qtac::NotificationLevel::Warn, 42u);

    assert(n.getMessage()         == "disk full");
    assert(n.getLevel()           == qtac::NotificationLevel::Warn);
    assert(n.getId()              == 42u);
    assert(n.getOccurrenceCount() == 1u);
}

static void test_notification_default_id()
{
    qtac::Notification n(qtac::String("ok"), qtac::NotificationLevel::Info);
    assert(n.getId() == 0u);
}

static void test_notification_add_occurrence()
{
    qtac::Notification n(qtac::String("retry"), qtac::NotificationLevel::Warn, 7u);
    assert(n.getOccurrenceCount() == 1u);

    auto t0 = n.getTimestamp();

    // Sleep briefly so the clock advances before addOccurrence().
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    n.addOccurrence();
    assert(n.getOccurrenceCount() == 2u);
    assert(n.getTimestamp() >= t0);

    n.addOccurrence();
    assert(n.getOccurrenceCount() == 3u);
}

static void test_notification_timestamp_is_recent()
{
    auto before = qtac::Notification::Clock::now();
    qtac::Notification n(qtac::String("event"), qtac::NotificationLevel::Error, 0u);
    auto after  = qtac::Notification::Clock::now();

    assert(n.getTimestamp() >= before);
    assert(n.getTimestamp() <= after);
}

static void test_notification_levels()
{
    qtac::Notification info(qtac::String("i"),  qtac::NotificationLevel::Info);
    qtac::Notification warn(qtac::String("w"),  qtac::NotificationLevel::Warn);
    qtac::Notification err (qtac::String("e"),  qtac::NotificationLevel::Error);

    assert(info.getLevel() == qtac::NotificationLevel::Info);
    assert(warn.getLevel() == qtac::NotificationLevel::Warn);
    assert(err.getLevel()  == qtac::NotificationLevel::Error);
}

// ===========================================================================
// main
// ===========================================================================

int main()
{
    // STM32PlatformConfiguration
    test_stm32_pin_count();
    test_stm32_pin_data_edl();
    test_stm32_pin_data_battery();
    test_stm32_pin_data_voldn();
    test_stm32_pin_not_found();
    test_stm32_buttons();
    test_stm32_variables();
    test_stm32_script_has_expected_commands();
    test_stm32_getPins_returns_sorted_entries();
    test_stm32_reinitialize();

    // PSOCPlatformConfiguration — I2C variant
    test_psoc_iic_variant_is_set();
    test_psoc_gpio_variant_default();
    test_psoc_iic_slave_configs_populated();
    test_psoc_iic_entries_populated();
    test_psoc_iic_active_entries_all_enabled();
    test_psoc_iic_entries_for_kts_slave();
    test_psoc_iic_slave_tab_name();
    test_psoc_iic_add_remove_slave();

    // Notification
    test_notification_construction();
    test_notification_default_id();
    test_notification_add_occurrence();
    test_notification_timestamp_is_recent();
    test_notification_levels();

    std::printf("All platform config tests passed!\n");
    return 0;
}
