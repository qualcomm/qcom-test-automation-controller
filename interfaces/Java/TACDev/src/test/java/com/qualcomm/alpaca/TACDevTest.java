package com.qualcomm.alpaca;

import org.junit.jupiter.api.Test;


public class TACDevTest
{
    @Test
    public void AlpacaVersionShouldNotBeEmpty() throws TACDevException {
        // Arrange
        TACDev tacdev = new TACDev();

        // Act
        String version = tacdev.AlpacaVersion();

        // Assert
        assert version.isEmpty() == false;
    }

    @Test
    public void AlpacaVersionShouldReturnExpectedString() throws TACDevException {
        // Arrange
        TACDev tacdev = new TACDev();

        // Act
        String version = tacdev.AlpacaVersion();

        // Assert : the version string should contain a major.minor.patch style version string
        assert version.split("\\.").length == 3;
    }

    @Test
    public void TACVersionShouldNotBeEmpty() throws TACDevException {
        // Arrange
        TACDev tacdev = new TACDev();

        // Act
        String version = tacdev.TACVersion();

        // Assert
        assert version.isEmpty() == false;
    }

    @Test
    public void TACVersionShouldReturnExpectedString() throws TACDevException {
        // Arrange
        TACDev tacdev = new TACDev();

        // Act
        String version = tacdev.TACVersion();

        // Assert : the version string should contain a major.minor.patch style version string
        assert version.split("\\.").length == 3;
    }

    @Test
    public void GetDeviceCountShouldReturnExpectedValue() throws TACDevException {
        // Arrange
        TACDev tacdev = new TACDev();

        // Act
        long deviceCount = tacdev.GetDeviceCount();

        // Assert
        assert deviceCount >= 0;
    }

    @Test
    public void GetDeviceShouldReturnTACDevice() throws TACDevException {
        // Arrange
        TACDevice tacDevice = null;
        TACDev tacdev = new TACDev();

        // Act
        long deviceCount = tacdev.GetDeviceCount();

        // Assert
        if(deviceCount > 0) {
            for (int idx = 0; idx < deviceCount; idx++) {
                tacDevice = tacdev.GetDevice(idx);

                assert tacDevice != null;
            }
        }
    }
}
