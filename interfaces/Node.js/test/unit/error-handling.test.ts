// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

import QTAC from '../../lib/index';

describe('Error Handling', () => {
    it('should throw error when opening invalid device', () => {
        expect(() => {
            QTAC.openDevice('INVALID_PORT_NAME_12345');
        }).toThrow();
    });

    it('should throw error when accessing closed device', () => {
        const devices = QTAC.listDevices();

        if (devices.length > 0) {
            const device = QTAC.openDevice(devices[0].port);
            device.close();

            // Accessing closed device should throw
            expect(() => {
                device.getName();
            }).toThrow();
        }
    });

    it('should handle invalid command names gracefully', () => {
        const devices = QTAC.listDevices();

        if (devices.length > 0) {
            const device = QTAC.openDevice(devices[0].port);

            try {
                expect(() => {
                    device.getCommandState('INVALID_COMMAND_THAT_DOES_NOT_EXIST');
                }).toThrow();
            } finally {
                device.close();
            }
        }
    });
});
