// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Human-readable and JSON output helpers for taccl.

#pragma once

#include <string>

namespace taccl {

struct OutputOptions {
    bool json  = false;
    bool quiet = false;
};

// Print a boolean state as either "on"/"off" or {"state":true/false}.
void printState(bool state, const OutputOptions& opts);

// Emit an error message to stderr. When opts.json is set, produces a JSON
// object of the form {"error":"..."}; otherwise "taccl: <message>".
void printError(const std::string& message, const OutputOptions& opts);

} // namespace taccl