// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include <napi.h>
#include "tacdev_wrapper.h"

// Module initialization
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    // Initialize the TACDevice wrapper class
    qtac_napi::TACDevWrapper::Init(env, exports);

    // Store constructor reference for later use
    Napi::Function constructor = exports.Get("TACDevice").As<Napi::Function>();
    auto* ref = new Napi::FunctionReference();
    *ref = Napi::Persistent(constructor);
    env.SetInstanceData(ref);

    return exports;
}

// Define the module
NODE_API_MODULE(qtac, Init)
