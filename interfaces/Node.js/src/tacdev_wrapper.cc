// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "tacdev_wrapper.h"
#include "utils.h"
#include <memory>

namespace qtac_napi {

// Initialize the NAPI wrapper class
Napi::Object TACDevWrapper::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "TACDevice", {
        // Static methods
        StaticMethod("InitializeTACDev", &TACDevWrapper::InitializeTACDev),
        StaticMethod("GetAlpacaVersion", &TACDevWrapper::GetAlpacaVersion),
        StaticMethod("GetTACVersion", &TACDevWrapper::GetTACVersion),
        StaticMethod("GetLastTACError", &TACDevWrapper::GetLastTACError),
        StaticMethod("GetLoggingState", &TACDevWrapper::GetLoggingState),
        StaticMethod("SetLoggingState", &TACDevWrapper::SetLoggingState),
        StaticMethod("GetDeviceCount", &TACDevWrapper::GetDeviceCount),
        StaticMethod("GetPortData", &TACDevWrapper::GetPortData),
        StaticMethod("OpenHandleByDescription", &TACDevWrapper::OpenHandleByDescription),

        // Instance methods
        InstanceMethod("close", &TACDevWrapper::Close),
        InstanceMethod("getHandle", &TACDevWrapper::GetHandle),

        // Device information
        InstanceMethod("getName", &TACDevWrapper::GetName),
        InstanceMethod("getFirmwareVersion", &TACDevWrapper::GetFirmwareVersion),
        InstanceMethod("getHardware", &TACDevWrapper::GetHardware),
        InstanceMethod("getHardwareVersion", &TACDevWrapper::GetHardwareVersion),
        InstanceMethod("getUUID", &TACDevWrapper::GetUUID),

        // Battery control
        InstanceMethod("setBatteryState", &TACDevWrapper::SetBatteryState),
        InstanceMethod("getBatteryState", &TACDevWrapper::GetBatteryState),

        // USB control
        InstanceMethod("setUsb0", &TACDevWrapper::SetUsb0),
        InstanceMethod("getUsb0State", &TACDevWrapper::GetUsb0State),
        InstanceMethod("setUsb1", &TACDevWrapper::SetUsb1),
        InstanceMethod("getUsb1State", &TACDevWrapper::GetUsb1State),

        // Button control
        InstanceMethod("setPowerKey", &TACDevWrapper::SetPowerKey),
        InstanceMethod("getPowerKeyState", &TACDevWrapper::GetPowerKeyState),
        InstanceMethod("setVolumeUp", &TACDevWrapper::SetVolumeUp),
        InstanceMethod("getVolumeUpState", &TACDevWrapper::GetVolumeUpState),
        InstanceMethod("setVolumeDown", &TACDevWrapper::SetVolumeDown),
        InstanceMethod("getVolumeDownState", &TACDevWrapper::GetVolumeDownState),

        // UIM/SD control
        InstanceMethod("setDisconnectUIM1", &TACDevWrapper::SetDisconnectUIM1),
        InstanceMethod("getDisconnectUIM1State", &TACDevWrapper::GetDisconnectUIM1State),
        InstanceMethod("setDisconnectUIM2", &TACDevWrapper::SetDisconnectUIM2),
        InstanceMethod("getDisconnectUIM2State", &TACDevWrapper::GetDisconnectUIM2State),
        InstanceMethod("setDisconnectSDCard", &TACDevWrapper::SetDisconnectSDCard),
        InstanceMethod("getDisconnectSDCardState", &TACDevWrapper::GetDisconnectSDCardState),

        // EDL control
        InstanceMethod("setPrimaryEDL", &TACDevWrapper::SetPrimaryEDL),
        InstanceMethod("getPrimaryEDLState", &TACDevWrapper::GetPrimaryEDLState),
        InstanceMethod("setSecondaryEDL", &TACDevWrapper::SetSecondaryEDL),
        InstanceMethod("getSecondaryEDLState", &TACDevWrapper::GetSecondaryEDLState),

        // Advanced control
        InstanceMethod("setForcePSHoldHigh", &TACDevWrapper::SetForcePSHoldHigh),
        InstanceMethod("getForcePSHoldHighState", &TACDevWrapper::GetForcePSHoldHighState),
        InstanceMethod("setSecondaryPmResinN", &TACDevWrapper::SetSecondaryPmResinN),
        InstanceMethod("getSecondaryPmResinNState", &TACDevWrapper::GetSecondaryPmResinNState),
        InstanceMethod("setEud", &TACDevWrapper::SetEud),
        InstanceMethod("getEUDState", &TACDevWrapper::GetEUDState),
        InstanceMethod("setHeadsetDisconnect", &TACDevWrapper::SetHeadsetDisconnect),
        InstanceMethod("getHeadsetDisconnectState", &TACDevWrapper::GetHeadsetDisconnectState),

        // Configuration
        InstanceMethod("setExternalPowerControl", &TACDevWrapper::SetExternalPowerControl),
        InstanceMethod("setName", &TACDevWrapper::SetName),
        InstanceMethod("getResetCount", &TACDevWrapper::GetResetCount),
        InstanceMethod("clearResetCount", &TACDevWrapper::ClearResetCount),

        // Commands
        InstanceMethod("getCommandCount", &TACDevWrapper::GetCommandCount),
        InstanceMethod("getCommand", &TACDevWrapper::GetCommand),
        InstanceMethod("getCommandState", &TACDevWrapper::GetCommandState),
        InstanceMethod("sendCommand", &TACDevWrapper::SendCommand),

        // Quick commands
        InstanceMethod("getQuickCommandCount", &TACDevWrapper::GetQuickCommandCount),
        InstanceMethod("getQuickCommand", &TACDevWrapper::GetQuickCommand),
        InstanceMethod("powerOnButton", &TACDevWrapper::PowerOnButton),
        InstanceMethod("powerOffButton", &TACDevWrapper::PowerOffButton),
        InstanceMethod("bootToFastBootButton", &TACDevWrapper::BootToFastBootButton),
        InstanceMethod("bootToUEFIMenuButton", &TACDevWrapper::BootToUEFIMenuButton),
        InstanceMethod("bootToEDLButton", &TACDevWrapper::BootToEDLButton),
        InstanceMethod("bootToSecondaryEDLButton", &TACDevWrapper::BootToSecondaryEDLButton),

        // Script variables
        InstanceMethod("getScriptVariableCount", &TACDevWrapper::GetScriptVariableCount),
        InstanceMethod("getScriptVariable", &TACDevWrapper::GetScriptVariable),
        InstanceMethod("updateScriptVariableValue", &TACDevWrapper::UpdateScriptVariableValue),

        // Advanced
        InstanceMethod("setPinState", &TACDevWrapper::SetPinState),
        InstanceMethod("isCommandQueueClear", &TACDevWrapper::IsCommandQueueClear),
        InstanceMethod("getHelpText", &TACDevWrapper::GetHelpText),
    });

    exports.Set("TACDevice", func);
    return exports;
}

TACDevWrapper::TACDevWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<TACDevWrapper>(info), handle_(kBadHandle) {
    // Constructor for creating wrapper from JavaScript
    if (info.Length() > 0 && info[0].IsNumber()) {
        handle_ = info[0].As<Napi::Number>().Uint32Value();
    }
}

TACDevWrapper::~TACDevWrapper() {
    if (handle_ != kBadHandle && handle_ != 0) {
        CloseTACHandle(handle_);
        handle_ = kBadHandle;
    }
}

// Static Methods

Napi::Value TACDevWrapper::InitializeTACDev(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    TAC_RESULT result = ::InitializeTACDev();

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to initialize TACDev");
        return env.Undefined();
    }

    return Napi::Number::New(env, result);
}

Napi::Value TACDevWrapper::GetAlpacaVersion(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string version = CallStringFunction(::GetAlpacaVersion);
    return Napi::String::New(env, version);
}

Napi::Value TACDevWrapper::GetTACVersion(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string version = CallStringFunction(::GetTACVersion);
    return Napi::String::New(env, version);
}

Napi::Value TACDevWrapper::GetLastTACError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string error = GetLastErrorMessage();
    return Napi::String::New(env, error);
}

Napi::Value TACDevWrapper::GetLoggingState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bool state = false;
    TAC_RESULT result = ::GetLoggingState(&state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get logging state");
        return env.Undefined();
    }

    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetLoggingState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::SetLoggingState(state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to set logging state");
        return env.Undefined();
    }

    return env.Undefined();
}

Napi::Value TACDevWrapper::GetDeviceCount(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    int count = 0;
    TAC_RESULT result = ::GetDeviceCount(&count);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get device count");
        return env.Undefined();
    }

    return Napi::Number::New(env, count);
}

Napi::Value TACDevWrapper::GetPortData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Number expected for device index").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    int index = info[0].As<Napi::Number>().Int32Value();
    char buffer[512];
    // GetPortData returns the number of bytes copied, not a TAC error code.
    unsigned long bytesWritten = ::GetPortData(index, buffer, sizeof(buffer));

    if (bytesWritten == 0) {
        Napi::Error::New(env, "Failed to get port data for device index " +
            std::to_string(index)).ThrowAsJavaScriptException();
        return env.Undefined();
    }

    return ParseDeviceInfo(env, std::string(buffer));
}

Napi::Value TACDevWrapper::OpenHandleByDescription(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected for port name").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    std::string portName = info[0].As<Napi::String>().Utf8Value();
    TAC_HANDLE handle = ::OpenHandleByDescription(portName.c_str());

    if (handle == kBadHandle || handle == 0) {
        Napi::Error::New(env, "Failed to open device: " + portName).ThrowAsJavaScriptException();
        return env.Undefined();
    }

    // Create new TACDevWrapper instance
    Napi::Function constructor = info.Env().GetInstanceData<Napi::FunctionReference>()->Value();
    return constructor.New({Napi::Number::New(env, handle)});
}

// Instance Methods

Napi::Value TACDevWrapper::Close(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (handle_ != kBadHandle && handle_ != 0) {
        TAC_RESULT result = ::CloseTACHandle(handle_);
        handle_ = kBadHandle;

        if (result != NO_TAC_ERROR) {
            ThrowTACError(env, result, "Failed to close device handle");
        }
    }

    return env.Undefined();
}

Napi::Value TACDevWrapper::GetHandle(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::Number::New(env, handle_);
}

// Device Information

Napi::Value TACDevWrapper::GetName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    std::string name = CallStringFunctionWithHandle(handle_, ::GetName);
    return Napi::String::New(env, name);
}

Napi::Value TACDevWrapper::GetFirmwareVersion(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    std::string fw = CallStringFunctionWithHandle(handle_, ::GetFirmwareVersion);
    return Napi::String::New(env, fw);
}

Napi::Value TACDevWrapper::GetHardware(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    std::string hw = CallStringFunctionWithHandle(handle_, ::GetHardware);
    return Napi::String::New(env, hw);
}

Napi::Value TACDevWrapper::GetHardwareVersion(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    std::string hwVer = CallStringFunctionWithHandle(handle_, ::GetHardwareVersion);
    return Napi::String::New(env, hwVer);
}

Napi::Value TACDevWrapper::GetUUID(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    std::string uuid = CallStringFunctionWithHandle(handle_, ::GetUUID);
    return Napi::String::New(env, uuid);
}

// Device Control - Battery

Napi::Value TACDevWrapper::SetBatteryState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::SetBatteryState(handle_, state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to set battery state");
    }

    return env.Undefined();
}

Napi::Value TACDevWrapper::GetBatteryState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    bool state = false;
    TAC_RESULT result = ::GetBatteryState(handle_, &state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get battery state");
        return env.Undefined();
    }

    return Napi::Boolean::New(env, state);
}

// Device Control - USB

Napi::Value TACDevWrapper::SetUsb0(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::Usb0(handle_, state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to set USB0 state");
    }

    return env.Undefined();
}

Napi::Value TACDevWrapper::GetUsb0State(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    bool state = false;
    TAC_RESULT result = ::GetUsb0State(handle_, &state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get USB0 state");
        return env.Undefined();
    }

    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetUsb1(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::Usb1(handle_, state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to set USB1 state");
    }

    return env.Undefined();
}

Napi::Value TACDevWrapper::GetUsb1State(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    bool state = false;
    TAC_RESULT result = ::GetUsb1State(handle_, &state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get USB1 state");
        return env.Undefined();
    }

    return Napi::Boolean::New(env, state);
}

// Device Control - Buttons (PowerKey, VolumeUp, VolumeDown)

Napi::Value TACDevWrapper::SetPowerKey(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::PowerKey(handle_, state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to set PowerKey state");
    }

    return env.Undefined();
}

Napi::Value TACDevWrapper::GetPowerKeyState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    bool state = false;
    TAC_RESULT result = ::GetPowerKeyState(handle_, &state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get PowerKey state");
        return env.Undefined();
    }

    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetVolumeUp(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::VolumeUp(handle_, state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to set VolumeUp state");
    }

    return env.Undefined();
}

Napi::Value TACDevWrapper::GetVolumeUpState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    bool state = false;
    TAC_RESULT result = ::GetVolumeUpState(handle_, &state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get VolumeUp state");
        return env.Undefined();
    }

    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetVolumeDown(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::VolumeDown(handle_, state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to set VolumeDown state");
    }

    return env.Undefined();
}

Napi::Value TACDevWrapper::GetVolumeDownState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    bool state = false;
    TAC_RESULT result = ::GetVolumeDownState(handle_, &state);

    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get VolumeDown state");
        return env.Undefined();
    }

    return Napi::Boolean::New(env, state);
}

// Device Control - UIM/SD (I'll continue with similar patterns for remaining controls)
// For brevity, implementing key ones and similar patterns can be replicated

Napi::Value TACDevWrapper::SetDisconnectUIM1(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::DisconnectUIM1(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set DisconnectUIM1");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetDisconnectUIM1State(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool state = false;
    TAC_RESULT result = ::GetDisconnectUIM1State(handle_, &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get DisconnectUIM1 state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetDisconnectUIM2(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::DisconnectUIM2(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set DisconnectUIM2");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetDisconnectUIM2State(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool state = false;
    TAC_RESULT result = ::GetDisconnectUIM2State(handle_, &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get DisconnectUIM2 state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetDisconnectSDCard(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::DisconnectSDCard(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set DisconnectSDCard");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetDisconnectSDCardState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool state = false;
    TAC_RESULT result = ::GetDisconnectSDCardState(handle_, &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get DisconnectSDCard state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

// EDL Controls

Napi::Value TACDevWrapper::SetPrimaryEDL(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::PrimaryEDL(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set PrimaryEDL");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetPrimaryEDLState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool state = false;
    TAC_RESULT result = ::GetPrimaryEDLState(handle_, &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get PrimaryEDL state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetSecondaryEDL(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::SecondaryEDL(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set SecondaryEDL");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetSecondaryEDLState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool state = false;
    TAC_RESULT result = ::GetSecondaryEDLState(handle_, &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get SecondaryEDL state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

// Advanced Controls

Napi::Value TACDevWrapper::SetForcePSHoldHigh(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::ForcePSHoldHigh(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set ForcePSHoldHigh");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetForcePSHoldHighState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool state = false;
    TAC_RESULT result = ::GetForcePSHoldHighState(handle_, &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get ForcePSHoldHigh state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetSecondaryPmResinN(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::SecondaryPmResinN(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set SecondaryPmResinN");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetSecondaryPmResinNState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool state = false;
    TAC_RESULT result = ::GetSecondaryPmResinNState(handle_, &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get SecondaryPmResinN state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetEud(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::Eud(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set EUD");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetEUDState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool state = false;
    TAC_RESULT result = ::GetEUDState(handle_, &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get EUD state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SetHeadsetDisconnect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_RESULT result = ::HeadsetDisconnect(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set HeadsetDisconnect");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetHeadsetDisconnectState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool state = false;
    TAC_RESULT result = ::GetHeadsetDisconnectState(handle_, &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get HeadsetDisconnect state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

// Configuration

Napi::Value TACDevWrapper::SetExternalPowerControl(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsBoolean()) {
        Napi::TypeError::New(env, "Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    bool state = info[0].As<Napi::Boolean>().Value();
    TAC_ERROR result = ::SetExternalPowerControl(handle_, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set external power control");
    return env.Undefined();
}

Napi::Value TACDevWrapper::SetName(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected for device name").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    std::string name = info[0].As<Napi::String>().Utf8Value();
    TAC_RESULT result = ::SetName(handle_, name.c_str());
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set device name");
    return env.Undefined();
}

Napi::Value TACDevWrapper::GetResetCount(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    int count = 0;
    TAC_RESULT result = ::GetResetCount(handle_, &count);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get reset count");
        return env.Undefined();
    }
    return Napi::Number::New(env, count);
}

Napi::Value TACDevWrapper::ClearResetCount(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    TAC_RESULT result = ::ClearResetCount(handle_);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to clear reset count");
    return env.Undefined();
}

// Commands

Napi::Value TACDevWrapper::GetCommandCount(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    unsigned long count = 0;
    TAC_ERROR result = ::GetCommandCount(handle_, &count);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get command count");
        return env.Undefined();
    }
    return Napi::Number::New(env, count);
}

Napi::Value TACDevWrapper::GetCommand(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Number expected for command index").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    unsigned long index = info[0].As<Napi::Number>().Uint32Value();
    char buffer[512];
    TAC_ERROR result = ::GetCommand(handle_, index, buffer, sizeof(buffer));
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get command");
        return env.Undefined();
    }
    return ParseCommandMetadata(env, std::string(buffer));
}

Napi::Value TACDevWrapper::GetCommandState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String expected for command name").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    std::string command = info[0].As<Napi::String>().Utf8Value();
    bool state = false;
    TAC_ERROR result = ::GetCommandState(handle_, command.c_str(), &state);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get command state");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, state);
}

Napi::Value TACDevWrapper::SendCommand(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsBoolean()) {
        Napi::TypeError::New(env, "String and Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    std::string command = info[0].As<Napi::String>().Utf8Value();
    bool state = info[1].As<Napi::Boolean>().Value();
    TAC_ERROR result = ::SendCommand(handle_, command.c_str(), state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to send command");
    return env.Undefined();
}

// Quick Commands

Napi::Value TACDevWrapper::GetQuickCommandCount(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    unsigned long count = 0;
    TAC_ERROR result = ::GetQuickCommandCount(handle_, &count);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get quick command count");
        return env.Undefined();
    }
    return Napi::Number::New(env, count);
}

Napi::Value TACDevWrapper::GetQuickCommand(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Number expected for quick command index").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    unsigned long index = info[0].As<Napi::Number>().Uint32Value();
    char buffer[256];
    TAC_ERROR result = ::GetQuickCommand(handle_, index, buffer, sizeof(buffer));
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get quick command");
        return env.Undefined();
    }
    return Napi::String::New(env, buffer);
}

Napi::Value TACDevWrapper::PowerOnButton(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    TAC_RESULT result = ::PowerOnButton(handle_);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to execute PowerOn");
    return env.Undefined();
}

Napi::Value TACDevWrapper::PowerOffButton(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    TAC_RESULT result = ::PowerOffButton(handle_);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to execute PowerOff");
    return env.Undefined();
}

Napi::Value TACDevWrapper::BootToFastBootButton(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    TAC_RESULT result = ::BootToFastBootButton(handle_);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to boot to fastboot");
    return env.Undefined();
}

Napi::Value TACDevWrapper::BootToUEFIMenuButton(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    TAC_RESULT result = ::BootToUEFIMenuButton(handle_);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to boot to UEFI");
    return env.Undefined();
}

Napi::Value TACDevWrapper::BootToEDLButton(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    TAC_RESULT result = ::BootToEDLButton(handle_);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to boot to EDL");
    return env.Undefined();
}

Napi::Value TACDevWrapper::BootToSecondaryEDLButton(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    TAC_RESULT result = ::BootToSecondaryEDLButton(handle_);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to boot to secondary EDL");
    return env.Undefined();
}

// Script Variables

Napi::Value TACDevWrapper::GetScriptVariableCount(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    unsigned long count = 0;
    TAC_ERROR result = ::GetScriptVariableCount(handle_, &count);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get script variable count");
        return env.Undefined();
    }
    return Napi::Number::New(env, count);
}

Napi::Value TACDevWrapper::GetScriptVariable(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Number expected for variable index").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    unsigned long index = info[0].As<Napi::Number>().Uint32Value();
    char buffer[512];
    TAC_ERROR result = ::GetScriptVariable(handle_, index, buffer, sizeof(buffer));
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to get script variable");
        return env.Undefined();
    }
    return Napi::String::New(env, buffer);
}

Napi::Value TACDevWrapper::UpdateScriptVariableValue(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "Two strings expected (variable, value)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    std::string variable = info[0].As<Napi::String>().Utf8Value();
    std::string value = info[1].As<Napi::String>().Utf8Value();
    TAC_ERROR result = ::UpdateScriptVariableValue(handle_, variable.c_str(), value.c_str());
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to update script variable");
    return env.Undefined();
}

// Advanced

Napi::Value TACDevWrapper::SetPinState(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsBoolean()) {
        Napi::TypeError::New(env, "Number and Boolean expected").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    int pin = info[0].As<Napi::Number>().Int32Value();
    bool state = info[1].As<Napi::Boolean>().Value();
    TAC_ERROR result = ::SetPinState(handle_, pin, state);
    if (result != NO_TAC_ERROR) ThrowTACError(env, result, "Failed to set pin state");
    return env.Undefined();
}

Napi::Value TACDevWrapper::IsCommandQueueClear(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);
    bool status = false;
    TAC_ERROR result = ::IsCommandQueueClear(handle_, &status);
    if (result != NO_TAC_ERROR) {
        ThrowTACError(env, result, "Failed to check command queue");
        return env.Undefined();
    }
    return Napi::Boolean::New(env, status);
}

Napi::Value TACDevWrapper::GetHelpText(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    ValidateHandle(env, handle_);

    int actualSize = 0;
    char buffer[4096];
    TAC_ERROR result = ::GetHelpText(handle_, buffer, sizeof(buffer), &actualSize);

    if (result == TACDEV_BUFFER_TOO_SMALL && actualSize > 0) {
        std::vector<char> largeBuffer(actualSize);
        result = ::GetHelpText(handle_, largeBuffer.data(), actualSize, &actualSize);
        if (result == NO_TAC_ERROR) {
            return Napi::String::New(env, largeBuffer.data());
        }
    } else if (result == NO_TAC_ERROR) {
        return Napi::String::New(env, buffer);
    }

    ThrowTACError(env, result, "Failed to get help text");
    return env.Undefined();
}

} // namespace qtac_napi
