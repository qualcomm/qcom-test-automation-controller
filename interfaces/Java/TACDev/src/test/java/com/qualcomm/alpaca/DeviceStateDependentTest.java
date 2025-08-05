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

package com.qualcomm.alpaca;

import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import static java.lang.Thread.sleep;
import static org.junit.jupiter.api.Assertions.assertEquals;

public class DeviceStateDependentTest {
    private final int deviceIndex = 1;

    @DisplayName("SetBatteryState and GetBatteryState should update battery state properly")
    @Test
    public void SetBatteryStateAndGetBatteryStateUpdatesBatteryStateProperly() throws TACDevException {
        // Arrange
        boolean expectedBatteryState = false;
        TACDev tacDev = new TACDev();
        long deviceCount = tacDev.GetDeviceCount();

        if (deviceCount > 0) {
            TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
            try {
                tacDevice.Open();

                boolean originalBatteryState = tacDevice.GetBatteryState();

                // Act
                tacDevice.SetBatteryState(expectedBatteryState);

                // Assert
                assertEquals(tacDevice.GetBatteryState(), expectedBatteryState);

                // Reset the battery state on the device to original value
                tacDevice.SetBatteryState(originalBatteryState);

                tacDevice.Close();

                // Allow the device state to restore
                sleep(60000);
            } catch (TACDeviceException e) {
                // In case, the test fails because the device could not be opened,
                // mark the test as failed.
                tacDevice.Close();
                assert false;
            } catch (InterruptedException e) {
                tacDevice.Close();
                throw new RuntimeException(e);
            }
        } else {
            // If a device is not connected, mark the test as failed
            assert false;
        }
    }
}
