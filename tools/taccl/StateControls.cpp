// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.

#include "StateControls.h"

#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>

#include "Device.h"
#include "Errors.h"
#include "Output.h"

namespace taccl {

namespace {

// ---------------------------------------------------------------------------
// Table of standard get/set state controls.
//
// Each entry corresponds to a subcommand of the form:
//     taccl <name> --device=<port> [--state=on|off]
//
// The getter/setter pointers come directly from TACDev.h.
// ---------------------------------------------------------------------------
const StateControl kStateControls[] = {
    {"battery",            "Battery power (query/set)",              GetBatteryState,           SetBatteryState},
    {"usb0",               "USB0 line (query/set)",                  GetUsb0State,              Usb0},
    {"usb1",               "USB1 line (query/set)",                  GetUsb1State,              Usb1},
    {"power-key",          "Power key (query/set)",                  GetPowerKeyState,          PowerKey},
    {"volume-up",          "Volume-up key (query/set)",              GetVolumeUpState,          VolumeUp},
    {"volume-down",        "Volume-down key (query/set)",            GetVolumeDownState,        VolumeDown},
    {"disconnect-uim1",    "Disconnect UIM1 (query/set)",            GetDisconnectUIM1State,    DisconnectUIM1},
    {"disconnect-uim2",    "Disconnect UIM2 (query/set)",            GetDisconnectUIM2State,    DisconnectUIM2},
    {"disconnect-sdcard",  "Disconnect SD card (query/set)",         GetDisconnectSDCardState,  DisconnectSDCard},
    {"disconnect-headset", "Disconnect headset (query/set)",         GetHeadsetDisconnectState, HeadsetDisconnect},
    {"primary-edl",        "Primary EDL (query/set)",                GetPrimaryEDLState,        PrimaryEDL},
    {"secondary-edl",      "Secondary EDL (query/set)",              GetSecondaryEDLState,      SecondaryEDL},
    {"force-pshold",       "Force PS-Hold high (query/set)",         GetForcePSHoldHighState,   ForcePSHoldHigh},
    {"secondary-pm-resin", "Secondary PM RESIN_N (query/set)",       GetSecondaryPmResinNState, SecondaryPmResinN},
    {"eud",                "EUD (query/set)",                        GetEUDState,               Eud},
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Fetch the last TAC error string (best-effort) for user-facing messages.
std::string lastTacError()
{
    char buffer[512]{};
    if (GetLastTACError(buffer, static_cast<int>(sizeof(buffer))) == NO_TAC_ERROR)
        return std::string(buffer);
    return "unknown TAC error";
}

// Open a device, printing an appropriate error message and setting the exit
// code on failure. Returns an invalid Device in that case.
Device openDeviceOrFail(const StateControlOptions& opts,
                       const OutputOptions&        out,
                       int&                        exitCode)
{
    std::string port = resolvePortName(opts.device, opts.serial);
    if (port.empty()) {
        printError("no device specified (use --device / --serial or "
                   "TACCL_DEVICE / TACCL_SERIAL)", out);
        exitCode = ExitCode::UsageError;
        return Device{};
    }

    Device dev(port);
    if (!dev.isValid()) {
        printError("device '" + port + "' not found", out);
        exitCode = ExitCode::DeviceNotFound;
    }
    return dev;
}

// Parse the string form of the --state option. Returns false on invalid input.
bool parseState(const std::string& value, bool& outState)
{
    if (value == "on")  { outState = true;  return true; }
    if (value == "off") { outState = false; return true; }
    return false;
}

// ---------------------------------------------------------------------------
// Runners for each subcommand shape
// ---------------------------------------------------------------------------

// Standard query/set control (entries in kStateControls).
void runStandardControl(const StateControl&        ctrl,
                        const StateControlOptions& opts,
                        const OutputOptions&       out,
                        int&                       exitCode)
{
    Device dev = openDeviceOrFail(opts, out, exitCode);
    if (!dev.isValid()) return;

    if (opts.stateProvided) {
        if (!ctrl.set) {
            printError(std::string(ctrl.name) + " is a read-only control", out);
            exitCode = ExitCode::UsageError;
            return;
        }

        bool desired = false;
        if (!parseState(opts.stateStr, desired)) {
            printError("invalid value '" + opts.stateStr +
                       "' for --state, expected 'on' or 'off'", out);
            exitCode = ExitCode::UsageError;
            return;
        }

        if (ctrl.set(dev.handle(), desired) != NO_TAC_ERROR) {
            printError(std::string(ctrl.name) + " set failed: " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }

        // Prefer read-back for confirmation; fall back to echoing the desired
        // state if the control is set-only (none currently, but future-proof).
        bool actual = desired;
        if (ctrl.get && ctrl.get(dev.handle(), &actual) != NO_TAC_ERROR)
            actual = desired;

        printState(actual, out);
    } else {
        if (!ctrl.get) {
            printError(std::string(ctrl.name) +
                       " is a set-only control; provide --state=on|off", out);
            exitCode = ExitCode::UsageError;
            return;
        }

        bool state = false;
        if (ctrl.get(dev.handle(), &state) != NO_TAC_ERROR) {
            printError(std::string(ctrl.name) + " query failed: " + lastTacError(), out);
            exitCode = ExitCode::TacCommandFail;
            return;
        }
        printState(state, out);
    }
}

// external-power: set-only, no TACDev getter.
void runExternalPower(const StateControlOptions& opts,
                      const OutputOptions&       out,
                      int&                       exitCode)
{
    if (!opts.stateProvided) {
        printError("external-power is set-only; provide --state=on|off", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    bool desired = false;
    if (!parseState(opts.stateStr, desired)) {
        printError("invalid value '" + opts.stateStr +
                   "' for --state, expected 'on' or 'off'", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    Device dev = openDeviceOrFail(opts, out, exitCode);
    if (!dev.isValid()) return;

    if (SetExternalPowerControl(dev.handle(), desired) != NO_TAC_ERROR) {
        printError("external-power set failed: " + lastTacError(), out);
        exitCode = ExitCode::TacCommandFail;
        return;
    }
    printState(desired, out);
}

// pin: set-only, requires --pin=<int> and --state=on|off.
struct PinOptions {
    StateControlOptions base;
    int                 pin = -1;
    bool                pinProvided = false;
};

void runPin(const PinOptions&    opts,
            const OutputOptions& out,
            int&                 exitCode)
{
    if (!opts.pinProvided || opts.pin < 0) {
        printError("pin requires --pin=<number>", out);
        exitCode = ExitCode::UsageError;
        return;
    }
    if (!opts.base.stateProvided) {
        printError("pin requires --state=on|off", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    bool desired = false;
    if (!parseState(opts.base.stateStr, desired)) {
        printError("invalid value '" + opts.base.stateStr +
                   "' for --state, expected 'on' or 'off'", out);
        exitCode = ExitCode::UsageError;
        return;
    }

    Device dev = openDeviceOrFail(opts.base, out, exitCode);
    if (!dev.isValid()) return;

    if (SetPinState(dev.handle(), opts.pin, desired) != NO_TAC_ERROR) {
        printError("pin set failed: " + lastTacError(), out);
        exitCode = ExitCode::TacCommandFail;
        return;
    }
    printState(desired, out);
}

// Attach --device / --serial / --state options to a subcommand.
CLI::Option* addCommonOptions(CLI::App*            sub,
                              StateControlOptions& opts,
                              bool                 requireState = false)
{
    sub->add_option("-d,--device", opts.device, "Device port name");
    sub->add_option("-s,--serial", opts.serial, "Device serial number");
    auto* stateOpt = sub->add_option("--state", opts.stateStr,
                                     "on|off (omit to query current state)")
                        ->check(CLI::IsMember({"on", "off"}));
    if (requireState) stateOpt->required();
    return stateOpt;
}

} // namespace

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

void registerStateControls(CLI::App& app,
                           OutputOptions& globalOutput,
                           int& exitCode)
{
    // Use shared_ptr to keep option storage alive for the lifetime of the app.
    // CLI11 callbacks fire after parse(), so plain locals would dangle.
    auto keepAlive = std::make_shared<std::vector<std::shared_ptr<void>>>();

    // Standard controls
    for (const auto& ctrl : kStateControls) {
        auto opts = std::make_shared<StateControlOptions>();
        keepAlive->push_back(opts);

        auto* sub = app.add_subcommand(ctrl.name, ctrl.description);
        auto* stateOpt = addCommonOptions(sub, *opts);

        sub->callback([&, ctrlRef = &ctrl, opts, stateOpt]() {
            opts->stateProvided = (stateOpt->count() > 0);
            runStandardControl(*ctrlRef, *opts, globalOutput, exitCode);
        });
    }

    // external-power (set-only, no getter in TACDev.h)
    {
        auto opts = std::make_shared<StateControlOptions>();
        keepAlive->push_back(opts);

        auto* sub = app.add_subcommand("external-power",
                                       "External power (set-only)");
        auto* stateOpt = addCommonOptions(sub, *opts, /*requireState=*/true);

        sub->callback([&, opts, stateOpt]() {
            opts->stateProvided = (stateOpt->count() > 0);
            runExternalPower(*opts, globalOutput, exitCode);
        });
    }

    // pin (set-only, needs --pin=<int> and --state)
    {
        auto opts = std::make_shared<PinOptions>();
        keepAlive->push_back(opts);

        auto* sub = app.add_subcommand("pin",
                                       "Raw pin state (set-only)");
        auto* stateOpt = addCommonOptions(sub, opts->base, /*requireState=*/true);
        auto* pinOpt   = sub->add_option("--pin", opts->pin, "Pin number")
                            ->required();

        sub->callback([&, opts, stateOpt, pinOpt]() {
            opts->base.stateProvided = (stateOpt->count() > 0);
            opts->pinProvided        = (pinOpt->count() > 0);
            runPin(*opts, globalOutput, exitCode);
        });
    }

    // Keep the option storage alive as long as the CLI11 app is alive.
    // The simplest way is to stash it on the app's user data via a final
    // callback that captures it; the pre_callback fires before any subcommand
    // callback so it's a safe place.
    app.preparse_callback([keepAlive](size_t){ (void)keepAlive; });
}

} // namespace taccl