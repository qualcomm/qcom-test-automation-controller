// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/**
 * Example program to demonstrate the TAC API and some basic TAC operations
 * @author Biswajit Roy < biswroy@qti.qualcomm.com >
 */

import com.qualcomm.alpaca.*;
import com.qualcomm.alpaca.TACDev.*;
import com.qualcomm.alpaca.TACDevice.*;
import com.qualcomm.alpaca.TACDeviceException;
import java.lang.management.ManagementFactory;


public class TACExample {
    public static void main(String[] args) throws InterruptedException, TACDevException, TACDeviceException {

        String name = ManagementFactory.getRuntimeMXBean().getName();
        System.out.println("Process ID: " + name.split("@")[0]);

        // Create a TACDev object
        TACDev tacdev = new TACDev();

        // TACDev provides access to some basic QTAC APIs
        System.out.println("QTAC version: " + tacdev.AlpacaVersion());
        System.out.println("TAC version: " + tacdev.TACVersion());

        long deviceCount = tacdev.GetDeviceCount();
        System.out.println("Device count: " + deviceCount);

        // If a TAC device is connected to the machine, deviceCount will be > 0
        if(deviceCount > 0) {
            // Get the first TAC device (index 0) instance from TACDev
            TACDevice device = tacdev.GetDevice(0);

            System.out.println("Device description: " + device.Description());

            // TAC device becomes valid after Open() is called
            System.out.println("Device is valid? " + device.Valid());


            boolean result = device.Open();
            System.out.println("Device is opened: " + result);
            if (result) {
                System.out.println("Device is valid? " + device.Valid());

                // Executing a device reset operation
                System.out.println("Attempting to power off and power on device");
                device.SetBatteryState(false);
                Thread.sleep(2000);
                device.SetBatteryState(true);
                System.out.println("Device restart sequence is complete");
                device.Close();
            }

        }
    }
}
