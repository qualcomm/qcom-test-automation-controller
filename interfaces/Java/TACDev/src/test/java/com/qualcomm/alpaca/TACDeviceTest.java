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
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.CsvSource;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;


public class TACDeviceTest {

    @DisplayName("GetName should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI",
            "IF7DS69RV, Sorry what?, COIV79DF",
            "S9D8V0WE7GF, I can't hear you over the sound of my flammenwerfer, SO9F7V8GRE4R"
    })
    public void GetNameThrowsExpectedException(String portName, String description, String serialNumber) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, tacDevice::Get_Name);

        // Assert
        assertEquals(exception.getMessage(), "[BAD TAC Handle]: Is the device open?");
    }

    @DisplayName("GetFirmwareVersion should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI"
    })
    public void GetFirmwareVersionThrowsExpectedException(String portName, String description, String serialNumber) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, tacDevice::GetFirmwareVersion);

        // Assert
        assertEquals(exception.getMessage(), "[BAD TAC Handle]: Is the device open?");
    }

    @DisplayName("GetHardware should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI"
    })
    public void GetHardwareThrowsExpectedException(String portName, String description, String serialNumber) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, tacDevice::GetHardware);

        // Assert
        assertEquals(exception.getMessage(), "[BAD TAC Handle]: Is the device open?");
    }

    @DisplayName("GetHardwareVersion should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI"
    })
    public void GetHardwareVersionThrowsExpectedException(String portName, String description, String serialNumber) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, tacDevice::Get_HardwareVersion);

        // Assert
        assertEquals(exception.getMessage(), "[BAD TAC Handle]: Is the device open?");
    }

    @DisplayName("GetUUID should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI"
    })
    public void GetUUIDThrowsExpectedException(String portName, String description, String serialNumber) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, tacDevice::Get_UUID);

        // Assert
        assertEquals(exception.getMessage(), "[BAD TAC Handle]: Is the device open?");
    }

    @DisplayName("SetButtonAssertTime should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}, expectedTime={3}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW, 1",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI, 22"
    })
    public void SetButtonAssertTimeThrowsExpectedException(String portName, String description, String serialNumber, int expectedTime) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, () ->tacDevice.Set_Button_Assert_Time(expectedTime));

        // Assert
        assertEquals("Set_Button_Assert_Time deprecated. This is now in the configurations", exception.getMessage());
    }

    @DisplayName("GetButtonAssertTime should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI"
    })
    public void GetButtonAssertTimeThrowsExpectedException(String portName, String description, String serialNumber) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, tacDevice::Get_Button_Assert_Time);

        // Assert
        assertEquals("Get_Button_Assert_Time deprecated. This is now in the configurations",exception.getMessage());
    }

    @DisplayName("SetExternalPowerControl should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}, state={3}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW, true",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI, false"
    })
    public void SetExternalPowerControlThrowsExpectedException(String portName, String description, String serialNumber, boolean state) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, () -> tacDevice.SetExternalPowerControl(state));

        // Assert
        assertEquals(exception.getMessage(), "[BAD TAC Handle]: Is the device open?");
    }

    @DisplayName("SetBatteryState should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}, state={3}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW, false",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI, true"
    })
    public void SetBatteryStateThrowsExpectedException(String portName, String description, String serialNumber, boolean state) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, () -> tacDevice.SetBatteryState(state));

        // Assert
        assertEquals(exception.getMessage(), "[BAD TAC Handle]: Is the device open?");
    }

    @DisplayName("GetBatteryState should throw exception on invalid TACDevice object")
    @ParameterizedTest(name = "{index} => portName={0}, description={1}, serialNumber={2}")
    @CsvSource({
            "TESTPORT, TEST DESCRIPTION, IDULWVCTYL8IREW",
            "8V76RE98VY, QTAC Lite Device, KJFDYV8IELFYVI"
    })
    public void GetBatteryStateThrowsExpectedException(String portName, String description, String serialNumber) {
        // Arrange
        TACDevice tacDevice = new TACDevice(portName, description, serialNumber);

        // Act
        Exception exception = assertThrows(TACDeviceException.class, tacDevice::GetBatteryState);

        // Assert
        assertEquals(exception.getMessage(), "[BAD TAC Handle]: Is the device open?");
    }
}
