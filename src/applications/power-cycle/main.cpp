// Copyright (c) 2020-2025 Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
    Author: Michael Simpson (msimpson@qti.qualcomm.com)
            Biswajit Roy (biswroy@qti.qualcomm.com)
*/

// TACDev
#include "TACDev.h"

// QCommon
#include "AlpacaDefines.h"
#include "Range.h"

// QT
#include <QCoreApplication>
#include <QThread>

// Windows
//#include <Windows.h>

// C++
#include <iostream>

void printUsage()
{
    std::cout << std::endl << "Usage: PowerCycle -v " << std::endl;
    std::cout << "Usage: PowerCycle <device name> i.e. \"COM8\" or \"VTP2\" " << std::endl;
}

int main(int argc, char *argv[])
{
    int result{1};

    QCoreApplication a(argc, argv);

    if (argc >= 2)
    {
        QString comPort = QString(argv[1]).toUpper();
        if (comPort.startsWith("-v", Qt::CaseInsensitive))
        {
            std::cout << "PowerCycle" << std::endl;
            std::cout << "Alpaca Version:" << kProductVersion.toLatin1().data() << std::endl;
            std::cout << "Build Date:" << kBuildTime.toLatin1().data() << std::endl;

            return 0;
        }
        else if (comPort.startsWith("-h", Qt::CaseInsensitive))
        {
            printUsage();
            return 0;
        }

        int deviceCount;

        TAC_RESULT result = GetDeviceCount(&deviceCount);
        if (result == NO_TAC_ERROR && deviceCount > 0)
        {
            TAC_HANDLE tacHandle = OpenHandleByDescription(comPort.toLatin1().data());
            if (tacHandle != kBadHandle)
            {
                SendCommand(tacHandle, "powerOff", true);
                QThread::sleep(1);
                SendCommand(tacHandle, "powerOn", true);

                bool status{false};
                while (!status)
                {
                    result = IsCommandQueueClear(tacHandle, &status);
                    QThread::sleep(500);

                    if (result != NO_TAC_ERROR)
                    {
                        std::cout << "Unable to get command queue status " << comPort.toLatin1().data() << ". Open in another process?" << std::endl;
                        break;
                    }
                }
                CloseTACHandle(tacHandle);

                result = 0;
            }
            else
            {
                std::cout << "Unable to open port " << comPort.toLatin1().data() << ". Open in another process?" << std::endl;

                std::cout << "Port " << comPort.toLatin1().data() << " not found." << std::endl;
                std::cout << "Available Ports(" << deviceCount << ") :" << std::endl;
                for (const auto &deviceIndex : range(deviceCount))
                {
                    char portData[1024];

                    GetPortData(deviceIndex, portData, sizeof(portData));
                    std::cout << "   " << portData << std::endl;
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
        std::cout << "COM port parameter required" << std::endl;
        printUsage();
    }

    return result;
}