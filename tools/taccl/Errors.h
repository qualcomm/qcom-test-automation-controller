// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Exit codes and small error helpers for taccl.
// Mirrors the codes documented in docs/taccl-plan.md.

#pragma once

namespace taccl {

enum ExitCode : int {
    Success        = 0,
    GeneralError   = 1,
    DeviceNotFound = 2,
    UsageError     = 3,
    TacCommandFail = 4,
    InitFailed     = 5,
};

} // namespace taccl