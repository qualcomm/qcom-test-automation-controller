// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Load the native module produced by `npm run build:native` directly.
// This keeps the examples runnable before the TypeScript wrapper is compiled.
const path = require('path');

const repositoryRoot = path.resolve(__dirname, '../../..');
const runtimeArchitecture = {
    x64: 'x64',
    arm64: 'ARM64',
}[process.arch];

if (!runtimeArchitecture) {
    throw new Error(`Unsupported Node.js architecture: ${process.arch}. Expected x64 or arm64.`);
}

const qtBin = process.env.QTBIN;
const runtimeDirectory = process.platform === 'win32'
    ? path.join(repositoryRoot, '__Builds', runtimeArchitecture, 'Release', 'bin')
    : path.join(repositoryRoot, '__Builds', 'Linux', 'Release', 'lib');

// The native addon needs Qt and TACDev libraries at load time. Build scripts
// set these paths only for their own process, so configure the example process
// before requiring qtac.node.
if (process.platform === 'win32') {
    process.env.PATH = [qtBin, runtimeDirectory, process.env.PATH]
        .filter(Boolean)
        .join(path.delimiter);
} else if (process.platform === 'linux') {
    const qtLib = qtBin ? path.join(path.dirname(qtBin), 'lib') : undefined;
    const requiredLibraryPaths = [runtimeDirectory, qtLib].filter(Boolean);
    const configuredLibraryPaths = (process.env.LD_LIBRARY_PATH || '').split(path.delimiter);
    const missingLibraryPaths = requiredLibraryPaths.filter(
        libraryPath => !configuredLibraryPaths.includes(libraryPath)
    );
    if (missingLibraryPaths.length > 0) {
        const suggestedLibraryPath = [...missingLibraryPaths, ...configuredLibraryPaths]
            .filter(Boolean)
            .join(path.delimiter);
        throw new Error(
            `Start Node with LD_LIBRARY_PATH="${suggestedLibraryPath}" so the native addon's shared libraries can be found.`
        );
    }
} else {
    throw new Error(`Unsupported platform: ${process.platform}`);
}

if (!require('fs').existsSync(runtimeDirectory)) {
    throw new Error(`TACDev runtime directory was not found: ${runtimeDirectory}. Build QTAC first.`);
}

const native = require(path.resolve(__dirname, '../build/Release/qtac.node'));
const TACDevice = native.TACDevice;

TACDevice.InitializeTACDev();

class Device {
    constructor(nativeDevice) {
        this.nativeDevice = nativeDevice;
    }

    close() { this.nativeDevice.close(); }
    getHandle() { return this.nativeDevice.getHandle(); }
    getName() { return this.nativeDevice.getName(); }
    getFirmwareVersion() { return this.nativeDevice.getFirmwareVersion(); }
    getHardware() { return this.nativeDevice.getHardware(); }
    getHardwareVersion() { return this.nativeDevice.getHardwareVersion(); }
    getUUID() { return this.nativeDevice.getUUID(); }

    setBatteryState(value) { return this.nativeDevice.setBatteryState(value); }
    getBatteryState() { return this.nativeDevice.getBatteryState(); }
    setUsb0(value) { return this.nativeDevice.setUsb0(value); }
    getUsb0State() { return this.nativeDevice.getUsb0State(); }
    setUsb1(value) { return this.nativeDevice.setUsb1(value); }
    getUsb1State() { return this.nativeDevice.getUsb1State(); }
    setPowerKey(value) { return this.nativeDevice.setPowerKey(value); }
    getPowerKeyState() { return this.nativeDevice.getPowerKeyState(); }
    setVolumeUp(value) { return this.nativeDevice.setVolumeUp(value); }
    getVolumeUpState() { return this.nativeDevice.getVolumeUpState(); }
    setVolumeDown(value) { return this.nativeDevice.setVolumeDown(value); }
    getVolumeDownState() { return this.nativeDevice.getVolumeDownState(); }

    getCommandCount() { return this.nativeDevice.getCommandCount(); }
    getCommand(index) { return this.nativeDevice.getCommand(index); }
    getAllCommands() {
        return Array.from({ length: this.getCommandCount() }, (_, index) => this.getCommand(index));
    }
    getCommandState(command) { return this.nativeDevice.getCommandState(command); }
    sendCommand(command, state) { return this.nativeDevice.sendCommand(command, state); }

    getQuickCommandCount() { return this.nativeDevice.getQuickCommandCount(); }
    getQuickCommand(index) { return this.nativeDevice.getQuickCommand(index); }
    powerOn() { return this.nativeDevice.powerOnButton(); }
    powerOff() { return this.nativeDevice.powerOffButton(); }
    bootToFastboot() { return this.nativeDevice.bootToFastBootButton(); }
    bootToUEFI() { return this.nativeDevice.bootToUEFIMenuButton(); }
    bootToEDL() { return this.nativeDevice.bootToEDLButton(); }

    getScriptVariableCount() { return this.nativeDevice.getScriptVariableCount(); }
    getScriptVariable(index) { return this.nativeDevice.getScriptVariable(index); }
    updateScriptVariable(variable, value) {
        return this.nativeDevice.updateScriptVariableValue(variable, value);
    }
}

const QTAC = {
    getVersion: () => TACDevice.GetAlpacaVersion(),
    getTACVersion: () => TACDevice.GetTACVersion(),
    getDeviceCount: () => TACDevice.GetDeviceCount(),
    listDevices: () => Array.from({ length: TACDevice.GetDeviceCount() }, (_, index) =>
        TACDevice.GetPortData(index)),
    openDevice: port => new Device(TACDevice.OpenHandleByDescription(port)),
};

module.exports = QTAC;
module.exports.native = native;
