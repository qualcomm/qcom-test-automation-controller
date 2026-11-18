// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

import QTAC from '../../lib/index';

describe('Device Enumeration', () => {
    it('should get device count', () => {
        const count = QTAC.getDeviceCount();
        expect(typeof count).toBe('number');
        expect(count).toBeGreaterThanOrEqual(0);
    });

    it('should list devices', () => {
        const devices = QTAC.listDevices();
        expect(Array.isArray(devices)).toBe(true);

        const count = QTAC.getDeviceCount();
        expect(devices.length).toBeLessThanOrEqual(count);
    });

    it('should return valid device info structure', () => {
        const devices = QTAC.listDevices();

        if (devices.length > 0) {
            const device = devices[0];

            expect(device).toHaveProperty('port');
            expect(device).toHaveProperty('description');
            expect(device).toHaveProperty('serialNumber');
            expect(device).toHaveProperty('index');

            expect(typeof device.port).toBe('string');
            expect(typeof device.description).toBe('string');
            expect(typeof device.serialNumber).toBe('string');
            expect(typeof device.index).toBe('number');
        }
    });

    it('should get device info by index', () => {
        const count = QTAC.getDeviceCount();

        if (count > 0) {
            const device = QTAC.getDeviceInfo(0);

            expect(device).toBeDefined();
            expect(device.port).toBeDefined();
            expect(device.description).toBeDefined();
        }
    });

    it('should throw error for invalid device index', () => {
        expect(() => {
            QTAC.getDeviceInfo(9999);
        }).toThrow();
    });
});
