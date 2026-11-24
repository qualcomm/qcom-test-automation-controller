// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 5 of docs/taccl-plan.md - State Controls.
//
// Each state-control subcommand queries or sets an on/off state on a TAC
// device. The mapping between CLI name and the pair of TACDev getter/setter
// functions is defined in StateControls.cpp as a table so that the same
// dispatch code services every command.

#pragma once

#include <string>

#include "TACDev.h"

namespace CLI { class App; }

namespace taccl {

struct OutputOptions;

// Function-pointer aliases matching the TACDev C ABI.
using StateGetter = TAC_RESULT (*)(TAC_HANDLE, bool*);
using StateSetter = TAC_RESULT (*)(TAC_HANDLE, bool);

struct StateControl {
    const char*  name;         // CLI subcommand, e.g. "battery"
    const char*  description;  // shown in --help
    StateGetter  get;          // may be nullptr for set-only controls
    StateSetter  set;          // may be nullptr for get-only (none currently)
};

// Options bag populated by CLI11 for every state-control subcommand.
struct StateControlOptions {
    std::string  device;
    std::string  serial;
    std::string  stateStr;   // "on" or "off"; empty when omitted
    bool         stateProvided = false;
};

// Register all Section 5 subcommands under `app`. Each subcommand is wired
// up with its own StateControlOptions and shares the global OutputOptions.
void registerStateControls(CLI::App& app,
                           OutputOptions& globalOutput,
                           int& exitCode);

} // namespace taccl