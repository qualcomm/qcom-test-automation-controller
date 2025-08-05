// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: Biswajit Roy <biswroy@qti.qualcomm.com>

package com.qualcomm.alpaca;

import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.LongByReference;

import java.nio.charset.StandardCharsets;

/**
 * The TACDevice class represents a TAC device. All QTAC TAC
 * operations can be performed using the TACDevice.
 */
final public class TACDevice {

    private final long NO_TAC_ERROR = 0;
    private final long TACDEV_BUFFER_TOO_SMALL = 1;
    private final long TACDEV_BAD_TAC_HANDLE = 2;
    private final long TACDEV_COMMAND_NOT_FOUND = 3;
    private final long TACDEV_BAD_INDEX = 4;

    private final String portName;
    private final String description;
    private final String serialNumber;

    private final int bufferSize = 1024;
    private long tacHandle = TACDEV_BAD_TAC_HANDLE;

    /**
     * The TACDevice constructor that accepts the port name description and serial number of the TAC device as input.
     * @param portName The name of the port that the TAC device is connected to as string
     * @param description The description of the TAC device as string
     * @param serialNumber The serial number of the TAC device as string
     */
    public TACDevice(String portName, String description, String serialNumber) {
        this.portName = portName;
        this.description = description;
        this.serialNumber = serialNumber;
    }

    /**
     * Returns the port name of the TAC device
     * @return The port name of the TAC device as string
     */
    public String PortName() {
        return this.portName;
    }

    /**
     * Returns the description of the TAC device
     * @return The description of the TAC device as string
     */
    public String Description() {
        return this.description;
    }

    /**
     * Returns the serial number of the TAC device
     * @return The serial number of the TAC device as string
     */
    public String SerialNumber() {
        return this.serialNumber;
    }

    /**
     * Returns whether the TAC device is valid. The TAC device is valid if it has a valid TAC handle or after Open() is called.
     * @return Whether the TAC device is valid as boolean
     */
    public boolean Valid() {
        return this.tacHandle != TACDEV_BAD_TAC_HANDLE;
    }

    /**
     * Returns true if the TAC device is opened successfully.
     * @return Whether the TAC device is opened successfully as boolean
     */
    public boolean Open() throws TACDeviceException {
        if(!this.portName.isEmpty()) {
            this.tacHandle = BaseTACDev.INSTANCE.OpenHandleByDescription(portName);
        }
        if(this.tacHandle == TACDEV_BAD_TAC_HANDLE) {
            this.tacHandle = BaseTACDev.INSTANCE.OpenHandleByDescription(serialNumber);
        }
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE)
            return true;

        byte[] buffer = new byte[bufferSize];

        if(buffer.length < 0)
            BaseTACDev.INSTANCE.GetLastTACError(buffer, bufferSize);
        else
            throw new TACDeviceException("Error occurred while opening TAC device" + buffer.toString());

        return false;
    }

    /**
     * Closes the TAC device if the TAC device is open.
     */
    public void Close() {
        BaseTACDev.INSTANCE.CloseTACHandle(this.tacHandle);
    }

    /**
     * Returns the name of the TAC device as string.
     * @return The string containing the name of the device
     * @throws TACDeviceException Throws exception if unsuccessful
     */
    public String Get_Name() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            byte[] buffer = new byte[bufferSize];
            long result = BaseTACDev.INSTANCE.GetName(this.tacHandle, buffer, bufferSize);
            if(result == NO_TAC_ERROR)
                return new String(buffer, StandardCharsets.US_ASCII).trim();
            else if(result == TACDEV_BUFFER_TOO_SMALL)
                throw new TACDeviceException("[TAC Error]: TAC buffer is too small.");
            else
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    /**
     * Returns the firmware version of the TAC device as string.
     * @return The string containing the firmware version of the device
     * @throws TACDeviceException Throws exception if unsuccessful
     */
    public String GetFirmwareVersion() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            byte[] buffer = new byte[bufferSize];
            long result = BaseTACDev.INSTANCE.GetFirmwareVersion(this.tacHandle, buffer, bufferSize);
            if(result == NO_TAC_ERROR)
                return new String(buffer, StandardCharsets.US_ASCII).trim();
            else if(result == TACDEV_BUFFER_TOO_SMALL)
                throw new TACDeviceException("[TAC Error]: TAC buffer is too small.");
            else
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    /**
     * Returns the name of the TAC device hardware as string.
     * @return The string containing the hardware name of the device
     * @throws TACDeviceException Throws exception if unsuccessful
     */
    public String GetHardware() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            byte[] buffer = new byte[bufferSize];
            long result = BaseTACDev.INSTANCE.GetHardware(this.tacHandle, buffer, bufferSize);
            if(result == NO_TAC_ERROR)
                return new String(buffer, StandardCharsets.US_ASCII).trim();
            else if(result == TACDEV_BUFFER_TOO_SMALL)
                throw new TACDeviceException("[TAC Error]: TAC buffer is too small.");
            else
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public String Get_HardwareVersion() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            byte[] buffer = new byte[bufferSize];
            long result = BaseTACDev.INSTANCE.GetHardwareVersion(this.tacHandle, buffer, bufferSize);
            if(result == NO_TAC_ERROR)
                return new String(buffer, StandardCharsets.US_ASCII).trim();
            else if(result == TACDEV_BUFFER_TOO_SMALL)
                throw new TACDeviceException("[TAC Error]: TAC buffer is too small.");
            else
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public String Get_UUID() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            byte[] buffer = new byte[bufferSize];
            long result = BaseTACDev.INSTANCE.GetUUID(this.tacHandle, buffer, bufferSize);
            if(result == NO_TAC_ERROR)
                return new String(buffer, StandardCharsets.US_ASCII).trim();
            else if(result == TACDEV_BUFFER_TOO_SMALL)
                throw new TACDeviceException("[TAC Error]: TAC buffer is too small.");
            else
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void SetExternalPowerControl(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.SetExternalPowerControl(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public long GetCommandCount() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            LongByReference commandCount = new LongByReference(0);
            long result = BaseTACDev.INSTANCE.GetCommandCount(this.tacHandle, commandCount);
            if(result == NO_TAC_ERROR)
                return commandCount.getValue();
            else
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public String GetCommand(long commandIndex) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            byte[] buffer = new byte[bufferSize];
            long result = BaseTACDev.INSTANCE.GetCommand(this.tacHandle, commandIndex, buffer, bufferSize);
            if(result == NO_TAC_ERROR)
                return new String(buffer, StandardCharsets.US_ASCII).trim();
            else if(result == TACDEV_BUFFER_TOO_SMALL)
                throw new TACDeviceException("[TAC Error]: TAC buffer is too small.");
            else
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetCommandState(String command) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            byte[] commandBuffer = command.getBytes(StandardCharsets.US_ASCII);
            long result = BaseTACDev.INSTANCE.GetCommandState(this.tacHandle, commandBuffer, newState);
            if(result == NO_TAC_ERROR)
                return newState.getValue() == 1;
            else
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void SendCommand(String command, boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            byte[] commandBuffer = command.getBytes(StandardCharsets.US_ASCII);
            long result = BaseTACDev.INSTANCE.SendCommand(this.tacHandle, commandBuffer, newState);
            if(result != NO_TAC_ERROR)
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public String GetHelpText() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            byte[] buffer = new byte[1];
            IntByReference actualSize = new IntByReference(0);
            long result = BaseTACDev.INSTANCE.GetHelpText(this.tacHandle, buffer, 1, actualSize);
            
            buffer = new byte[actualSize.getValue() + 1];
            result = BaseTACDev.INSTANCE.GetHelpText(this.tacHandle, buffer, actualSize.getValue() + 1, actualSize);

            if(result == NO_TAC_ERROR)
                return new String(buffer, StandardCharsets.US_ASCII).trim();
            else if(result == TACDEV_BUFFER_TOO_SMALL)
                throw new TACDeviceException("[TAC Error]: TAC buffer is too small.");
            else
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void SetPinState(int pin, boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            long result = BaseTACDev.INSTANCE.SetPinState(this.tacHandle, pin, newState);
            if(result != NO_TAC_ERROR)
                throw new TACDeviceException("[TAC Error]: Bad TAC Handle. Is the device open?");
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void Set_Button_Assert_Time(long secondsToAssert) throws TACDeviceException {
        throw new TACDeviceException("Set_Button_Assert_Time deprecated. This is now in the configurations");
    }

    public long Get_Button_Assert_Time() throws TACDeviceException {
        throw new TACDeviceException("Get_Button_Assert_Time deprecated. This is now in the configurations");
    }

    public void SetBatteryState(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.SetBatteryState(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetBatteryState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetBatteryState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void USB0(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.Usb0(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    boolean GetUSB0State() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetUsb0State(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void USB1(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.Usb1(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetUSB1State() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetUsb1State(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void PowerKey(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.PowerKey(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetPowerKeyState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetPowerKeyState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void VolumeUp(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.VolumeUp(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetVolumeUpState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetVolumeUpState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void VolumeDown(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.VolumeDown(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetVolumeDownState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetVolumeDownState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void DisconnectUIM1(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.DisconnectUIM1(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetDisconnectUIM1State() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetDisconnectUIM1State(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void DisconnectUIM2(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.DisconnectUIM2(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    boolean GetDisconnectUIM2State() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetDisconnectUIM2State(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void DisconnectSDCard(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.DisconnectSDCard(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    boolean GetDisconnectSDCardState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetDisconnectSDCardState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void PrimaryEDL(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.PrimaryEDL(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetPrimaryEDLState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetPrimaryEDLState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void ForcePSHoldHigh(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.ForcePSHoldHigh(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetForcePSHoldHighState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetForcePSHoldHighState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void SecondaryEDL(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.SecondaryEDL(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetSecondaryEDLState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetSecondaryEDLState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void SecondaryPM_RESIN_N(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.SecondaryPmResinN(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetSecondaryPM_RESIN_NState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetSecondaryPmResinNState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void EUD(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.Eud(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetEUDState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetEUDState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void HeadsetDisconnect(boolean newState) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.HeadsetDisconnect(this.tacHandle, newState);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public boolean GetHeadsetDisconnectState() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            IntByReference newState = new IntByReference(0);
            BaseTACDev.INSTANCE.GetHeadsetDisconnectState(this.tacHandle, newState);
            return newState.getValue() == 1;
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void SetName(String newName) throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.SetName(this.tacHandle, newName);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public long GetResetCount() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            LongByReference resetCount = new LongByReference(0);
            BaseTACDev.INSTANCE.GetResetCount(this.tacHandle, resetCount);
            return resetCount.getValue();
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void ClearResetCount() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.ClearResetCount(this.tacHandle);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void PowerOnButton() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.PowerOnButton(this.tacHandle);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void PowerOffButton() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.PowerOffButton(this.tacHandle);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void BootToFastBootButton() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.BootToFastBootButton(this.tacHandle);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void BootToUEFIMenuButton() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.BootToUEFIMenuButton(this.tacHandle);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void BootToEDLButton() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.BootToEDLButton(this.tacHandle);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }

    public void BootToSecondaryEDLButton() throws TACDeviceException {
        if(this.tacHandle != TACDEV_BAD_TAC_HANDLE) {
            BaseTACDev.INSTANCE.BootToSecondaryEDLButton(this.tacHandle);
        }
        else {
            throw new TACDeviceException("[BAD TAC Handle]: Is the device open?");
        }
    }
}
