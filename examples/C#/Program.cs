// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
