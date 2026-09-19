// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 1 – Device Discovery (`taccl list`)
// Section 2 – Version subcommand (`taccl version`)
// Section 3 – Device Information (`taccl info`)
// Section 4 – Device Configuration (`taccl set`)

#pragma once

#include <CLI/CLI.hpp>
#include "Output.h"

namespace taccl {

// Section 1: taccl list
void registerList(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

// Section 2: taccl version (subcommand form)
void registerVersion(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

// Section 3: taccl info
void registerInfo(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

// Section 4: taccl set
void registerSet(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

} // namespace taccl
