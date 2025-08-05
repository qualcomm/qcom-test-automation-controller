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
