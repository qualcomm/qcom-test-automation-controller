// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.

#include "QuickCommands.h"

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

struct QuickCommandsOptions {
    std::string device;
    std::string serial;
};

void runQuickCommands(const QuickCommandsOptions& opts,
                      const OutputOptions&        out,
                      int&                        exitCode)
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
    if (GetQuickCommandCount(dev.handle(), &count) != NO_TAC_ERROR) {
        printError("quick-commands query failed: " + lastTacError(), out);
        exitCode = ExitCode::TacCommandFail;
        return;
    }

    std::vector<std::string> commands;
    commands.reserve(count);
    for (unsigned long i = 0; i < count; ++i) {
        char buffer[1024]{};
        if (GetQuickCommand(dev.handle(), i, buffer,
                            static_cast<int>(sizeof(buffer))) != NO_TAC_ERROR) {
            printError("quick-commands query failed at index " +
                       std::to_string(i) + ": " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        commands.emplace_back(buffer);
    }

    if (out.quiet)
        return;

    if (out.json) {
        nlohmann::json j = commands;
        std::cout << j.dump() << std::endl;
    } else {
        for (const auto& cmd : commands)
            std::cout << cmd << "\n";
    }
}

} // namespace

void registerQuickCommands(CLI::App&      app,
                           OutputOptions& globalOutput,
                           int&           exitCode)
{
    auto opts = std::make_shared<QuickCommandsOptions>();

    auto* sub = app.add_subcommand("quick-commands",
                                   "List quick commands available on the device");
    sub->add_option("-d,--device", opts->device, "Device port name");
    sub->add_option("-s,--serial", opts->serial, "Device serial number");

    sub->callback([&, opts]() {
        runQuickCommands(*opts, globalOutput, exitCode);
    });
}

} // namespace taccl
