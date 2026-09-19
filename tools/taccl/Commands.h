// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 7 – Dynamic Commands (`taccl commands`, `taccl command`)

#pragma once

#include <CLI/CLI.hpp>
#include "Output.h"

namespace taccl {

// taccl commands --device=<port>           (list all commands)
void registerCommands(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

// taccl command --device=<port> --name=<cmd> [--state=on|off]
void registerCommand(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

} // namespace taccl
