// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 8 – Script Variables (`taccl vars`, `taccl var`)

#pragma once

#include <CLI/CLI.hpp>
#include "Output.h"

namespace taccl {

// taccl vars --device=<port>                            List all variables
void registerVars(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

// taccl var --device=<port> --name=<var> [--value=<v>] Get / set a variable
void registerVar(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

} // namespace taccl
