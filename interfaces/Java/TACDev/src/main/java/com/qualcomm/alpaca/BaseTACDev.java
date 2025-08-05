/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Author: Biswajit Roy <biswroy@qti.qualcomm.com>

package com.qualcomm.alpaca;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.LongByReference;

public interface BaseTACDev extends Library {
	// The .so library will be loaded for linux and .dll for windows
	BaseTACDev INSTANCE = Native.load(("TACDev"), BaseTACDev.class);
	long InitializeTACDev();

	long GetAlpacaVersion(byte[] alpacaVersion, int bufferSize);
	long GetTACVersion(byte[] tacVersion, int bufferSize);
	long GetLastTACError(byte[] lastError, int bufferSize);

	long GetLoggingState(IntByReference loggingState);
	long SetLoggingState(boolean loggingState);

	long GetDeviceCount(IntByReference deviceCount);

	long GetPortData(int deviceIndex, byte[] portData, int bufferSize);

	long OpenHandleByDescription(String portName);
	long CloseTACHandle(long tacHandle);

	long GetName(long tacHandle, byte[] deviceName, int bufferSize);
	long GetFirmwareVersion(long tacHandle, byte[] portName, int bufferSize);
	long GetHardware(long tacHandle, byte[] hardware, int bufferSize);
	long GetHardwareVersion(long tacHandle, byte[] hardwareVersion, int bufferSize);
	long GetUUID(long tacHandle, byte[] uuid, int bufferSize);

	long SetExternalPowerControl(long tacHandle, boolean state);

	// dynamic commands
	long GetCommandCount(long tacHandle, LongByReference commandCount);
	long GetCommand(long tacHandle, long commandIndex, byte[] commandBuffer, int bufferSize);

	// This command is the root of all others
	long GetCommandState(long tacHandle, byte[] command, IntByReference state);
	long SendCommand(long tacHandle, byte[] command, boolean state);

	// Help
	long GetHelpText(long tacHandle, byte[] helpBuffer, int bufferSize, IntByReference actualSize);

	// Raw Pin State
	long SetPinState(long tacHandle, int pin, boolean state);

	long SetBatteryState(long tacHandle, boolean state);
	long GetBatteryState(long tacHandle, IntByReference state);

	long Usb0(long tacHandle, boolean state);
	long GetUsb0State(long tacHandle, IntByReference state);

	long Usb1(long tacHandle, boolean state);
	long GetUsb1State(long tacHandle, IntByReference state);

	long PowerKey(long tacHandle, boolean state);
	long GetPowerKeyState(long tacHandle, IntByReference state);

	long VolumeUp(long tacHandle, boolean state);
	long GetVolumeUpState(long tacHandle, IntByReference state);

	long VolumeDown(long tacHandle, boolean state);
	long GetVolumeDownState(long tacHandle, IntByReference state);

	long DisconnectUIM1(long tacHandle, boolean state);
	long GetDisconnectUIM1State(long tacHandle, IntByReference state);

	long DisconnectUIM2(long tacHandle, boolean state);
	long GetDisconnectUIM2State(long tacHandle, IntByReference state);

	long DisconnectSDCard(long tacHandle, boolean state);
	long GetDisconnectSDCardState(long tacHandle, IntByReference state);

	long PrimaryEDL(long tacHandle, boolean state);
	long GetPrimaryEDLState(long tacHandle, IntByReference state);

	long SecondaryEDL(long tacHandle, boolean state);
	long GetSecondaryEDLState(long tacHandle, IntByReference state);

	long ForcePSHoldHigh(long tacHandle, boolean state);
	long GetForcePSHoldHighState(long tacHandle, IntByReference state);

	long SecondaryPmResinN(long tacHandle, boolean state);
	long GetSecondaryPmResinNState(long tacHandle, IntByReference state);

	long Eud(long tacHandle, boolean state);
	long GetEUDState(long tacHandle, IntByReference state);

	long HeadsetDisconnect(long tacHandle, boolean state);
	long GetHeadsetDisconnectState(long tacHandle, IntByReference state);

	long SetName(long tacHandle, String newName);
	long GetResetCount(long tacHandle, LongByReference resetCount);
	long ClearResetCount(long tacHandle);

	long PowerOnButton(long tacHandle);
	long PowerOffButton(long tacHandle);
	long BootToFastBootButton(long tacHandle);
	long BootToUEFIMenuButton(long tacHandle);
	long BootToEDLButton(long tacHandle);
	long BootToSecondaryEDLButton(long tacHandle);
}