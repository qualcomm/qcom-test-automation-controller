// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

import QTAC, { TACDevice } from '../../lib/index';

/**
 * Integration tests for quick commands
 * Requires actual hardware
 *
 * WARNING: These tests execute actual device commands.
 * Only run with devices that are safe to power cycle.
 */
describe('Quick Commands Integration Tests', () => {
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

    // Note: We don't actually execute power commands in automated tests
    // as they would reset the connected device. These tests just verify
    // the methods exist and are callable.

    it('should have powerOn method', () => {
        if (!device) return;
        expect(typeof device.powerOn).toBe('function');
    });

    it('should have powerOff method', () => {
        if (!device) return;
        expect(typeof device.powerOff).toBe('function');
    });

    it('should have bootToFastboot method', () => {
        if (!device) return;
        expect(typeof device.bootToFastboot).toBe('function');
    });

    it('should have bootToEDL method', () => {
        if (!device) return;
        expect(typeof device.bootToEDL).toBe('function');
    });

    it('should have bootToUEFI method', () => {
        if (!device) return;
        expect(typeof device.bootToUEFI).toBe('function');
    });

    it('should have bootToSecondaryEDL method', () => {
        if (!device) return;
        expect(typeof device.bootToSecondaryEDL).toBe('function');
    });

    // Script variables
    it('should get script variable count', () => {
        if (!device) return;

        const count = device.getScriptVariableCount();
        expect(typeof count).toBe('number');
        expect(count).toBeGreaterThanOrEqual(0);
    });

    it('should get script variables', () => {
        if (!device) return;

        const count = device.getScriptVariableCount();
        if (count === 0) return;

        const variable = device.getScriptVariable(0);
        expect(typeof variable).toBe('string');
    });

    // Advanced features
    it('should get help text', () => {
        if (!device) return;

        const helpText = device.getHelpText();
        expect(typeof helpText).toBe('string');
    });
});
