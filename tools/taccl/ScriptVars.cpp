// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Section 8 – Script Variables

#include "ScriptVars.h"

#include <iostream>
#include <memory>
#include <sstream>
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

// Script variable format from GetScriptVariable is semicolon-separated:
// "name;label;description;type;defaultValue;position"
// We extract name (index 0) and default value (index 4).
struct ScriptVar {
    std::string name;
    std::string value;
};

ScriptVar parseVar(const std::string& raw)
{
    std::stringstream ss(raw);
    std::string token;
    ScriptVar v;
    int i = 0;
    while (std::getline(ss, token, ';')) {
        if (i == 0) v.name  = token;
        if (i == 4) v.value = token;
        ++i;
    }
    if (v.name.empty()) v.name = raw; // fallback
    return v;
}

// ---------------------------------------------------------------------------
// taccl vars
// ---------------------------------------------------------------------------

struct VarsOptions {
    std::string device;
    std::string serial;
};

void runVars(const VarsOptions& opts, const OutputOptions& out, int& exitCode)
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
    if (GetScriptVariableCount(dev.handle(), &count) != NO_TAC_ERROR) {
        printError("vars query failed: " + lastTacError(), out);
        exitCode = ExitCode::TacCommandFail;
        return;
    }

    std::vector<ScriptVar> vars;
    vars.reserve(count);
    for (unsigned long i = 0; i < count; ++i) {
        char buf[1024]{};
        if (GetScriptVariable(dev.handle(), i, buf,
                              static_cast<int>(sizeof(buf))) != NO_TAC_ERROR) {
            printError("vars query failed at index " + std::to_string(i) +
                       ": " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        vars.push_back(parseVar(buf));
    }

    if (out.quiet)
        return;

    if (out.json) {
        nlohmann::json j = nlohmann::json::object();
        for (const auto& v : vars)
            j[v.name] = v.value;
        std::cout << j.dump() << "\n";
    } else {
        for (const auto& v : vars)
            std::cout << v.name << "=" << v.value << "\n";
    }
}

// ---------------------------------------------------------------------------
// taccl var
// ---------------------------------------------------------------------------

struct VarOptions {
    std::string device;
    std::string serial;
    std::string name;
    std::string value;   // empty = query
    bool hasValue = false;
};

void runVar(const VarOptions& opts, const OutputOptions& out, int& exitCode)
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

    if (!opts.hasValue) {
        // Query: find the variable by name in the list
        unsigned long count = 0;
        if (GetScriptVariableCount(dev.handle(), &count) != NO_TAC_ERROR) {
            printError("var query failed: " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        for (unsigned long i = 0; i < count; ++i) {
            char buf[1024]{};
            if (GetScriptVariable(dev.handle(), i, buf,
                                  static_cast<int>(sizeof(buf))) != NO_TAC_ERROR)
                continue;
            ScriptVar v = parseVar(buf);
            if (v.name == opts.name) {
                if (out.quiet)
                    return;
                if (out.json)
                    std::cout << nlohmann::json({{"name", v.name}, {"value", v.value}}).dump() << "\n";
                else
                    std::cout << v.value << "\n";
                return;
            }
        }
        printError("variable '" + opts.name + "' not found", out);
        exitCode = ExitCode::TacCommandFail;
    } else {
        // Set
        if (UpdateScriptVariableValue(dev.handle(), opts.name.c_str(),
                                      opts.value.c_str()) != NO_TAC_ERROR) {
            printError("set var '" + opts.name + "' failed: " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        if (!out.quiet) {
            if (out.json)
                std::cout << nlohmann::json({{"name", opts.name}, {"value", opts.value}}).dump() << "\n";
            else
                std::cout << opts.value << "\n";
        }
    }
}

} // namespace

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

void registerVars(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<VarsOptions>();

    auto* sub = app.add_subcommand("vars", "List all script variables on the device");
    sub->add_option("-d,--device", opts->device, "Device port name");
    sub->add_option("-s,--serial", opts->serial, "Device serial number");

    sub->callback([&, opts]() {
        runVars(*opts, globalOutput, exitCode);
    });
}

void registerVar(CLI::App& app, OutputOptions& globalOutput, int& exitCode)
{
    auto opts = std::make_shared<VarOptions>();

    auto* sub = app.add_subcommand("var", "Get or set a script variable");
    sub->add_option("-d,--device", opts->device, "Device port name");
    sub->add_option("-s,--serial", opts->serial, "Device serial number");
    sub->add_option("-n,--name",   opts->name,   "Variable name")->required();
    auto* valOpt = sub->add_option("-v,--value", opts->value, "Value to set (omit to query)");

    sub->callback([&, opts, valOpt]() {
        opts->hasValue = valOpt->count() > 0;
        runVar(*opts, globalOutput, exitCode);
    });
}

} // namespace taccl
