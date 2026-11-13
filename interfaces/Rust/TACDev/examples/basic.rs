// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

//! Basic usage example — mirrors the Python TACDev.py main() function.

use tacdev::{TACDev, TACError};

fn main() -> Result<(), TACError> {
    let tac = TACDev::load()?;

    println!("QTAC version : {}", tac.alpaca_version()?);
    println!("TAC version  : {}", tac.tac_version()?);

    let count = tac.device_count()?;
    println!("Device count : {count}");

    for i in 0..count {
        let dev = tac.get_device(i)?;
        println!(
            "  [{i}] port={} serial={}",
            dev.port_name(),
            dev.serial_number()
        );
    }

    if count > 0 {
        let mut dev = tac.get_device(0)?;
        dev.open()?;

        println!("\nOpened: {}", dev.name()?);
        println!("  Firmware : {}", dev.firmware_version()?);
        println!("  Hardware : {}", dev.hardware()?);
        println!("  UUID     : {}", dev.uuid()?);
        println!("  Battery  : {}", dev.battery_state()?);

        dev.set_battery_state(true)?;
        println!("  Battery set -> true");

        dev.set_battery_state(false)?;
        println!("  Battery set -> false");

        let cmd_count = dev.command_count()?;
        println!("  Commands : {cmd_count}");
        if cmd_count > 1 {
            println!("  Command[1] : {}", dev.command(1)?);
        }

        let quick_count = dev.quick_command_count()?;
        println!("  Quick commands : {quick_count}");

        let var_count = dev.script_variable_count()?;
        println!("  Script variables : {var_count}");

        // dev.close() is called automatically on drop, but explicit is fine too
        dev.close()?;
    }

    Ok(())
}
