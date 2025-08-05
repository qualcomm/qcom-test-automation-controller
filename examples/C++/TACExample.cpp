/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Author: Biswajit Roy <biswroy@qti.qualcomm.com>

// TacExample.cpp : This file contains the 'main' function. Program execution begins and ends there.

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
