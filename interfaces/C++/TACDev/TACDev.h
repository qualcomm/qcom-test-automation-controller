#ifndef DEVTAC_H
#define DEVTAC_H
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

/*
   Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#if defined(TACDEV_LIBRARY)
	#ifdef __linux__
		#define TACDEV_EXPORT __attribute__((visibility("default")))
	#else
		#define TACDEV_EXPORT __declspec(dllexport)
	#endif
#else
	#ifdef __linux__
	#   define TACDEV_EXPORT __attribute__((visibility("default")))
	#else
	#  define TACDEV_EXPORT  __declspec(dllimport)
	#endif
#endif

typedef unsigned long TAC_HANDLE;
typedef unsigned long TAC_ERROR;
typedef unsigned long TAC_RESULT;

const TAC_HANDLE kBadHandle{0};

// Keep in sync with TacException.h

const unsigned long NO_TAC_ERROR{0};
const unsigned long TACDEV_BUFFER_TOO_SMALL{1};
const unsigned long TACDEV_BAD_TAC_HANDLE{2};
const unsigned long TACDEV_COMMAND_NOT_FOUND{3};
const unsigned long TACDEV_BAD_INDEX{4};
const unsigned long TACDEV_INIT_FAILED{5};
const unsigned long TACDEV_SCRIPT_VARIABLE_NOT_FOUND{6};

#ifdef __cplusplus
extern "C" {
#endif
	// Must be called before any other calls
	TACDEV_EXPORT TAC_RESULT InitializeTACDev();

	TACDEV_EXPORT TAC_RESULT GetAlpacaVersion(char* alpacaVersion, int bufferSize);
	TACDEV_EXPORT TAC_RESULT GetTACVersion(char* tacVersion, int bufferSize);
	TACDEV_EXPORT TAC_RESULT GetLastTACError(char* lastError, int bufferSize);

	TACDEV_EXPORT TAC_RESULT GetLoggingState(bool* loggingState);
	TACDEV_EXPORT TAC_RESULT SetLoggingState(bool loggingState);

	TACDEV_EXPORT TAC_RESULT GetDeviceCount(int* deviceCount);
	TACDEV_EXPORT unsigned long GetPortData(int deviceIndex, char* portData, int bufferSize);

	TACDEV_EXPORT TAC_HANDLE OpenHandleByDescription(const char* portName);
	TACDEV_EXPORT TAC_RESULT CloseTACHandle(TAC_HANDLE tacHandle);

	TACDEV_EXPORT TAC_RESULT GetName(TAC_HANDLE tacHandle, char* deviceName, int bufferSize);
	TACDEV_EXPORT TAC_RESULT GetFirmwareVersion(TAC_HANDLE tacHandle, char* firmwareVersion, int bufferSize);
	TACDEV_EXPORT TAC_RESULT GetHardware(TAC_HANDLE tacHandle, char* hardware, int bufferSize);
	TACDEV_EXPORT TAC_RESULT GetHardwareVersion(TAC_HANDLE tacHandle, char* hardwareVersion, int bufferSize);
	TACDEV_EXPORT TAC_RESULT GetUUID(TAC_HANDLE tacHandle, char* uuid, int bufferSize);

	TACDEV_EXPORT TAC_ERROR SetExternalPowerControl(TAC_HANDLE tacHandle, bool state);

	// dynamic commands
	TACDEV_EXPORT TAC_ERROR GetCommandCount(TAC_HANDLE tacHandle, unsigned long* commandCount);
	TACDEV_EXPORT TAC_ERROR GetCommand(TAC_HANDLE tacHandle, unsigned long commandIndex, char* commandBuffer, int bufferSize);

	// quick commands
	TACDEV_EXPORT TAC_ERROR GetQuickCommandCount(TAC_HANDLE tacHandle, unsigned long* commandCount);
	TACDEV_EXPORT TAC_ERROR GetQuickCommand(TAC_HANDLE tacHandle, unsigned long commandIndex, char* commandBuffer, int bufferSize);

	// script variables
	TACDEV_EXPORT TAC_ERROR GetScriptVariableCount(TAC_HANDLE tacHandle, unsigned long* scriptVariableCount);
	TACDEV_EXPORT TAC_ERROR GetScriptVariable(TAC_HANDLE tacHandle, unsigned long scriptVariableIndex, char* scriptVariableBuffer, int bufferSize);
    TACDEV_EXPORT TAC_ERROR UpdateScriptVariableValue(TAC_HANDLE tacHandle, const char* scriptVariable, const char* scriptVariableValue);

	// This command is the root of all others
	TACDEV_EXPORT TAC_ERROR GetCommandState(TAC_HANDLE tacHandle, const char* command, bool* state);
	TACDEV_EXPORT TAC_ERROR SendCommand(TAC_HANDLE tacHandle, const char* command, bool state);

	// Help
	TACDEV_EXPORT TAC_ERROR GetHelpText(TAC_HANDLE tacHandle, char* helpBuffer, int bufferSize, int* actualSize);

	// Raw Pin State
	TACDEV_EXPORT TAC_ERROR SetPinState(TAC_HANDLE tacHandle, int pin, bool state);

	TACDEV_EXPORT TAC_RESULT SetBatteryState(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetBatteryState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT Usb0(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetUsb0State(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT Usb1(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetUsb1State(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT PowerKey(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetPowerKeyState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT VolumeUp(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetVolumeUpState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT VolumeDown(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetVolumeDownState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT DisconnectUIM1(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetDisconnectUIM1State(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT DisconnectUIM2(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetDisconnectUIM2State(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT DisconnectSDCard(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetDisconnectSDCardState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT PrimaryEDL(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetPrimaryEDLState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT SecondaryEDL(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetSecondaryEDLState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT ForcePSHoldHigh(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetForcePSHoldHighState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT SecondaryPmResinN(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetSecondaryPmResinNState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT Eud(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetEUDState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT HeadsetDisconnect(TAC_HANDLE tacHandle, bool state);
	TACDEV_EXPORT TAC_RESULT GetHeadsetDisconnectState(TAC_HANDLE tacHandle, bool* state);

	TACDEV_EXPORT TAC_RESULT SetName(TAC_HANDLE tacHandle, const char* newName);
	TACDEV_EXPORT TAC_RESULT GetResetCount(TAC_HANDLE tacHandle, int* resetCount);
	TACDEV_EXPORT TAC_RESULT ClearResetCount(TAC_HANDLE tacHandle);

	TACDEV_EXPORT TAC_RESULT PowerOnButton(TAC_HANDLE tacHandle);
	TACDEV_EXPORT TAC_RESULT PowerOffButton(TAC_HANDLE tacHandle);
	TACDEV_EXPORT TAC_RESULT BootToFastBootButton(TAC_HANDLE tacHandle);
	TACDEV_EXPORT TAC_RESULT BootToUEFIMenuButton(TAC_HANDLE tacHandle);
	TACDEV_EXPORT TAC_RESULT BootToEDLButton(TAC_HANDLE tacHandle);
	TACDEV_EXPORT TAC_RESULT BootToSecondaryEDLButton(TAC_HANDLE tacHandle);

#ifdef __cplusplus
}
#endif

#endif // DEVTAC_H
