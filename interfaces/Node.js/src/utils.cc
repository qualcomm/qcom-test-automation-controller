// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "utils.h"
#include <sstream>
#include <vector>

namespace qtac_napi {

std::string GetLastErrorMessage() {
    char errorBuffer[512];
    TAC_ERROR result = GetLastTACError(errorBuffer, sizeof(errorBuffer));
    if (result == NO_TAC_ERROR) {
        return std::string(errorBuffer);
    }
    return "Unknown error";
}

void ThrowTACError(const Napi::Env& env, TAC_ERROR error, const std::string& context) {
    if (error == NO_TAC_ERROR) {
        return;
    }

    std::string errorMsg = GetLastErrorMessage();
    std::string fullMessage;

    if (!context.empty()) {
        fullMessage = context + ": " + errorMsg + " (Error code: " + std::to_string(error) + ")";
    } else {
        fullMessage = errorMsg + " (Error code: " + std::to_string(error) + ")";
    }

    switch (error) {
        case TACDEV_BAD_TAC_HANDLE:
            Napi::TypeError::New(env, fullMessage).ThrowAsJavaScriptException();
            break;
        case TACDEV_BAD_INDEX:
            Napi::RangeError::New(env, fullMessage).ThrowAsJavaScriptException();
            break;
        case TACDEV_COMMAND_NOT_FOUND:
        case TACDEV_SCRIPT_VARIABLE_NOT_FOUND:
            Napi::Error::New(env, fullMessage).ThrowAsJavaScriptException();
            break;
        case TACDEV_INIT_FAILED:
            Napi::Error::New(env, fullMessage).ThrowAsJavaScriptException();
            break;
        default:
            Napi::Error::New(env, fullMessage).ThrowAsJavaScriptException();
    }
}

std::string CallStringFunction(
    std::function<TAC_ERROR(char*, int)> func,
    int initialBufferSize
) {
    std::vector<char> buffer(initialBufferSize);
    TAC_ERROR result = func(buffer.data(), static_cast<int>(buffer.size()));

    if (result == TACDEV_BUFFER_TOO_SMALL) {
        // Retry with larger buffer
        buffer.resize(initialBufferSize * 2);
        result = func(buffer.data(), static_cast<int>(buffer.size()));
    }

    if (result != NO_TAC_ERROR) {
        return "";
    }

    return std::string(buffer.data());
}

std::string CallStringFunctionWithHandle(
    TAC_HANDLE handle,
    std::function<TAC_ERROR(TAC_HANDLE, char*, int)> func,
    int initialBufferSize
) {
    std::vector<char> buffer(initialBufferSize);
    TAC_ERROR result = func(handle, buffer.data(), static_cast<int>(buffer.size()));

    if (result == TACDEV_BUFFER_TOO_SMALL) {
        // Retry with larger buffer
        buffer.resize(initialBufferSize * 2);
        result = func(handle, buffer.data(), static_cast<int>(buffer.size()));
    }

    if (result != NO_TAC_ERROR) {
        return "";
    }

    return std::string(buffer.data());
}

Napi::Object ParseDeviceInfo(const Napi::Env& env, const std::string& data) {
    Napi::Object obj = Napi::Object::New(env);

    std::istringstream ss(data);
    std::string token;
    std::vector<std::string> parts;

    while (std::getline(ss, token, ';')) {
        parts.push_back(token);
    }

    if (parts.size() >= 4) {
        obj.Set("port", Napi::String::New(env, parts[0]));
        obj.Set("description", Napi::String::New(env, parts[1]));
        obj.Set("serialNumber", Napi::String::New(env, parts[2]));
        obj.Set("index", Napi::Number::New(env, std::stoi(parts[3])));
    }

    return obj;
}

Napi::Object ParseCommandMetadata(const Napi::Env& env, const std::string& data) {
    Napi::Object obj = Napi::Object::New(env);

    std::istringstream ss(data);
    std::string token;
    std::vector<std::string> parts;

    while (std::getline(ss, token, ';')) {
        parts.push_back(token);
    }

    if (parts.size() >= 6) {
        obj.Set("command", Napi::String::New(env, parts[0]));
        obj.Set("helpText", Napi::String::New(env, parts[1]));

        try {
            obj.Set("pin", Napi::Number::New(env, std::stoi(parts[2])));
        } catch (...) {
            obj.Set("pin", Napi::Number::New(env, -1));
        }

        obj.Set("tabName", Napi::String::New(env, parts[3]));
        obj.Set("groupName", Napi::String::New(env, parts[4]));
        obj.Set("cellLocation", Napi::String::New(env, parts[5]));
    }

    return obj;
}

void ValidateHandle(const Napi::Env& env, TAC_HANDLE handle) {
    if (handle == kBadHandle || handle == 0) {
        Napi::TypeError::New(env, "Invalid or closed device handle").ThrowAsJavaScriptException();
    }
}

} // namespace qtac_napi
