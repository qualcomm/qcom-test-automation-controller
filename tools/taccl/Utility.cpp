// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 9 – Utility subcommands

#include "Utility.h"

#include <iostream>
#include <memory>
#include <string>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "Device.h"
#include "Errors.h"
#include "Output.h"
#include "TACDev.h"

namespace taccl {

namespace {

std::string lastTacError()
{
    char buffer[512]{};
    if (GetLastTACError(buffer, static_cast<int>(sizeof(buffer))) == NO_TAC_ERROR)
        return std::string(buffer);
    return "unknown TAC error";
}

// ---------------------------------------------------------------------------
// help-text
// ---------------------------------------------------------------------------

struct HelpTextOptions {
    std::string device;
    std::string serial;
};

void runHelpText(const HelpTextOptions& opts, const OutputOptions& out, int& exitCode)
{
    std::string port = resolvePortName(opts.device, opts.serial);
    if (port.empty()) {
        printError("no device specified (use --device / --serial or "
                   "TACCL_DEVICE / TACCL_SERIAL)", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    Device dev(port);
    if (!dev.isValid()) {
        printError("device '" + port + "' not found", out);
        exitCode = ExitCode::DeviceNotFound;
        return;
    }

    // Query actual size first so we can allocate a large-enough buffer.
    int actualSize = 0;
    {
        char dummy[1]{};
        GetHelpText(dev.handle(), dummy, 1, &actualSize);
    }

    std::string text(static_cast<std::size_t>(actualSize + 1), '\0');
    if (GetHelpText(dev.handle(), &text[0],
                    static_cast<int>(text.size()), &actualSize) != NO_TAC_ERROR) {
        printError("help-text query failed: " + lastTacError(), out);
        exitCode = ExitCode::TacCommandFail;
        return;
    }
    text.resize(static_cast<std::size_t>(actualSize));

    if (out.quiet)
        return;

    if (out.json) {
        nlohmann::json j = {{"text", text}};
        std::cout << j.dump() << "\n";
    } else {
        std::cout << text << "\n";
    }
}

// ---------------------------------------------------------------------------
// queue-clear
// ---------------------------------------------------------------------------

struct QueueClearOptions {
    std::string device;
    std::string serial;
};

void runQueueClear(const QueueClearOptions& opts, const OutputOptions& out, int& exitCode)
{
    std::string port = resolvePortName(opts.device, opts.serial);
    if (port.empty()) {
        printError("no device specified (use --device / --serial or "
                   "TACCL_DEVICE / TACCL_SERIAL)", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    Device dev(port);
    if (!dev.isValid()) {
        printError("device '" + port + "' not found", out);
        exitCode = ExitCode::DeviceNotFound;
        return;
    }

    bool clear = false;
    if (IsCommandQueueClear(dev.handle(), &clear) != NO_TAC_ERROR) {
        printError("queue-clear query failed: " + lastTacError(), out);
        exitCode = ExitCode::TacCommandFail;
        return;
    }

    printState(clear, out);
}

// ---------------------------------------------------------------------------
// logging
// ---------------------------------------------------------------------------

struct LoggingOptions {
    std::string state; // "on" | "off" | "" = query
    bool hasState = false;
};

void runLogging(const LoggingOptions& opts, const OutputOptions& out, int& exitCode)
{
    if (!opts.hasState) {
        // Query
        bool st = false;
        if (GetLoggingState(&st) != NO_TAC_ERROR) {
            printError("logging query failed: " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        printState(st, out);
    } else {
        bool on = (opts.state == "on");
        if (SetLoggingState(on) != NO_TAC_ERROR) {
            printError("set logging failed: " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        printState(on, out);
    }
}

} // namespace

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

void registerHelpText(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<HelpTextOptions>();

    auto* sub = app.add_subcommand("help-text", "Retrieve device help text");
    sub->add_option("-d,--device", opts->device, "Device port name");
    sub->add_option("-s,--serial", opts->serial, "Device serial number");

    sub->callback([&, opts]() {
        runHelpText(*opts, globalOutput, exitCode);
    });
}

void registerQueueClear(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<QueueClearOptions>();

    auto* sub = app.add_subcommand("queue-clear", "Check if command queue is clear");
    sub->add_option("-d,--device", opts->device, "Device port name");
    sub->add_option("-s,--serial", opts->serial, "Device serial number");

    sub->callback([&, opts]() {
        runQueueClear(*opts, globalOutput, exitCode);
    });
}

void registerLogging(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<LoggingOptions>();

    auto* sub = app.add_subcommand("logging",
                                   "Get or set TACDev logging state");
    auto* stateOpt = sub->add_option("--state", opts->state,
                                     "State to set: on or off (omit to query)")
                        ->check(CLI::IsMember({"on", "off"}));

    sub->callback([&, opts, stateOpt]() {
        opts->hasState = stateOpt->count() > 0;
        runLogging(*opts, globalOutput, exitCode);
    });
}

} // namespace taccl
