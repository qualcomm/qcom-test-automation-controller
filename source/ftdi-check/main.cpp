// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "FTDICheckApplication.h"

#include <iostream>
#include <string>

static void printUsage()
{
    std::cout <<
        "ftdi-check verifies whether the appropriate FTDI drivers are installed\n"
        "and updates them if required.\n\n"
        "Usage: FTDICheck [validate | uninstall | install | version | --help]\n"
        "  validate    Validates the currently installed driver\n"
        "  uninstall   Uninstalls the current driver\n"
        "  install     Installs the appropriate FTDI driver for QTAC\n"
        "  version     Displays the version of FTDICheck\n"
        "  --help      Shows this help\n";
}

int main(int argc, char* argv[])
{
    FTDICheckApplication app;

    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if (arg == "validate")           return app.verifyInstalledDriver() ? 0 : 1;
        if (arg == "uninstall")          return app.uninstallDriver()       ? 0 : 1;
        if (arg == "install")            return app.installDriver()         ? 0 : 1;
        if (arg == "version")            { app.showVersion(); return 0; }
        if (arg == "--help" || arg == "-h") { printUsage(); return 0; }
    }

    return app.defaultActivity();
}
