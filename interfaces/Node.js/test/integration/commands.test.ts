// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

import QTAC, { TACDevice } from '../../lib/index';

/**
 * Integration tests for dynamic commands
 * Requires actual hardware
 */
describe('Commands Integration Tests', () => {
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

    it('should get command count', () => {
        if (!device) return;

        const count = device.getCommandCount();
        expect(typeof count).toBe('number');
        expect(count).toBeGreaterThanOrEqual(0);
    });

    it('should get command metadata', () => {
        if (!device) return;

        const count = device.getCommandCount();
        if (count === 0) {
            console.warn('Device has no commands');
            return;
        }

        const cmd = device.getCommand(0);
        expect(cmd).toBeDefined();
        expect(cmd).toHaveProperty('command');
        expect(cmd).toHaveProperty('helpText');
        expect(cmd).toHaveProperty('pin');
        expect(cmd).toHaveProperty('tabName');
        expect(cmd).toHaveProperty('groupName');
        expect(cmd).toHaveProperty('cellLocation');
    });

    it('should get all commands', () => {
        if (!device) return;

        const commands = device.getAllCommands();
        expect(Array.isArray(commands)).toBe(true);

        const count = device.getCommandCount();
        expect(commands.length).toBe(count);
    });

    it('should get command state', () => {
        if (!device) return;

        const count = device.getCommandCount();
        if (count === 0) return;

        const cmd = device.getCommand(0);
        const state = device.getCommandState(cmd.command);
        expect(typeof state).toBe('boolean');
    });

    it('should get quick command count', () => {
        if (!device) return;

        const count = device.getQuickCommandCount();
        expect(typeof count).toBe('number');
        expect(count).toBeGreaterThanOrEqual(0);
    });

    it('should get quick command names', () => {
        if (!device) return;

        const count = device.getQuickCommandCount();
        if (count === 0) return;

        const qcmd = device.getQuickCommand(0);
        expect(typeof qcmd).toBe('string');
        expect(qcmd.length).toBeGreaterThan(0);
    });
});
