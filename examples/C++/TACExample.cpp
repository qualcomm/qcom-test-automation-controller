// TacExample.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// QTAC
#include "TACDev.h"

// windows
#include <Windows.h>

// C++
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>

std::string ParsePortName(char* portData)
{
    std::string str(portData);
    std::string buf;                 // Have a buffer string
    std::stringstream ss(str);       // Insert the string into a stream

    std::vector<std::string> tokens; // Create vector to hold our words

    while (std::getline(ss, buf, ';'))
        tokens.push_back(buf);

    return tokens[0];
}


int main()
{
    char buffer[1048];
    unsigned long bufferSize{sizeof(buffer)};
    int result{0};

    InitializeTACDev();

    if (GetAlpacaVersion(buffer, bufferSize) == NO_TAC_ERROR)
        std::cout << "QTAC Version: " << buffer << std::endl;

    if (GetTACVersion(buffer, bufferSize) == NO_TAC_ERROR)
        std::cout << "TAC Version: " << buffer << std::endl;

    int deviceCount(0);
    GetDeviceCount(&deviceCount);
    std::cout << "Device Count: " << deviceCount << std::endl;

    if (deviceCount > 0)
    {
        std::string portName;

        for (int deviceIndex{ 0 }; deviceIndex < deviceCount; deviceIndex++)
        {
            GetPortData(deviceIndex, buffer, bufferSize);
            std::cout << "Port Data: \"" << buffer << "\"" << std::endl;

            if (portName.length() == 0)
                portName = ParsePortName(buffer);
        }

        if (portName.length() > 0)
        {
            TAC_HANDLE tacHandle = OpenHandleByDescription(portName.data());
            if (tacHandle != kBadHandle)
            {
                ::Sleep(500);

                if (GetUUID(tacHandle, buffer, bufferSize) == NO_TAC_ERROR)
                    std::cout << "UUID: \"" << buffer << "\"" << std::endl;

                CloseTACHandle(tacHandle);
            }
        }
    }
}
