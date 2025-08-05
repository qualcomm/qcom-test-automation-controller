// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
