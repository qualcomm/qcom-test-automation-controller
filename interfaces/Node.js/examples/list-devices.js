// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

const QTAC = require('./qtac-native');

console.log('=== QTAC Device Enumeration Example ===\n');

try {
    // Get library versions
    console.log('QTAC Version:', QTAC.getVersion());
    console.log('TAC Version:', QTAC.getTACVersion());
    console.log();

    // Get device count
    const deviceCount = QTAC.getDeviceCount();
    console.log(`Found ${deviceCount} device(s):\n`);

    if (deviceCount === 0) {
        console.log('No devices connected. Please connect a TAC device and try again.');
        process.exit(0);
    }

    // List all devices
    const devices = QTAC.listDevices();

    devices.forEach((device, index) => {
        console.log(`Device ${index + 1}:`);
        console.log(`  Port:         ${device.port}`);
        console.log(`  Description:  ${device.description}`);
        console.log(`  Serial:       ${device.serialNumber}`);
        console.log(`  Index:        ${device.index}`);
        console.log();
    });

} catch (error) {
    console.error('Error:', error.message);
    process.exit(1);
}
