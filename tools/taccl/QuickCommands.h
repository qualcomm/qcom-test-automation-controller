// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 7 (partial) of docs/taccl-plan.md - Quick Commands.
//
// Registers the `taccl quick-commands` subcommand, which lists the quick
// commands available on a TAC device via GetQuickCommandCount / GetQuickCommand.

#pragma once

namespace CLI { class App; }

namespace taccl {

struct OutputOptions;

// Register the `quick-commands` subcommand under `app`.
void registerQuickCommands(CLI::App& app,
                           OutputOptions& globalOutput,
                           int& exitCode);

} // namespace taccl
