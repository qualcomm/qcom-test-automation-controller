// Copyright (c) 2020 Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
    Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// libTAC
#include "AlpacaDevice.h"
// QCommon
#include "AppCore.h"

// QT
#include <QCoreApplication>
#include <QThread>

// C++
#include <iostream>

void printUsage()
{
    std::cout << std::endl << "Usage: ExternalPowerControl <device name> <state:1 or 0>" << std::endl;
}

int main(int argc, char *argv[])
{
    int result{1};

    QCoreApplication a(argc, argv);

    AppCore* appCore = AppCore::getAppCore();
    PreferencesBase preferences;

    preferences.setAppName("External Power Control", "1.1.1");
    appCore->setPreferences(&preferences);

    if (argc >= 3)
    {
        bool ok;

        QString comPort = QString(argv[1]).toUpper();
        bool state = (QString(argv[2]).toUInt(&ok) > 0) ? true : false;

        if (ok)
        {
            AlpacaDevices alpacaDevices;

            int deviceCount = _AlpacaDevice::updateAlpacaDevices();

            if (deviceCount)
            {
                AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(comPort.toLatin1());
                if (alpacaDevice->active())
                {
                    alpacaDevice->open();

                    alpacaDevice->externalPowerControl(state);
                    result = 0;
                }
                else
                {
                    std::cout << "Port " << comPort.toLatin1().data() << " not found." << std::endl;
                    std::cout << "Available Ports(" << deviceCount << ") :" << std::endl;
                    for (const auto& alpacaDevice: std::as_const(alpacaDevices))
                    {
                        std::cout << "   " << alpacaDevice->portName().data() << std::endl;
                    }
                }
            }
            else
            {
                std::cout << "No TAC devices found" << std::endl;
            }
        }
        else
        {
            std::cout << "State should be a 0 or 1" << std::endl;
            printUsage();
        }
    }
    else
    {
        std::cout << "COM port and state parameters required" << std::endl;
        printUsage();
    }

    return result;
}