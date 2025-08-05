// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: Biswajit Roy <biswroy@qti.qualcomm.com>

package com.qualcomm.alpaca;

import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;


public class DeviceDependentTest {
	
	private final int deviceIndex = 1;

	@DisplayName("GetName should return expected name of the connected TAC device")
	@Test
	public void GetNameReturnsExpectedName() throws TACDevException {
		// Arrange
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				// Act
				String deviceName = tacDevice.Get_Name();
				tacDevice.Close();

				// Assert
				assertEquals("H680IEG9", deviceName);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("GetFirmwareVersion should return the expected firmware version")
	@Test
	public void GetFirmwareVersionReturnsExpectedString() throws TACDevException {
		// Arrange
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				// Act
				String firmwareVersion = tacDevice.GetFirmwareVersion();
				tacDevice.Close();

				// Assert
				assertEquals("1.4.1", firmwareVersion);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("GetHardware should return the expected debug board hardware type")
	@Test
	public void GetHardwareReturnsExpectedString() throws TACDevException {
		// Arrange
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				// Act
				String hardware = tacDevice.GetHardware();
				tacDevice.Close();

				// Assert
				assertEquals("FTDI", hardware);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("GetHardwareVersion return the version of debug board hardware")
	@Test
	public void GetHardwareVersionReturnsExpectedString() throws TACDevException {
		// Arrange
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				// Act
				String hardwareVersion = tacDevice.Get_HardwareVersion();
				tacDevice.Close();

				// Assert
				assertEquals("ALPACA LITE (FTDI)", hardwareVersion);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("GetUUID should return the UUID of the debug board")
	@Test
	public void GetUUIDReturnsExpectedString() throws TACDevException {
		// Arrange
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				// Act
				String uuid = tacDevice.Get_UUID();
				tacDevice.Close();

				// Assert
				assertEquals("FTDI - No UUID", uuid);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("GetCommandCount should return a positive count of the platform commands")
	@Test
	public void GetCommandCountReturnsPositiveInteger() throws TACDevException {
		// Arrange
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				// Act
				long result = tacDevice.GetCommandCount();
				tacDevice.Close();

				// Assert
				assert result > 0;
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("GetCommand for index 0 should return non-empty string")
	@Test
	public void GetCommandReturnsNonEmptyString() throws TACDevException {
		// Arrange
		long commandIndex = 0;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				// Act
				String result = tacDevice.GetCommand(commandIndex);
				tacDevice.Close();

				// Assert
				assert result.length() > 0;
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("GetCommandState and SendCommand sets the expected command state")
	@Test
	public void GetCommandStateReturnsExpectedState() throws TACDevException {
		// Arrange
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				// Act
				long commandCount = tacDevice.GetCommandCount();
				if (commandCount > 0) {
					// Consider the first command in the commands list
					String commandString = tacDevice.GetCommand(2);
					// Take the first element because that is the actual command
					String actualCommand = commandString.split(";")[0];
					// Initial command state
					boolean state = tacDevice.GetCommandState(actualCommand);
					tacDevice.SendCommand(actualCommand, !state);
					boolean newState = tacDevice.GetCommandState(actualCommand);

					// Reset the command state to original state
					tacDevice.SendCommand(actualCommand, state);
					tacDevice.Close();

					// Assert
					assert newState != state;
				}

				tacDevice.Close();

			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("GetHelpText should return a non-empty string")
	@Test
	public void GetHelpTextReturnsNonEmptyString() throws TACDevException {
		// Arrange
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				// Act
				String result = tacDevice.GetHelpText();
				tacDevice.Close();

				// Assert
				assert result.length() > 0;
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("USB0 and GetUSB0State should return consistent data")
	@Test
	public void USB0AndGetUSB0StateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedUSB0State = false;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalUSB0State = tacDevice.GetUSB0State();

				// Act
				tacDevice.USB0(expectedUSB0State);
				boolean foundState = tacDevice.GetUSB0State();
				// Reset the button assert time on the device to original value
				tacDevice.USB0(originalUSB0State);
				tacDevice.Close();

				// Assert
				assertEquals(foundState, expectedUSB0State);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("USB1 and GetUS10State should return consistent data")
	@Test
	public void USB1AndGetUSB1StateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedUSB1State = false;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalUSB1State = tacDevice.GetUSB1State();

				// Act
				tacDevice.USB1(expectedUSB1State);
				boolean state = tacDevice.GetUSB1State();
				// Reset the button assert time on the device to original value
				tacDevice.USB1(originalUSB1State);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedUSB1State);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("PowerKey and GetPowerKey should return consistent data")
	@Test
	public void PowerKeyAndGetPowerKeyReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedPowerKeyState = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalPowerKeyState = tacDevice.GetPowerKeyState();

				// Act
				tacDevice.PowerKey(expectedPowerKeyState);
				boolean state = tacDevice.GetPowerKeyState();
				// Reset the button assert time on the device to original value
				tacDevice.PowerKey(originalPowerKeyState);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedPowerKeyState);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("VolumeUp and GetVolumeUpState should return consistent data")
	@Test
	public void VolumeUpAndGetVolumeUpStateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedVolumeUpState = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalVolumeUpState = tacDevice.GetVolumeUpState();

				// Act
				tacDevice.VolumeUp(expectedVolumeUpState);
				boolean state = tacDevice.GetVolumeUpState();
				// Reset the button assert time on the device to original value
				tacDevice.VolumeUp(originalVolumeUpState);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedVolumeUpState);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("VolumeDown and GetVolumeDownState should return consistent data")
	@Test
	public void VolumeDownAndGetVolumeDownStateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedVolumeDownState = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalVolumeDownState = tacDevice.GetVolumeDownState();

				// Act
				tacDevice.VolumeDown(expectedVolumeDownState);
				boolean state = tacDevice.GetVolumeDownState();
				// Reset the button assert time on the device to original value
				tacDevice.VolumeDown(originalVolumeDownState);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedVolumeDownState);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("DisconnectUIM1 and GetDisconnectUIM1State should return consistent data")
	@Test
	public void DisconnectUIM1AndGetDisconnectUIM1StateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedDisconnectUIM1State = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalDisconnectUIM1State = tacDevice.GetDisconnectUIM1State();

				// Act
				tacDevice.DisconnectUIM1(expectedDisconnectUIM1State);
				boolean state = tacDevice.GetDisconnectUIM1State();
				// Reset the button assert time on the device to original value
				tacDevice.DisconnectUIM1(originalDisconnectUIM1State);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedDisconnectUIM1State);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("DisconnectUIM2 and GetDisconnectUIM2State should return consistent data")
	@Test
	public void DisconnectUIM2AndGetDisconnectUIM2StateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedDisconnectUIM2State = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalDisconnectUIM2State = tacDevice.GetDisconnectUIM2State();

				// Act
				tacDevice.DisconnectUIM2(expectedDisconnectUIM2State);
				boolean state = tacDevice.GetDisconnectUIM2State();
				// Reset the button assert time on the device to original value
				tacDevice.DisconnectUIM1(originalDisconnectUIM2State);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedDisconnectUIM2State);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("DisconnectSDCard and GetDisconnectSDCardState should return consistent data")
	@Test
	public void DisconnectSDCardAndGetDisconnectSDCardStateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedDisconnectSDCardState = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalDisconnectSDCardState = tacDevice.GetDisconnectSDCardState();

				// Act
				tacDevice.DisconnectSDCard(expectedDisconnectSDCardState);
				boolean state = tacDevice.GetDisconnectSDCardState();
				// Reset the button assert time on the device to original value
				tacDevice.DisconnectSDCard(originalDisconnectSDCardState);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedDisconnectSDCardState);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("PrimaryEDL and GetPrimaryEDLState should return consistent data")
	@Test
	public void PrimaryEDLAndGetPrimaryEDLStateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedPrimaryEDLState = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalPrimaryEDLState = tacDevice.GetPrimaryEDLState();

				// Act
				tacDevice.PrimaryEDL(expectedPrimaryEDLState);
				boolean state = tacDevice.GetPrimaryEDLState();
				// Reset the button assert time on the device to original value
				tacDevice.PrimaryEDL(originalPrimaryEDLState);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedPrimaryEDLState);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("SecondaryEDL and GetSecondaryEDLState should return consistent data")
	@Test
	public void SecondaryEDLAndGetSecondaryEDLStateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedSecondaryEDLState = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalSecondaryEDLState = tacDevice.GetSecondaryEDLState();

				// Act
				tacDevice.SecondaryEDL(expectedSecondaryEDLState);
				boolean state = tacDevice.GetSecondaryEDLState();
				// Reset the button assert time on the device to original value
				tacDevice.SecondaryEDL(originalSecondaryEDLState);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedSecondaryEDLState);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("SecondaryPM_RESIN_N and GetSecondaryPM_RESIN_NState should return consistent data")
	@Test
	public void SecondaryPM_RESIN_NAndGetSecondaryPM_RESIN_NStateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedSecondaryPM_RESIN_NState = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalSecondaryPM_RESIN_NState = tacDevice.GetSecondaryPM_RESIN_NState();

				// Act
				tacDevice.SecondaryPM_RESIN_N(expectedSecondaryPM_RESIN_NState);
				boolean state = tacDevice.GetSecondaryPM_RESIN_NState();
				// Reset the button assert time on the device to original value
				tacDevice.SecondaryPM_RESIN_N(originalSecondaryPM_RESIN_NState);
				tacDevice.Close();

				// Assert
				assertEquals(expectedSecondaryPM_RESIN_NState, state);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("EUD and GetEUDState should return consistent data")
	@Test
	public void EUDAndGetEUDStateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedEUDState = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalEUDState = tacDevice.GetEUDState();

				// Act
				tacDevice.EUD(expectedEUDState);
				boolean state = tacDevice.GetEUDState();
				// Reset the button assert time on the device to original value
				tacDevice.EUD(originalEUDState);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedEUDState);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("HeadsetDisconnect and GetHeadsetDisconnect should return consistent data")
	@Test
	public void HeadsetDisconnectAndGetHeadsetDisconnectStateReturnsConsistentData() throws TACDevException {
		// Arrange
		boolean expectedHeadsetDisconnectState = true;
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				boolean originalHeadsetDisconnectState = tacDevice.GetHeadsetDisconnectState();

				// Act
				tacDevice.HeadsetDisconnect(expectedHeadsetDisconnectState);
				boolean state = tacDevice.GetHeadsetDisconnectState();
				// Reset the button assert time on the device to original value
				tacDevice.HeadsetDisconnect(originalHeadsetDisconnectState);
				tacDevice.Close();

				// Assert
				assertEquals(state, expectedHeadsetDisconnectState);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}

	@DisplayName("SetName and Get_Name should return consistent data")
	@Test
	public void SetNameAndGet_NameReturnsConsistentData() throws TACDevException {
		// Arrange
		String expectedName = "Biswajit Roy";
		TACDev tacDev = new TACDev();
		long deviceCount = tacDev.GetDeviceCount();

		if (deviceCount > 0) {
			TACDevice tacDevice = tacDev.GetDevice(deviceIndex);
			try {
				tacDevice.Open();

				String originalName = tacDevice.Get_Name();

				// Act
				tacDevice.SetName(expectedName);
				String name = tacDevice.Get_Name();
				// Reset the button assert time on the device to original value
				tacDevice.SetName(originalName);
				tacDevice.Close();

				// Assert
				assertEquals(name, expectedName);
			} catch (TACDeviceException e) {
				// In case, the test fails because the device could not be opened,
				// mark the test as failed.
				tacDevice.Close();
				assert false;
			}
		} else {
			// If a device is not connected, mark the test as failed
			assert false;
		}
	}
}
