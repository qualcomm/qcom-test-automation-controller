// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <napi.h>
#include "TACDev.h"

namespace qtac_napi {

// NAPI wrapper for TACDev C API
class TACDevWrapper : public Napi::ObjectWrap<TACDevWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    explicit TACDevWrapper(const Napi::CallbackInfo& info);
    ~TACDevWrapper();

private:
    TAC_HANDLE handle_;

    // Static methods (no device handle required)
    static Napi::Value InitializeTACDev(const Napi::CallbackInfo& info);
    static Napi::Value GetAlpacaVersion(const Napi::CallbackInfo& info);
    static Napi::Value GetTACVersion(const Napi::CallbackInfo& info);
    static Napi::Value GetLastTACError(const Napi::CallbackInfo& info);

    static Napi::Value GetLoggingState(const Napi::CallbackInfo& info);
    static Napi::Value SetLoggingState(const Napi::CallbackInfo& info);

    static Napi::Value GetDeviceCount(const Napi::CallbackInfo& info);
    static Napi::Value GetPortData(const Napi::CallbackInfo& info);

    static Napi::Value OpenHandleByDescription(const Napi::CallbackInfo& info);

    // Instance methods (require device handle)
    Napi::Value Close(const Napi::CallbackInfo& info);
    Napi::Value GetHandle(const Napi::CallbackInfo& info);

    // Device Information
    Napi::Value GetName(const Napi::CallbackInfo& info);
    Napi::Value GetFirmwareVersion(const Napi::CallbackInfo& info);
    Napi::Value GetHardware(const Napi::CallbackInfo& info);
    Napi::Value GetHardwareVersion(const Napi::CallbackInfo& info);
    Napi::Value GetUUID(const Napi::CallbackInfo& info);

    // Device Control - Battery
    Napi::Value SetBatteryState(const Napi::CallbackInfo& info);
    Napi::Value GetBatteryState(const Napi::CallbackInfo& info);

    // Device Control - USB
    Napi::Value SetUsb0(const Napi::CallbackInfo& info);
    Napi::Value GetUsb0State(const Napi::CallbackInfo& info);
    Napi::Value SetUsb1(const Napi::CallbackInfo& info);
    Napi::Value GetUsb1State(const Napi::CallbackInfo& info);

    // Device Control - Buttons
    Napi::Value SetPowerKey(const Napi::CallbackInfo& info);
    Napi::Value GetPowerKeyState(const Napi::CallbackInfo& info);
    Napi::Value SetVolumeUp(const Napi::CallbackInfo& info);
    Napi::Value GetVolumeUpState(const Napi::CallbackInfo& info);
    Napi::Value SetVolumeDown(const Napi::CallbackInfo& info);
    Napi::Value GetVolumeDownState(const Napi::CallbackInfo& info);

    // Device Control - UIM/SD
    Napi::Value SetDisconnectUIM1(const Napi::CallbackInfo& info);
    Napi::Value GetDisconnectUIM1State(const Napi::CallbackInfo& info);
    Napi::Value SetDisconnectUIM2(const Napi::CallbackInfo& info);
    Napi::Value GetDisconnectUIM2State(const Napi::CallbackInfo& info);
    Napi::Value SetDisconnectSDCard(const Napi::CallbackInfo& info);
    Napi::Value GetDisconnectSDCardState(const Napi::CallbackInfo& info);

    // Device Control - EDL
    Napi::Value SetPrimaryEDL(const Napi::CallbackInfo& info);
    Napi::Value GetPrimaryEDLState(const Napi::CallbackInfo& info);
    Napi::Value SetSecondaryEDL(const Napi::CallbackInfo& info);
    Napi::Value GetSecondaryEDLState(const Napi::CallbackInfo& info);

    // Device Control - Advanced
    Napi::Value SetForcePSHoldHigh(const Napi::CallbackInfo& info);
    Napi::Value GetForcePSHoldHighState(const Napi::CallbackInfo& info);
    Napi::Value SetSecondaryPmResinN(const Napi::CallbackInfo& info);
    Napi::Value GetSecondaryPmResinNState(const Napi::CallbackInfo& info);
    Napi::Value SetEud(const Napi::CallbackInfo& info);
    Napi::Value GetEUDState(const Napi::CallbackInfo& info);
    Napi::Value SetHeadsetDisconnect(const Napi::CallbackInfo& info);
    Napi::Value GetHeadsetDisconnectState(const Napi::CallbackInfo& info);

    // Configuration
    Napi::Value SetExternalPowerControl(const Napi::CallbackInfo& info);
    Napi::Value SetName(const Napi::CallbackInfo& info);
    Napi::Value GetResetCount(const Napi::CallbackInfo& info);
    Napi::Value ClearResetCount(const Napi::CallbackInfo& info);

    // Commands
    Napi::Value GetCommandCount(const Napi::CallbackInfo& info);
    Napi::Value GetCommand(const Napi::CallbackInfo& info);
    Napi::Value GetCommandState(const Napi::CallbackInfo& info);
    Napi::Value SendCommand(const Napi::CallbackInfo& info);

    // Quick Commands
    Napi::Value GetQuickCommandCount(const Napi::CallbackInfo& info);
    Napi::Value GetQuickCommand(const Napi::CallbackInfo& info);
    Napi::Value PowerOnButton(const Napi::CallbackInfo& info);
    Napi::Value PowerOffButton(const Napi::CallbackInfo& info);
    Napi::Value BootToFastBootButton(const Napi::CallbackInfo& info);
    Napi::Value BootToUEFIMenuButton(const Napi::CallbackInfo& info);
    Napi::Value BootToEDLButton(const Napi::CallbackInfo& info);
    Napi::Value BootToSecondaryEDLButton(const Napi::CallbackInfo& info);

    // Script Variables
    Napi::Value GetScriptVariableCount(const Napi::CallbackInfo& info);
    Napi::Value GetScriptVariable(const Napi::CallbackInfo& info);
    Napi::Value UpdateScriptVariableValue(const Napi::CallbackInfo& info);

    // Advanced
    Napi::Value SetPinState(const Napi::CallbackInfo& info);
    Napi::Value IsCommandQueueClear(const Napi::CallbackInfo& info);
    Napi::Value GetHelpText(const Napi::CallbackInfo& info);
};

} // namespace qtac_napi
