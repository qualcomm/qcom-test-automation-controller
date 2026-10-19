// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 6 – Boot Sequences

#include "BootSequences.h"

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

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

struct BootOptions {
    std::string device;
    std::string serial;
    std::string mode;
};

using BootFn = TAC_RESULT (*)(TAC_HANDLE);

const std::unordered_map<std::string, BootFn> kBootModes = {
    {"power-on",       PowerOnButton},
    {"power-off",      PowerOffButton},
    {"fastboot",       BootToFastBootButton},
    {"uefi",           BootToUEFIMenuButton},
    {"edl",            BootToEDLButton},
    {"secondary-edl",  BootToSecondaryEDLButton},
};

void runBoot(const BootOptions& opts, const OutputOptions& out, int& exitCode)
{
    std::string port = resolvePortName(opts.device, opts.serial);
    if (port.empty()) {
        printError("no device specified (use --device / --serial or "
                   "TACCL_DEVICE / TACCL_SERIAL)", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    if (opts.mode.empty()) {
        printError("--mode is required", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    auto it = kBootModes.find(opts.mode);
    if (it == kBootModes.end()) {
        printError("unknown boot mode '" + opts.mode +
                   "' (use power-on, power-off, fastboot, uefi, edl, secondary-edl)", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    Device dev(port);
    if (!dev.isValid()) {
        printError("device '" + port + "' not found", out);
        exitCode = ExitCode::DeviceNotFound;
        return;
    }

    if (it->second(dev.handle()) != NO_TAC_ERROR) {
        printError("boot '" + opts.mode + "' failed: " + lastTacError(), out);
        exitCode = ExitCode::TacCommandFail;
        return;
    }

    if (!out.quiet) {
        if (out.json)
            std::cout << "{\"mode\":\"" << opts.mode << "\"}\n";
        else
            std::cout << opts.mode << "\n";
    }
}

} // namespace

void registerBoot(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<BootOptions>();

    auto* sub = app.add_subcommand("boot", "Trigger a boot sequence on the device");
    sub->add_option("-d,--device", opts->device, "Device port name");
    sub->add_option("-s,--serial", opts->serial, "Device serial number");
    sub->add_option("-m,--mode",   opts->mode,
                   "Boot mode: power-on, power-off, fastboot, uefi, edl, secondary-edl")
       ->required();

    sub->callback([&, opts]() {
        runBoot(*opts, globalOutput, exitCode);
    });
}

} // namespace taccl
