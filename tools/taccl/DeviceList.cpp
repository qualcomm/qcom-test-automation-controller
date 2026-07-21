// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Sections 1-4: list, version, info, set

#include "DeviceList.h"

#include <iomanip>
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
// Section 1 – list
// ---------------------------------------------------------------------------

void runList(const OutputOptions& out, int& exitCode)
{
    auto devices = enumerateDevices();

    if (out.quiet)
        return;

    if (out.json) {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& d : devices) {
            j.push_back({{"port", d.port}, {"serial", d.serial}});
        }
        std::cout << j.dump() << "\n";
    } else {
        if (devices.empty()) {
            std::cout << "(no TAC devices found)\n";
            return;
        }
        // Fixed-width columns: PORT (8) + SERIAL
        std::cout << std::left;
        std::cout << "PORT     SERIAL\n";
        for (const auto& d : devices)
            std::cout << std::left << std::setw(9) << d.port << d.serial << "\n";
    }
}

// ---------------------------------------------------------------------------
// Section 2 – version subcommand
// ---------------------------------------------------------------------------

void runVersion(const OutputOptions& out)
{
    char alpacaBuf[256]{};
    char tacBuf[256]{};
    std::string alpaca = (GetAlpacaVersion(alpacaBuf, sizeof(alpacaBuf)) == NO_TAC_ERROR)
                         ? alpacaBuf : "unknown";
    std::string tac    = (GetTACVersion(tacBuf, sizeof(tacBuf)) == NO_TAC_ERROR)
                         ? tacBuf : "unknown";

    if (out.quiet)
        return;

    if (out.json) {
        nlohmann::json j = {{"qtac", alpaca}, {"tac", tac}};
        std::cout << j.dump() << "\n";
    } else {
        std::cout << "QTAC: " << alpaca << "\n";
        std::cout << "TAC:  " << tac    << "\n";
    }
}

// ---------------------------------------------------------------------------
// Section 3 – info
// ---------------------------------------------------------------------------

struct InfoOptions {
    std::string device;
    std::string serial;
    bool showName            = false;
    bool showFirmware        = false;
    bool showHardware        = false;
    bool showHardwareVersion = false;
    bool showUUID            = false;
    bool showResetCount      = false;
};

void runInfo(const InfoOptions& opts, const OutputOptions& out, int& exitCode)
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

    // Determine which fields to show (all by default if no filter flags set)
    bool any = opts.showName || opts.showFirmware || opts.showHardware ||
               opts.showHardwareVersion || opts.showUUID || opts.showResetCount;
    bool showAll = !any;

    auto fetchStr = [&](auto fn, const char* label) -> std::string {
        char buf[512]{};
        if (fn(dev.handle(), buf, static_cast<int>(sizeof(buf))) == NO_TAC_ERROR)
            return std::string(buf);
        return "";
    };

    std::string name, firmware, hardware, hwVersion, uuid;
    int resetCount = 0;

    if (showAll || opts.showName)
        name = fetchStr(GetName, "name");
    if (showAll || opts.showFirmware)
        firmware = fetchStr(GetFirmwareVersion, "firmware");
    if (showAll || opts.showHardware)
        hardware = fetchStr(GetHardware, "hardware");
    if (showAll || opts.showHardwareVersion)
        hwVersion = fetchStr(GetHardwareVersion, "hardware-version");
    if (showAll || opts.showUUID)
        uuid = fetchStr(GetUUID, "uuid");
    if (showAll || opts.showResetCount) {
        GetResetCount(dev.handle(), &resetCount);
    }

    if (out.quiet)
        return;

    if (out.json) {
        nlohmann::json j = nlohmann::json::object();
        if (showAll || opts.showName)            j["name"]            = name;
        if (showAll || opts.showFirmware)        j["firmware"]        = firmware;
        if (showAll || opts.showHardware)        j["hardware"]        = hardware;
        if (showAll || opts.showHardwareVersion) j["hardwareVersion"] = hwVersion;
        if (showAll || opts.showUUID)            j["uuid"]            = uuid;
        if (showAll || opts.showResetCount)      j["resetCount"]      = resetCount;
        std::cout << j.dump() << "\n";
    } else {
        if (showAll || opts.showName)            std::cout << "name: "             << name      << "\n";
        if (showAll || opts.showFirmware)        std::cout << "firmware: "         << firmware  << "\n";
        if (showAll || opts.showHardware)        std::cout << "hardware: "         << hardware  << "\n";
        if (showAll || opts.showHardwareVersion) std::cout << "hardware-version: " << hwVersion << "\n";
        if (showAll || opts.showUUID)            std::cout << "uuid: "             << uuid      << "\n";
        if (showAll || opts.showResetCount)      std::cout << "reset-count: "      << resetCount << "\n";
    }
}

// ---------------------------------------------------------------------------
// Section 4 – set
// ---------------------------------------------------------------------------

struct SetOptions {
    std::string device;
    std::string serial;
    std::string newName;
    bool clearResetCount = false;
};

void runSet(const SetOptions& opts, const OutputOptions& out, int& exitCode)
{
    std::string port = resolvePortName(opts.device, opts.serial);
    if (port.empty()) {
        printError("no device specified (use --device / --serial or "
                   "TACCL_DEVICE / TACCL_SERIAL)", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    if (opts.newName.empty() && !opts.clearResetCount) {
        printError("no change specified (use --name or --clear-reset-count)", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    Device dev(port);
    if (!dev.isValid()) {
        printError("device '" + port + "' not found", out);
        exitCode = ExitCode::DeviceNotFound;
        return;
    }

    if (!opts.newName.empty()) {
        if (SetName(dev.handle(), opts.newName.c_str()) != NO_TAC_ERROR) {
            printError("set name failed: " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        if (!out.quiet) {
            if (out.json)
                std::cout << "{\"name\":\"" << opts.newName << "\"}\n";
            else
                std::cout << "name: " << opts.newName << "\n";
        }
    }

    if (opts.clearResetCount) {
        if (ClearResetCount(dev.handle()) != NO_TAC_ERROR) {
            printError("clear reset count failed: " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        if (!out.quiet) {
            if (out.json)
                std::cout << "{\"resetCount\":0}\n";
            else
                std::cout << "reset-count: 0\n";
        }
    }
}

} // namespace

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

void registerList(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto* sub = app.add_subcommand("list", "List all connected TAC devices");
    sub->callback([&]() {
        runList(globalOutput, exitCode);
    });
}

void registerVersion(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto* sub = app.add_subcommand("version", "Show QTAC and TAC library versions");
    sub->callback([&]() {
        runVersion(globalOutput);
    });
}

void registerInfo(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<InfoOptions>();

    auto* sub = app.add_subcommand("info", "Show device information");
    sub->add_option("-d,--device",  opts->device, "Device port name");
    sub->add_option("-s,--serial",  opts->serial, "Device serial number");
    sub->add_flag("--name",             opts->showName,            "Show device name");
    sub->add_flag("--firmware",         opts->showFirmware,        "Show firmware version");
    sub->add_flag("--hardware",         opts->showHardware,        "Show hardware type");
    sub->add_flag("--hardware-version", opts->showHardwareVersion, "Show hardware version");
    sub->add_flag("--uuid",             opts->showUUID,            "Show UUID");
    sub->add_flag("--reset-count",      opts->showResetCount,      "Show reset count");

    sub->callback([&, opts]() {
        runInfo(*opts, globalOutput, exitCode);
    });
}

void registerSet(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<SetOptions>();

    auto* sub = app.add_subcommand("set", "Configure device settings");
    sub->add_option("-d,--device",         opts->device,         "Device port name");
    sub->add_option("-s,--serial",         opts->serial,         "Device serial number");
    sub->add_option("--name",              opts->newName,         "Set device name");
    sub->add_flag("--clear-reset-count",   opts->clearResetCount, "Clear the reset counter");

    sub->callback([&, opts]() {
        runSet(*opts, globalOutput, exitCode);
    });
}

} // namespace taccl
