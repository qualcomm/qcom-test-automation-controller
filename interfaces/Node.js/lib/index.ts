// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

import bindings from 'bindings';
import path from 'path';
import { TACDevice } from './device';
import { DeviceInfo } from './types';

// `qtac.node` is linked against TACDev and Qt DLLs on Windows. The main
// project build adds those directories to PATH only for its own process, so
// add them before loading the addon in consumers such as Jest as well.
if (process.platform === 'win32') {
    const runtimeArchitecture = process.arch === 'arm64' ? 'ARM64' : 'x64';
    const runtimeBin = path.resolve(__dirname, '../../../__Builds', runtimeArchitecture, 'Release', 'bin');
    const dependencyPaths = [runtimeBin, process.env.QTBIN, process.env.PATH].filter(
        (value): value is string => Boolean(value)
    );

    process.env.PATH = dependencyPaths.join(path.delimiter);
}

// Load the native NAPI module
const native = bindings('qtac');

// Initialize TACDev library on module load
native.TACDevice.InitializeTACDev();

/**
 * QTAC - Main class for interacting with Qualcomm Test Automation Controller
 */
export class QTAC {
    /**
     * Get QTAC (Alpaca) version
     */
    static getVersion(): string {
        return native.TACDevice.GetAlpacaVersion();
    }

    /**
     * Get TAC version
     */
    static getTACVersion(): string {
        return native.TACDevice.GetTACVersion();
    }

    /**
     * Get last error message from TACDev library
     */
    static getLastError(): string {
        return native.TACDevice.GetLastTACError();
    }

    /**
     * Get number of connected devices
     */
    static getDeviceCount(): number {
        return native.TACDevice.GetDeviceCount();
    }

    /**
     * Get device information by index
     * @param index - Device index (0 to count-1)
     */
    static getDeviceInfo(index: number): DeviceInfo {
        return native.TACDevice.GetPortData(index);
    }

    /**
     * List all connected devices
     */
    static listDevices(): DeviceInfo[] {
        const count = this.getDeviceCount();
        const devices: DeviceInfo[] = [];

        for (let i = 0; i < count; i++) {
            try {
                devices.push(this.getDeviceInfo(i));
            } catch (error) {
                // Skip device if we can't get its info
                console.error(`Failed to get device info for index ${i}:`, error);
            }
        }

        return devices;
    }

    /**
     * Open a device by port name or serial number
     * @param portNameOrSerial - Port name (e.g., "COM3") or serial number
     * @returns TACDevice instance
     * @throws Error if device cannot be opened
     */
    static openDevice(portNameOrSerial: string): TACDevice {
        const nativeDevice = native.TACDevice.OpenHandleByDescription(portNameOrSerial);
        return new TACDevice(nativeDevice);
    }

    /**
     * Enable or disable logging
     * @param enabled - true to enable logging, false to disable
     */
    static setLogging(enabled: boolean): void {
        native.TACDevice.SetLoggingState(enabled);
    }

    /**
     * Get current logging state
     */
    static getLoggingState(): boolean {
        return native.TACDevice.GetLoggingState();
    }
}

// Export types and classes
export { TACDevice } from './device';
export { DeviceInfo, CommandMetadata, TACError } from './types';
export {
    TACDevError,
    TACDevHandleError,
    TACDevInitError,
    TACDevCommandError
} from './errors';

// Default export
export default QTAC;
