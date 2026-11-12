// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

const QTAC = require('./qtac-native');

console.log('=== QTAC Device Control Example ===\n');

async function main() {
    try {
        // List devices
        const devices = QTAC.listDevices();

        if (devices.length === 0) {
            console.log('No devices found. Please connect a TAC device.');
            process.exit(0);
        }

        console.log(`Opening device: ${devices[0].description} (${devices[0].port})\n`);

        // Open the first device
        const device = QTAC.openDevice(devices[0].port);

        try {
            // Get device information
            console.log('Device Information:');
            console.log('  Name:            ', device.getName());
            console.log('  Firmware:        ', device.getFirmwareVersion());
            console.log('  Hardware:        ', device.getHardware());
            console.log('  Hardware Version:', device.getHardwareVersion());
            console.log('  UUID:            ', device.getUUID());
            console.log('  Reset Count:     ', device.getResetCount());
            console.log();

            // Battery control example
            console.log('Testing battery control...');
            console.log('  Current battery state:', device.getBatteryState());
            device.setBatteryState(true);
            console.log('  Set battery to ON');
            console.log('  New battery state:    ', device.getBatteryState());
            console.log();

            // USB control example
            console.log('Testing USB control...');
            console.log('  USB0 state:', device.getUsb0State());
            console.log('  USB1 state:', device.getUsb1State());
            console.log();

            // Button state example
            console.log('Testing button states...');
            console.log('  PowerKey state:  ', device.getPowerKeyState());
            console.log('  VolumeUp state:  ', device.getVolumeUpState());
            console.log('  VolumeDown state:', device.getVolumeDownState());
            console.log();

            // List available commands
            console.log('Available Commands:');
            const commandCount = device.getCommandCount();
            console.log(`  Total commands: ${commandCount}`);

            if (commandCount > 0) {
                // Show first few commands
                const displayCount = Math.min(5, commandCount);
                console.log(`  First ${displayCount} commands:`);

                for (let i = 0; i < displayCount; i++) {
                    const cmd = device.getCommand(i);
                    console.log(`    ${i + 1}. ${cmd.command} - ${cmd.helpText}`);
                }

                if (commandCount > displayCount) {
                    console.log(`    ... and ${commandCount - displayCount} more`);
                }
            }
            console.log();

            // Quick commands example
            console.log('Quick Commands:');
            const quickCount = device.getQuickCommandCount();
            console.log(`  Total quick commands: ${quickCount}`);

            if (quickCount > 0) {
                for (let i = 0; i < quickCount; i++) {
                    const qcmd = device.getQuickCommand(i);
                    console.log(`    ${i + 1}. ${qcmd}`);
                }
            }
            console.log();

            console.log('To execute quick commands:');
            console.log('  device.powerOn()         - Power on the device');
            console.log('  device.powerOff()        - Power off the device');
            console.log('  device.bootToFastboot()  - Boot to fastboot mode');
            console.log('  device.bootToEDL()       - Boot to EDL mode');
            console.log();

        } finally {
            // Always close the device
            device.close();
            console.log('Device closed.');
        }

    } catch (error) {
        console.error('Error:', error.message);
        process.exit(1);
    }
}

main();
