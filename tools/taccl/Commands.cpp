// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 7 – Dynamic Commands

#include "Commands.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

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
// taccl commands
// ---------------------------------------------------------------------------

struct CommandsOptions {
    std::string device;
    std::string serial;
};

void runCommands(const CommandsOptions& opts, const OutputOptions& out, int& exitCode)
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

    unsigned long count = 0;
    if (GetCommandCount(dev.handle(), &count) != NO_TAC_ERROR) {
        printError("commands query failed: " + lastTacError(), out);
        exitCode = ExitCode::TacCommandFail;
        return;
    }

    std::vector<std::string> cmds;
    cmds.reserve(count);
    for (unsigned long i = 0; i < count; ++i) {
        char buf[1024]{};
        if (GetCommand(dev.handle(), i, buf, static_cast<int>(sizeof(buf))) != NO_TAC_ERROR) {
            printError("commands query failed at index " + std::to_string(i) +
                       ": " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        cmds.emplace_back(buf);
    }

    if (out.quiet)
        return;

    if (out.json) {
        nlohmann::json j = cmds;
        std::cout << j.dump() << "\n";
    } else {
        for (const auto& c : cmds)
            std::cout << c << "\n";
    }
}

// ---------------------------------------------------------------------------
// taccl command
// ---------------------------------------------------------------------------

struct CommandOptions {
    std::string device;
    std::string serial;
    std::string name;
    std::string state; // "on" | "off" | "" (query)
};

void runCommand(const CommandOptions& opts, const OutputOptions& out, int& exitCode)
{
    std::string port = resolvePortName(opts.device, opts.serial);
    if (port.empty()) {
        printError("no device specified (use --device / --serial or "
                   "TACCL_DEVICE / TACCL_SERIAL)", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    if (opts.name.empty()) {
        printError("--name is required", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    Device dev(port);
    if (!dev.isValid()) {
        printError("device '" + port + "' not found", out);
        exitCode = ExitCode::DeviceNotFound;
        return;
    }

    if (opts.state.empty()) {
        // Query
        bool st = false;
        if (GetCommandState(dev.handle(), opts.name.c_str(), &st) != NO_TAC_ERROR) {
            printError("command '" + opts.name + "' query failed: " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        printState(st, out);
    } else {
        // Set
        bool on = (opts.state == "on");
        if (SendCommand(dev.handle(), opts.name.c_str(), on) != NO_TAC_ERROR) {
            printError("command '" + opts.name + "' failed: " + lastTacError(), out);
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

void registerCommands(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<CommandsOptions>();

    auto* sub = app.add_subcommand("commands", "List available commands on the device");
    sub->add_option("-d,--device", opts->device, "Device port name");
    sub->add_option("-s,--serial", opts->serial, "Device serial number");

    sub->callback([&, opts]() {
        runCommands(*opts, globalOutput, exitCode);
    });
}

void registerCommand(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<CommandOptions>();

    auto* sub = app.add_subcommand("command", "Query or set a dynamic command state");
    sub->add_option("-d,--device", opts->device, "Device port name");
    sub->add_option("-s,--serial", opts->serial, "Device serial number");
    sub->add_option("-n,--name",   opts->name,   "Command name")->required();
    sub->add_option("--state",     opts->state,  "State to set: on or off (omit to query)")
       ->check(CLI::IsMember({"on", "off"}));

    sub->callback([&, opts]() {
        runCommand(*opts, globalOutput, exitCode);
    });
}

} // namespace taccl
