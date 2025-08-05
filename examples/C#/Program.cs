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

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

using System;
using System.Threading;
using System.Diagnostics;

using TACDevInterop;

namespace TACExample
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Process ID:" + Process.GetCurrentProcess().Id);
            Console.WriteLine("QTAC Version:" + TacDevice.GetAlpacaVersion());
            Console.WriteLine("TAC Version:" + TacDevice.GetTACVersion());

            uint deviceCount = TacDevice.DeviceCount();
            Console.WriteLine("Device Count:" + deviceCount);
            try
            {
                for (uint deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++)
                {
                    TacDevice tacDevice = TacDevice.GetDevice(deviceIndex);

                    Console.WriteLine("Found Port Name: " + tacDevice.PortName() + " Serial Number: " + tacDevice.SerialNumber() + " Description:" + tacDevice.Description() + "\n");

                    if (tacDevice.Open() == true)
                    {
                        Console.WriteLine($"Power Cycling the device with serial number: {tacDevice.SerialNumber()} and device name: {tacDevice.GetName()}");

                        bool batteryState = tacDevice.GetBatteryState();
                        Console.WriteLine($"Initial battery state is: {batteryState}");

                        tacDevice.SetBatteryState(!batteryState);
                        Console.WriteLine($"Updated battery state to: {tacDevice.GetBatteryState()}");

                        Thread.Sleep(2000);

                        tacDevice.SetBatteryState(batteryState);
                        Console.WriteLine($"Updated battery state to: {tacDevice.GetBatteryState()}");

                        uint commandCount = tacDevice.GetCommandCount();
                        Console.WriteLine($"Command count: {commandCount}");

                        if (commandCount > 0)
                        {
                            String commandString = tacDevice.GetCommand(0);
                            Console.WriteLine($"Command String at index 0 is: {commandString}");
                        }


                        Console.WriteLine($"\nHelp text for the TAC device:\n{tacDevice.GetHelpText()}");

                        Console.WriteLine("Disconnecting the battery using SendCommand()");
                        tacDevice.SendCommand("battery", true);
                        Thread.Sleep(2000);
                        Console.WriteLine("Re-connecting the battery using SendCommand()");
                        tacDevice.SendCommand("battery", false);

                        bool state = tacDevice.GetCommandState("battery");
                        Console.WriteLine($"Current state of the battery command is: {state}");

                        Thread.Sleep(2000);

                        tacDevice.Close();
                    }
                }
            }
            catch(Exception e)
            {
                Console.WriteLine($"Error: {e.Message}");
            }
        }
    }
}
