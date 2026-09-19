// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 6 – Boot Sequences (`taccl boot --mode=<mode>`)

#pragma once

#include <CLI/CLI.hpp>
#include "Output.h"

namespace taccl {

void registerBoot(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

} // namespace taccl
