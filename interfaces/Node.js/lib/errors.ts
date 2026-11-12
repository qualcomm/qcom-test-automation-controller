// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

import { TAC_ERROR, TACError } from './types';

/**
 * Custom error class for TAC errors
 */
export class TACDevError extends Error {
    public code: TAC_ERROR;

    constructor(message: string, code: TAC_ERROR = TACError.NO_ERROR) {
        super(message);
        this.name = 'TACDevError';
        this.code = code;
        Object.setPrototypeOf(this, TACDevError.prototype);
    }
}

/**
 * Error for bad device handles
 */
export class TACDevHandleError extends TACDevError {
    constructor(message: string = 'Invalid or closed device handle') {
        super(message, TACError.BAD_HANDLE);
        this.name = 'TACDevHandleError';
        Object.setPrototypeOf(this, TACDevHandleError.prototype);
    }
}

/**
 * Error for initialization failures
 */
export class TACDevInitError extends TACDevError {
    constructor(message: string = 'Failed to initialize TACDev') {
        super(message, TACError.INIT_FAILED);
        this.name = 'TACDevInitError';
        Object.setPrototypeOf(this, TACDevInitError.prototype);
    }
}

/**
 * Error for command not found
 */
export class TACDevCommandError extends TACDevError {
    constructor(message: string = 'Command not found') {
        super(message, TACError.COMMAND_NOT_FOUND);
        this.name = 'TACDevCommandError';
        Object.setPrototypeOf(this, TACDevCommandError.prototype);
    }
}
