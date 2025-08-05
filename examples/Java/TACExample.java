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
