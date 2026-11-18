// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

import QTAC from '../../lib/index';

describe('QTAC Initialization', () => {
    it('should get QTAC version', () => {
        const version = QTAC.getVersion();
        expect(version).toBeDefined();
        expect(typeof version).toBe('string');
        expect(version.length).toBeGreaterThan(0);
    });

    it('should get TAC version', () => {
        const version = QTAC.getTACVersion();
        expect(version).toBeDefined();
        expect(typeof version).toBe('string');
        expect(version.length).toBeGreaterThan(0);
    });

    it('should get last error', () => {
        const error = QTAC.getLastError();
        expect(error).toBeDefined();
        expect(typeof error).toBe('string');
    });

    it('should get logging state', () => {
        const state = QTAC.getLoggingState();
        expect(typeof state).toBe('boolean');
    });

    it('should set logging state', () => {
        const originalState = QTAC.getLoggingState();

        // Toggle logging
        QTAC.setLogging(!originalState);
        expect(QTAC.getLoggingState()).toBe(!originalState);

        // Restore original state
        QTAC.setLogging(originalState);
        expect(QTAC.getLoggingState()).toBe(originalState);
    });
});
