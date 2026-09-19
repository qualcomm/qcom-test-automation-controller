// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.

#include "Output.h"

#include <iostream>

#include <nlohmann/json.hpp>

namespace taccl {

void printState(bool state, const OutputOptions& opts)
{
    if (opts.quiet)
        return;

    if (opts.json) {
        nlohmann::json j;
        j["state"] = state;
        std::cout << j.dump() << std::endl;
    } else {
        std::cout << (state ? "on" : "off") << std::endl;
    }
}

void printError(const std::string& message, const OutputOptions& opts)
{
    if (opts.json) {
        nlohmann::json j;
        j["error"] = message;
        std::cerr << j.dump() << std::endl;
    } else {
        std::cerr << "taccl: " << message << std::endl;
    }
}

} // namespace taccl