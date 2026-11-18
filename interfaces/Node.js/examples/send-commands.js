// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

const QTAC = require('./qtac-native');

console.log('=== QTAC Send Commands Example ===\n');

async function main() {
    try {
        // List and open device
        const devices = QTAC.listDevices();

        if (devices.length === 0) {
            console.log('No devices found. Please connect a TAC device.');
            process.exit(0);
        }

        console.log(`Opening device: ${devices[0].description}\n`);
        const device = QTAC.openDevice(devices[0].port);

        try {
            // Get all available commands
            const commands = device.getAllCommands();

            console.log(`Device has ${commands.length} available commands:\n`);

            // Display commands grouped by tab
            const commandsByTab = {};

            commands.forEach(cmd => {
                if (!commandsByTab[cmd.tabName]) {
                    commandsByTab[cmd.tabName] = [];
                }
                commandsByTab[cmd.tabName].push(cmd);
            });

            Object.keys(commandsByTab).forEach(tabName => {
                console.log(`[${tabName}]`);
                commandsByTab[tabName].forEach(cmd => {
                    const state = device.getCommandState(cmd.command);
                    const stateStr = state ? 'ON ' : 'OFF';
                    console.log(`  ${stateStr} - ${cmd.command}`);
                    if (cmd.helpText) {
                        console.log(`       ${cmd.helpText}`);
                    }
                });
                console.log();
            });

            // Example: Send a command
            if (commands.length > 0) {
                const exampleCmd = commands[0].command;
                console.log(`Example - Querying command state: "${exampleCmd}"`);
                const currentState = device.getCommandState(exampleCmd);
                console.log(`  Current state: ${currentState ? 'ON' : 'OFF'}`);
                console.log();

                console.log('To send a command:');
                console.log(`  device.sendCommand('${exampleCmd}', true);  // Turn ON`);
                console.log(`  device.sendCommand('${exampleCmd}', false); // Turn OFF`);
                console.log();
            }

            // Script variables
            const varCount = device.getScriptVariableCount();
            if (varCount > 0) {
                console.log(`Script Variables (${varCount} total):`);
                for (let i = 0; i < Math.min(5, varCount); i++) {
                    const variable = device.getScriptVariable(i);
                    console.log(`  ${i + 1}. ${variable}`);
                }
                if (varCount > 5) {
                    console.log(`  ... and ${varCount - 5} more`);
                }
                console.log();

                console.log('To update a script variable:');
                console.log('  device.updateScriptVariable("variableName", "newValue");');
                console.log();
            }

        } finally {
            device.close();
            console.log('Device closed.');
        }

    } catch (error) {
        console.error('Error:', error.message);
        if (error.stack) {
            console.error(error.stack);
        }
        process.exit(1);
    }
}

main();
