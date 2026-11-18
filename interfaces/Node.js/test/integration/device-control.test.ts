// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

import QTAC, { TACDevice } from '../../lib/index';

/**
 * Integration tests that require actual hardware
 * These tests will be skipped if no devices are connected
 */
describe('Device Control Integration Tests', () => {
    let device: TACDevice | null = null;

    beforeAll(() => {
        const devices = QTAC.listDevices();

        if (devices.length === 0) {
            console.warn('No TAC devices found - skipping integration tests');
            return;
        }

        device = QTAC.openDevice(devices[0].port);
    });

    afterAll(() => {
        if (device) {
            device.close();
        }
    });

    it('should get device name', () => {
        if (!device) return;

        const name = device.getName();
        expect(name).toBeDefined();
        expect(typeof name).toBe('string');
        expect(name.length).toBeGreaterThan(0);
    });

    it('should get firmware version', () => {
        if (!device) return;

        const fw = device.getFirmwareVersion();
        expect(fw).toBeDefined();
        expect(typeof fw).toBe('string');
    });

    it('should get hardware type', () => {
        if (!device) return;

        const hw = device.getHardware();
        expect(hw).toBeDefined();
        expect(typeof hw).toBe('string');
    });

    it('should get device UUID', () => {
        if (!device) return;

        const uuid = device.getUUID();
        expect(uuid).toBeDefined();
        expect(typeof uuid).toBe('string');
    });

    it('should control battery state', () => {
        if (!device) return;

        // Get initial state
        const initialState = device.getBatteryState();
        expect(typeof initialState).toBe('boolean');

        // Toggle battery state
        device.setBatteryState(!initialState);
        const newState = device.getBatteryState();
        expect(newState).toBe(!initialState);

        // Restore original state
        device.setBatteryState(initialState);
        const restoredState = device.getBatteryState();
        expect(restoredState).toBe(initialState);
    });

    it('should get USB states', () => {
        if (!device) return;

        const usb0 = device.getUsb0State();
        const usb1 = device.getUsb1State();

        expect(typeof usb0).toBe('boolean');
        expect(typeof usb1).toBe('boolean');
    });

    it('should get button states', () => {
        if (!device) return;

        const powerKey = device.getPowerKeyState();
        const volumeUp = device.getVolumeUpState();
        const volumeDown = device.getVolumeDownState();

        expect(typeof powerKey).toBe('boolean');
        expect(typeof volumeUp).toBe('boolean');
        expect(typeof volumeDown).toBe('boolean');
    });

    it('should get reset count', () => {
        if (!device) return;

        const resetCount = device.getResetCount();
        expect(typeof resetCount).toBe('number');
        expect(resetCount).toBeGreaterThanOrEqual(0);
    });
});
