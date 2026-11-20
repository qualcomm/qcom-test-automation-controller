// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.

#include "Output.h"

#include <iostream>

#include <qtac/Json.h>

namespace taccl {

void printState(bool state, const OutputOptions& opts)
{
    if (opts.quiet)
        return;

    if (opts.json) {
        qtac::JsonValue j = qtac::JsonValue::makeObject();
        j.setKey("state", state);
        std::cout << j.serialize() << std::endl;
    } else {
        std::cout << (state ? "on" : "off") << std::endl;
    }
}

void printError(const std::string& message, const OutputOptions& opts)
{
    if (opts.json) {
        qtac::JsonValue j = qtac::JsonValue::makeObject();
        j.setKey("error", message);
        std::cerr << j.serialize() << std::endl;
    } else {
        std::cerr << "taccl: " << message << std::endl;
    }
}

} // namespace taccl
