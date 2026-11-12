// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

import { CommandMetadata } from './types';

/**
 * TACDevice class - represents an opened TAC device
 * Wraps the native NAPI bindings with a higher-level TypeScript interface
 */
export class TACDevice {
    private nativeDevice: any;

    /**
     * Create a TACDevice from a native device handle
     * @param nativeDevice - Native NAPI device instance
     * @internal
     */
    constructor(nativeDevice: any) {
        this.nativeDevice = nativeDevice;
    }

    /**
     * Close the device handle
     * After calling this, the device instance should not be used
     */
    close(): void {
        this.nativeDevice.close();
    }

    /**
     * Get the native handle value (for debugging)
     */
    getHandle(): number {
        return this.nativeDevice.getHandle();
    }

    // Device Information

    /**
     * Get device name
     */
    getName(): string {
        return this.nativeDevice.getName();
    }

    /**
     * Get firmware version
     */
    getFirmwareVersion(): string {
        return this.nativeDevice.getFirmwareVersion();
    }

    /**
     * Get hardware type
     */
    getHardware(): string {
        return this.nativeDevice.getHardware();
    }

    /**
     * Get hardware version
     */
    getHardwareVersion(): string {
        return this.nativeDevice.getHardwareVersion();
    }

    /**
     * Get device UUID
     */
    getUUID(): string {
        return this.nativeDevice.getUUID();
    }

    // Device Control - Battery

    /**
     * Set battery state
     * @param state - true to enable, false to disable
     */
    setBatteryState(state: boolean): void {
        this.nativeDevice.setBatteryState(state);
    }

    /**
     * Get battery state
     */
    getBatteryState(): boolean {
        return this.nativeDevice.getBatteryState();
    }

    // Device Control - USB

    /**
     * Set USB0 state
     * @param state - true to enable, false to disable
     */
    setUsb0(state: boolean): void {
        this.nativeDevice.setUsb0(state);
    }

    /**
     * Get USB0 state
     */
    getUsb0State(): boolean {
        return this.nativeDevice.getUsb0State();
    }

    /**
     * Set USB1 state
     * @param state - true to enable, false to disable
     */
    setUsb1(state: boolean): void {
        this.nativeDevice.setUsb1(state);
    }

    /**
     * Get USB1 state
     */
    getUsb1State(): boolean {
        return this.nativeDevice.getUsb1State();
    }

    // Device Control - Buttons

    /**
     * Set power key state
     * @param state - true to press, false to release
     */
    setPowerKey(state: boolean): void {
        this.nativeDevice.setPowerKey(state);
    }

    /**
     * Get power key state
     */
    getPowerKeyState(): boolean {
        return this.nativeDevice.getPowerKeyState();
    }

    /**
     * Set volume up button state
     * @param state - true to press, false to release
     */
    setVolumeUp(state: boolean): void {
        this.nativeDevice.setVolumeUp(state);
    }

    /**
     * Get volume up button state
     */
    getVolumeUpState(): boolean {
        return this.nativeDevice.getVolumeUpState();
    }

    /**
     * Set volume down button state
     * @param state - true to press, false to release
     */
    setVolumeDown(state: boolean): void {
        this.nativeDevice.setVolumeDown(state);
    }

    /**
     * Get volume down button state
     */
    getVolumeDownState(): boolean {
        return this.nativeDevice.getVolumeDownState();
    }

    // Device Control - UIM/SD

    /**
     * Disconnect UIM1 (SIM card slot 1)
     * @param state - true to disconnect, false to connect
     */
    setDisconnectUIM1(state: boolean): void {
        this.nativeDevice.setDisconnectUIM1(state);
    }

    /**
     * Get UIM1 disconnect state
     */
    getDisconnectUIM1State(): boolean {
        return this.nativeDevice.getDisconnectUIM1State();
    }

    /**
     * Disconnect UIM2 (SIM card slot 2)
     * @param state - true to disconnect, false to connect
     */
    setDisconnectUIM2(state: boolean): void {
        this.nativeDevice.setDisconnectUIM2(state);
    }

    /**
     * Get UIM2 disconnect state
     */
    getDisconnectUIM2State(): boolean {
        return this.nativeDevice.getDisconnectUIM2State();
    }

    /**
     * Disconnect SD card
     * @param state - true to disconnect, false to connect
     */
    setDisconnectSDCard(state: boolean): void {
        this.nativeDevice.setDisconnectSDCard(state);
    }

    /**
     * Get SD card disconnect state
     */
    getDisconnectSDCardState(): boolean {
        return this.nativeDevice.getDisconnectSDCardState();
    }

    // Device Control - EDL

    /**
     * Set primary EDL mode
     * @param state - true to enable, false to disable
     */
    setPrimaryEDL(state: boolean): void {
        this.nativeDevice.setPrimaryEDL(state);
    }

    /**
     * Get primary EDL state
     */
    getPrimaryEDLState(): boolean {
        return this.nativeDevice.getPrimaryEDLState();
    }

    /**
     * Set secondary EDL mode
     * @param state - true to enable, false to disable
     */
    setSecondaryEDL(state: boolean): void {
        this.nativeDevice.setSecondaryEDL(state);
    }

    /**
     * Get secondary EDL state
     */
    getSecondaryEDLState(): boolean {
        return this.nativeDevice.getSecondaryEDLState();
    }

    // Device Control - Advanced

    /**
     * Force PS_HOLD signal high
     * @param state - true to force high, false to release
     */
    setForcePSHoldHigh(state: boolean): void {
        this.nativeDevice.setForcePSHoldHigh(state);
    }

    /**
     * Get force PS_HOLD high state
     */
    getForcePSHoldHighState(): boolean {
        return this.nativeDevice.getForcePSHoldHighState();
    }

    /**
     * Set secondary PM RESIN_N state
     * @param state - true to enable, false to disable
     */
    setSecondaryPmResinN(state: boolean): void {
        this.nativeDevice.setSecondaryPmResinN(state);
    }

    /**
     * Get secondary PM RESIN_N state
     */
    getSecondaryPmResinNState(): boolean {
        return this.nativeDevice.getSecondaryPmResinNState();
    }

    /**
     * Set EUD (Embedded USB Debugger) state
     * @param state - true to enable, false to disable
     */
    setEud(state: boolean): void {
        this.nativeDevice.setEud(state);
    }

    /**
     * Get EUD state
     */
    getEUDState(): boolean {
        return this.nativeDevice.getEUDState();
    }

    /**
     * Set headset disconnect state
     * @param state - true to disconnect, false to connect
     */
    setHeadsetDisconnect(state: boolean): void {
        this.nativeDevice.setHeadsetDisconnect(state);
    }

    /**
     * Get headset disconnect state
     */
    getHeadsetDisconnectState(): boolean {
        return this.nativeDevice.getHeadsetDisconnectState();
    }

    // Configuration

    /**
     * Set external power control
     * @param state - true to enable, false to disable
     */
    setExternalPowerControl(state: boolean): void {
        this.nativeDevice.setExternalPowerControl(state);
    }

    /**
     * Set device name
     * @param name - New device name
     */
    setName(name: string): void {
        this.nativeDevice.setName(name);
    }

    /**
     * Get hardware reset count
     */
    getResetCount(): number {
        return this.nativeDevice.getResetCount();
    }

    /**
     * Clear hardware reset count
     */
    clearResetCount(): void {
        this.nativeDevice.clearResetCount();
    }

    // Commands

    /**
     * Get number of available commands
     */
    getCommandCount(): number {
        return this.nativeDevice.getCommandCount();
    }

    /**
     * Get command metadata by index
     * @param index - Command index (0 to count-1)
     */
    getCommand(index: number): CommandMetadata {
        return this.nativeDevice.getCommand(index);
    }

    /**
     * Get all available commands
     */
    getAllCommands(): CommandMetadata[] {
        const count = this.getCommandCount();
        const commands: CommandMetadata[] = [];
        for (let i = 0; i < count; i++) {
            commands.push(this.getCommand(i));
        }
        return commands;
    }

    /**
     * Get command state
     * @param command - Command name
     */
    getCommandState(command: string): boolean {
        return this.nativeDevice.getCommandState(command);
    }

    /**
     * Send command to device
     * @param command - Command name
     * @param state - Command state
     */
    sendCommand(command: string, state: boolean): void {
        this.nativeDevice.sendCommand(command, state);
    }

    // Quick Commands

    /**
     * Get number of quick commands
     */
    getQuickCommandCount(): number {
        return this.nativeDevice.getQuickCommandCount();
    }

    /**
     * Get quick command name by index
     * @param index - Quick command index
     */
    getQuickCommand(index: number): string {
        return this.nativeDevice.getQuickCommand(index);
    }

    /**
     * Execute power on sequence
     */
    powerOn(): void {
        this.nativeDevice.powerOnButton();
    }

    /**
     * Execute power off sequence
     */
    powerOff(): void {
        this.nativeDevice.powerOffButton();
    }

    /**
     * Boot device to fastboot mode
     */
    bootToFastboot(): void {
        this.nativeDevice.bootToFastBootButton();
    }

    /**
     * Boot device to UEFI menu
     */
    bootToUEFI(): void {
        this.nativeDevice.bootToUEFIMenuButton();
    }

    /**
     * Boot device to EDL mode
     */
    bootToEDL(): void {
        this.nativeDevice.bootToEDLButton();
    }

    /**
     * Boot device to secondary EDL mode
     */
    bootToSecondaryEDL(): void {
        this.nativeDevice.bootToSecondaryEDLButton();
    }

    // Script Variables

    /**
     * Get number of script variables
     */
    getScriptVariableCount(): number {
        return this.nativeDevice.getScriptVariableCount();
    }

    /**
     * Get script variable definition by index
     * @param index - Variable index
     */
    getScriptVariable(index: number): string {
        return this.nativeDevice.getScriptVariable(index);
    }

    /**
     * Update script variable value
     * @param variable - Variable name
     * @param value - New value
     */
    updateScriptVariable(variable: string, value: string): void {
        this.nativeDevice.updateScriptVariableValue(variable, value);
    }

    // Advanced

    /**
     * Set pin state directly
     * @param pin - Pin number
     * @param state - Pin state
     */
    setPinState(pin: number, state: boolean): void {
        this.nativeDevice.setPinState(pin, state);
    }

    /**
     * Check if command queue is clear
     * Note: This may block for up to 10 seconds on PIC32CX devices
     */
    isCommandQueueClear(): boolean {
        return this.nativeDevice.isCommandQueueClear();
    }

    /**
     * Get device help text
     */
    getHelpText(): string {
        return this.nativeDevice.getHelpText();
    }
}
