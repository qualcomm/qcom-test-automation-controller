// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 9 – Utility subcommands:
//   taccl help-text   – retrieve device help text
//   taccl queue-clear – check if command queue is clear
//   taccl logging     – get/set logging state

#pragma once

#include <CLI/CLI.hpp>
#include "Output.h"

namespace taccl {

void registerHelpText(CLI::App& app, OutputOptions& globalOutput, int& exitCode);
void registerQueueClear(CLI::App& app, OutputOptions& globalOutput, int& exitCode);
void registerLogging(CLI::App& app, OutputOptions& globalOutput, int& exitCode);

} // namespace taccl
