// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

export type TAC_RESULT = number;
export type TAC_HANDLE = number;
export type TAC_ERROR = number;

/**
 * Device information returned from enumeration
 */
export interface DeviceInfo {
    /** Port name (e.g., "COM3" on Windows, "/dev/ttyUSB0" on Linux) */
    port: string;
    /** Device description */
    description: string;
    /** Device serial number */
    serialNumber: string;
    /** Device index */
    index: number;
}

/**
 * Command metadata
 */
export interface CommandMetadata {
    /** Command name */
    command: string;
    /** Help text describing the command */
    helpText: string;
    /** Pin number associated with command (-1 if not applicable) */
    pin: number;
    /** Tab name in UI */
    tabName: string;
    /** Group name in UI */
    groupName: string;
    /** Cell location in UI */
    cellLocation: string;
}

/**
 * TAC error codes
 */
export enum TACError {
    NO_ERROR = 0,
    BUFFER_TOO_SMALL = 1,
    BAD_HANDLE = 2,
    COMMAND_NOT_FOUND = 3,
    BAD_INDEX = 4,
    INIT_FAILED = 5,
    VARIABLE_NOT_FOUND = 6
}
