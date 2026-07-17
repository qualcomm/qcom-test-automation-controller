// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// taccl - command-line interface for TACDev.
//
// This entry point wires up global options and delegates subcommand
// registration to the section-specific modules (currently only Section 5,
// State Controls). Other sections can be added by calling their own
// register*() functions here.

#include <cstdlib>
#include <iostream>
#include <string>

#ifdef _WIN32
#  include <crtdbg.h>
#endif

#include <CLI/CLI.hpp>

#include "TACDev.h"

#include "Errors.h"
#include "Output.h"
#include "StateControls.h"

namespace {

constexpr const char* kVersion = "taccl 0.1.0 (Section 5 - State Controls)";

std::string tacVersionString()
{
    char buffer[256]{};
    if (GetTACVersion(buffer, static_cast<int>(sizeof(buffer))) == NO_TAC_ERROR)
        return std::string(buffer);
    return "unknown";
}

std::string alpacaVersionString()
{
    char buffer[256]{};
    if (GetAlpacaVersion(buffer, static_cast<int>(sizeof(buffer))) == NO_TAC_ERROR)
        return std::string(buffer);
    return "unknown";
}

} // namespace

int main(int argc, char** argv)
{
#ifdef _WIN32
    // Suppress the MSVC debug CRT "Abort/Retry/Ignore" dialog.
    // Without this, any abort() call (e.g. from Qt assertions or CRT errors)
    // pops an interactive dialog and blocks automated test runs.
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR,  _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_WARN,   _CRTDBG_MODE_DEBUG);
#endif

    // ---------------------------------------------------------------------
    // Initialize TACDev up front. All subcommands assume this has succeeded.
    // ---------------------------------------------------------------------
    if (InitializeTACDev() != NO_TAC_ERROR) {
        std::cerr << "taccl: failed to initialize TACDev" << std::endl;
        return taccl::ExitCode::InitFailed;
    }

    // ---------------------------------------------------------------------
    // CLI11 app + global options
    // ---------------------------------------------------------------------
    CLI::App app{"taccl - command-line interface for TAC devices"};
    app.require_subcommand(1);
    // Let global options (e.g. --json, --quiet) work when placed after a
    // subcommand: `taccl battery --device=COM3 --json`.
    app.fallthrough();
    app.set_version_flag("--version", []() {
        return std::string(kVersion) +
               "\nQTAC: "  + alpacaVersionString() +
               "\nTAC:  "  + tacVersionString();
    });

    taccl::OutputOptions output;
    app.add_flag("-j,--json",  output.json,  "Output in JSON format");
    app.add_flag("-q,--quiet", output.quiet, "Suppress non-essential output");

    // Honour TACCL_JSON=1 as an environment default.
    if (const char* env = std::getenv("TACCL_JSON")) {
        if (std::string(env) == "1")
            output.json = true;
    }

    // ---------------------------------------------------------------------
    // Subcommand registration
    // ---------------------------------------------------------------------
    int exitCode = taccl::ExitCode::Success;

    taccl::registerStateControls(app, output, exitCode);

    // ---------------------------------------------------------------------
    // Parse & run
    // ---------------------------------------------------------------------
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError& e) {
        // CLI11 handles --help / --version internally and returns non-zero.
        // Any parse error is a usage error.
        return app.exit(e);
    }

    return exitCode;
}