// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace TACDevInterop
{
	public class TACException : Exception
	{
		public TACException()
		{
		}

		public TACException(string message)
			: base(message)
		{
		}
	}

	public class TacDevice
	{
		public TacDevice(string portName, string description, string serialNumber)
		{
			_portName = portName;
			_description = description;
			_serialNumber = serialNumber;
		}

		public string alpacaVersion
		{
			get { return GetAlpacaVersion(); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetAlpacaVersion(byte[] alpacaVersion, uint bufferSize);

		static public string GetAlpacaVersion()
		{
			string result;

			byte[] output = new byte[kBufferSize];
			testResult(GetAlpacaVersion(output, kBufferSize));
			result = Encoding.ASCII.GetString(output).Trim('\0');
		
			return result;
		}

		public string tacVersion
		{
			get { return GetTACVersion(); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetTACVersion(byte[] tacVersion, uint bufferSize);

		static public string GetTACVersion()
		{
			string result = "";

			byte[] output = new byte[kBufferSize];
			testResult(GetTACVersion(output, kBufferSize));
			result = Encoding.ASCII.GetString(output).Trim('\0');

			return result;
		}


		public string lastError
		{
			get { return GetLastTACError(); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetLastTACError(byte[] error, uint bufferSize);

		static public string GetLastTACError()
		{
			string result = "";

			byte[] output = new byte[kBufferSize];
			testResult(GetLastTACError(output, kBufferSize));
			result = Encoding.ASCII.GetString(output).Trim('\0');

			return result;
		}


		public uint getLoggingState
		{
			get { return GetLoggingState(); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetLoggingState(ref bool loggingState);

		static public uint GetLoggingState()
		{
			return GetLoggingState();
		}

		public uint setLoggingState
		{
			get { return SetLoggingState(); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint SetLoggingState(bool loggingState);

		static public uint SetLoggingState()
		{
			return SetLoggingState();
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetDeviceCount(ref uint deviceCount);

		[DllImport("TACDev.dll")]
		private static extern uint InitializeTACDev();

		static public uint DeviceCount()
		{
			uint deviceCount = 0;

			testResult(InitializeTACDev());
			testResult(GetDeviceCount(ref deviceCount));
			
			return deviceCount;
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetPortData(uint deviceIndex, byte[] portData, uint bufferSize);

		static public TacDevice GetDevice(uint deviceIndex)
		{
			TacDevice result = null;

			byte[] output = new byte[kBufferSize];
			uint tacResult = GetPortData(deviceIndex, output, kBufferSize);
			if (tacResult > 0)
			{
				string portData = Encoding.ASCII.GetString(output);
				string[] portAttributes = portData.Split(';');
				if (portAttributes.Length > 2)
				{
					result = new TacDevice(portAttributes[0].Trim('\0'), portAttributes[1].Trim('\0'), portAttributes[2].Trim('\0'));
				}
			}

			return result;
		}

		public string portName
		{
			get { return PortName(); }
		}

		public string PortName()
		{
			return _portName;
		}

		public string description
		{
			get { return Description(); }
		}

		public string Description()
		{
			return _description;
		}

		public string serialNumber
		{
			get { return SerialNumber(); }
		}

		public string SerialNumber()
		{
			return _serialNumber;
		}

		[DllImport("TACDev.dll")]
		private static extern uint OpenHandleByDescription(string portName);

		public bool Open()
		{
			bool result = false;

			if (_portName.Length > 0)
				_tacHandle = OpenHandleByDescription(_portName);

			if (_tacHandle == BAD_TAC_HANDLE)
			{
				if (_serialNumber.Length > 0)
					_tacHandle = OpenHandleByDescription(_serialNumber);
			}

			if (_tacHandle != BAD_TAC_HANDLE)
				result = true;
			else
				throw new TACException(_portName + " not found");

			return result;
		}

		[DllImport("TACDev.dll")]
		private static extern uint CloseTACHandle(uint tacHandle);

		public bool Close()
		{
			uint tacResult = CloseTACHandle(_tacHandle);
			_tacHandle = BAD_TAC_HANDLE;
			if (tacResult == NO_TAC_ERROR)
				return true;

			if (tacResult == BAD_TAC_HANDLE)
				throw new TACException("Bad TAC Handle");

			return false;
		}

		[DllImport("TACDev.dll")]
		private static extern uint SetPinState(uint tacHandle, int pin, bool state);

		public void SetPinState(int pin, bool state)
		{
			if (TestHandle())
				testResult(SetPinState(_tacHandle, pin, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetCommandCount(uint tacHandle, ref uint secondsToAssert);

		public uint GetCommandCount()
		{
			uint result = 0;

			if (TestHandle())
				testResult(GetCommandCount(_tacHandle, ref result));

			return result;
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetCommand(uint tacHandle, uint commandIndex, byte[] portData, uint bufferSize);

		public string GetCommand(uint commandIndex)
		{
			string result = "";

			if (TestHandle())
			{
				byte[] output = new byte[kBufferSize];
				testResult(GetCommand(_tacHandle, commandIndex, output, kBufferSize));
				result = Encoding.ASCII.GetString(output).Trim('\0');
			}

			return result;
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetCommandState(uint tacHandle, string command, ref bool state);

		public bool GetCommandState(string command)
		{
			bool state = false;

			if (TestHandle())
				testResult(GetCommandState(_tacHandle, command, ref state));

			return state;
		}

		[DllImport("TACDev.dll")]
		private static extern uint SendCommand(uint tacHandle, string command, bool state); 
		
		public void SendCommand(string command, bool state)
		{
			if (TestHandle())
				testResult(SendCommand(_tacHandle, command, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetHelpText(uint tacHandle, byte[] helpData, uint bufferSize, ref uint actualBufferSize);

		public string GetHelpText()
		{
			string result = "";

			if (TestHandle())
			{

				uint actualBufferSize = 0;
				byte[] output = new byte[1];

				GetHelpText(_tacHandle, output, (uint)1, ref actualBufferSize);
				output = new byte[actualBufferSize + 1];
				GetHelpText(_tacHandle, output, actualBufferSize + 1, ref actualBufferSize);
				result = Encoding.ASCII.GetString(output).Trim('\0');
			}

			return result;
		}

		public string deviceName
		{
			get { return GetName(); }
			set { SetName(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetName(uint tacHandle, byte[] portData, uint bufferSize);

		public string GetName()
		{
			string result = "";

			if (TestHandle())
			{
				byte[] output = new byte[kBufferSize];
				testResult(GetName(_tacHandle, output, kBufferSize));
				result = Encoding.ASCII.GetString(output).Trim('\0');	
			}

			return result;
		}

		public string firmwareVersion
		{
			get { return GetFirmwareVersion(); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetFirmwareVersion(uint tacHandle, byte[] portData, uint bufferSize); 
		
		public string GetFirmwareVersion()
		{
			string result = "";

			if (TestHandle())
			{
				byte[] output = new byte[kBufferSize];
				testResult(GetFirmwareVersion(_tacHandle, output, kBufferSize));
				result = Encoding.ASCII.GetString(output).Trim('\0');
			}

			return result;
		}

		public string hardware
		{
			get { return GetHardware(); }
		}
		[DllImport("TACDev.dll")]
		private static extern uint GetHardware(uint tacHandle, byte[] portData, uint bufferSize);

		public string GetHardware()
		{
			string result = "";

			if (TestHandle())
			{
				byte[] output = new byte[kBufferSize];
				testResult(GetHardware(_tacHandle, output, kBufferSize));
				result = Encoding.ASCII.GetString(output).Trim('\0');
			}

			return result;
		}

		public string hardwareVersion
		{
			get { return GetHardwareVersion(); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetHardwareVersion(uint tacHandle, byte[] portData, uint bufferSize);

		public string GetHardwareVersion()
		{
			string result = "";

			if (TestHandle())
			{
				byte[] output = new byte[kBufferSize];
				testResult(GetHardwareVersion(_tacHandle, output, kBufferSize));
				result = Encoding.ASCII.GetString(output).Trim('\0');
			}

			return result;
		}

		public string uuid
		{
			get { return GetUUID(); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetUUID(uint tacHandle, byte[] portData, uint bufferSize); 
		
		public string GetUUID()
		{
			string result = "";

			if (TestHandle())
			{
				byte[] output = new byte[kBufferSize];
				testResult(GetUUID(_tacHandle, output, kBufferSize));
				result = Encoding.ASCII.GetString(output).Trim('\0');
			}


			return result;
		}

		public uint buttonAssertTime
		{
			get { return GetButtonAssertTime(); }
			set { SetButtonAssertTime(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint SetButtonAssertTime(uint tacHandle, uint secondsToAssert);

		public bool SetButtonAssertTime(uint secondsToAssert)
		{
			bool result = false;

			if (secondsToAssert == 0)
				throw new TACException("Button Assert Time Can't be Zero");

			if (TestHandle())
				testResult(SetButtonAssertTime(_tacHandle, secondsToAssert));

			return result;
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetButtonAssertTime(uint tacHandle, ref uint secondsToAssert); 
		
		public uint GetButtonAssertTime()
		{
			uint result = 0;

			if (TestHandle())
				testResult(GetButtonAssertTime(_tacHandle, ref result));

			return result;
		}

		public bool externalPowerControl
		{
			set { SetExtPowerControl(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint SetExternalPowerControl(uint tacHandle, bool state); 

		public void SetExtPowerControl(bool state)
		{
			if (TestHandle())
				testResult(SetExternalPowerControl(_tacHandle, state));
		}

		public bool batteryState
		{
			get { return GetBatteryState(); }
			set { SetBatteryState(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint SetBatteryState(uint tacHandle, bool state);

		public void SetBatteryState(bool state)
		{
			if (TestHandle())
				testResult(SetBatteryState(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetBatteryState(uint tacHandle, ref bool state); 
		
		public bool GetBatteryState()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetBatteryState(_tacHandle, ref state));

			return state;
		}

		public bool usb0
		{
			get { return GetUsb0State(); }
			set { Usb0(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint Usb0(uint tacHandle, bool state);

		public void Usb0(bool state)
		{
			if (TestHandle())
				testResult(Usb0(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetUsb0State(uint tacHandle, ref bool state);

		public bool GetUsb0State()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetUsb0State(_tacHandle, ref state));

			return state;
		}

		public bool usb1
		{
			get { return GetUsb1State(); }
			set { Usb1(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint Usb1(uint tacHandle, bool state);

		public void Usb1(bool state)
		{
			if (TestHandle())
				testResult(Usb1(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetUsb1State(uint tacHandle, ref bool state);

		public bool GetUsb1State()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetUsb1State(_tacHandle, ref state));

			return state;
		}

		public bool powerKey
		{
			get { return GetPowerKeyState(); }
			set { PowerKey(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint PowerKey(uint tacHandle, bool state);

		public void PowerKey(bool state)
		{
			if (TestHandle())
				testResult(PowerKey(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetPowerKeyState(uint tacHandle, ref bool state);

		public bool GetPowerKeyState()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetPowerKeyState(_tacHandle, ref state));

			return state;
		}

		public bool volumeUp
		{
			get { return GetVolumeUpState(); }
			set { VolumeUp(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint VolumeUp(uint tacHandle, bool state);

		public void VolumeUp(bool state)
		{
			if (TestHandle())
				testResult(VolumeUp(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetVolumeUpState(uint tacHandle, ref bool state);

		public bool GetVolumeUpState()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetVolumeUpState(_tacHandle, ref state));

			return state;
		}

		public bool volumeDown
		{
			get { return GetVolumeDownState(); }
			set { VolumeDown(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint VolumeDown(uint tacHandle, bool state);

		public void VolumeDown(bool state)
		{
			if (TestHandle())
				testResult(VolumeDown(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetVolumeDownState(uint tacHandle, ref bool state);

		public bool GetVolumeDownState()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetVolumeDownState(_tacHandle, ref state));

			return state;
		}

		public bool uim1Disconnect
		{
			get { return GetDisconnectUIM1State(); }
			set { DisconnectUIM1(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint DisconnectUIM1(uint tacHandle, bool state);

		public void DisconnectUIM1(bool state)
		{
			if (TestHandle())
				testResult(DisconnectUIM1(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetDisconnectUIM1State(uint tacHandle, ref bool state);

		public bool GetDisconnectUIM1State()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetDisconnectUIM1State(_tacHandle, ref state));

			return state;
		}

		public bool uim2Disconnect
		{
			get { return GetDisconnectUIM2State(); }
			set { DisconnectUIM2(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint DisconnectUIM2(uint tacHandle, bool state);

		public void DisconnectUIM2(bool state)
		{
			if (TestHandle())
				testResult(DisconnectUIM2(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetDisconnectUIM2State(uint tacHandle, ref bool state);

		public bool GetDisconnectUIM2State()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetDisconnectUIM2State(_tacHandle, ref state));

			return state;
		}

		public bool disconnectSDCard
		{
			get { return GetDisconnectSDCardState(); }
			set { DisconnectSDCard(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint DisconnectSDCard(uint tacHandle, bool state);

		public void DisconnectSDCard(bool state)
		{
			if (TestHandle())
				testResult(DisconnectSDCard(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetDisconnectSDCardState(uint tacHandle, ref bool state);

		public bool GetDisconnectSDCardState()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetDisconnectSDCardState(_tacHandle, ref state));

			return state;
		}

		public bool primaryEDL
		{
			get { return GetPrimaryEDLState(); }
			set { PrimaryEDL(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint PrimaryEDL(uint tacHandle, bool state);

		public void PrimaryEDL(bool state)
		{
			if (TestHandle())
				testResult(PrimaryEDL(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetPrimaryEDLState(uint tacHandle, ref bool state);

		public bool GetPrimaryEDLState()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetPrimaryEDLState(_tacHandle, ref state));

			return state;
		}

		public bool secondaryEDL
		{
			get { return GetSecondaryEDLState(); }
			set { SecondaryEDL(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint SecondaryEDL(uint tacHandle, bool state);

		public void SecondaryEDL(bool state)
		{
			if (TestHandle())
				testResult(SecondaryEDL(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetSecondaryEDLState(uint tacHandle, ref bool state);

		public bool GetSecondaryEDLState()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetSecondaryEDLState(_tacHandle, ref state));

			return state;
		}

		public bool forcePSHoldHigh
		{
			get { return GetForcePSHoldHighState(); }
			set { ForcePSHoldHigh(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint ForcePSHoldHigh(uint tacHandle, bool state);

		public void ForcePSHoldHigh(bool state)
		{
			if (TestHandle())
				testResult(ForcePSHoldHigh(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetForcePSHoldHighState(uint tacHandle, ref bool state);

		public bool GetForcePSHoldHighState()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetForcePSHoldHighState(_tacHandle, ref state));

			return state;
		}

		public bool secondaryPmResinN
		{
			get { return GetSecondaryPmResinNState(); }
			set { SecondaryPmResinN(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint SecondaryPmResinN(uint tacHandle, bool state);

		public void SecondaryPmResinN(bool state)
		{
			if (TestHandle())
				testResult(SecondaryPmResinN(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetSecondaryPmResinNState(uint tacHandle, ref bool state);

		public bool GetSecondaryPmResinNState()
		{
			bool state = false;

			if (TestHandle())
				testResult(GetSecondaryPmResinNState(_tacHandle, ref state));

			return state;
		}

		public bool eud
		{
			get { return GetEUDState(); }
			set { Eud(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint Eud(uint tacHandle, bool state);

		public void Eud(bool state)
		{
			if (TestHandle())
				testResult(Eud(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetEUDState(uint tacHandle, ref bool state);

		public bool GetEUDState()
		{
			bool result = false;

			if (TestHandle())
				testResult(GetEUDState(_tacHandle, ref result));

			return result;
		}

		public bool headsetDisconnect
		{
			get { return GetHeadsetDisconnectState(); }
			set { HeadsetDisconnect(value); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint HeadsetDisconnect(uint tacHandle, bool state);

		public void HeadsetDisconnect(bool state)
		{
			if (TestHandle())
				testResult(HeadsetDisconnect(_tacHandle, state));
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetHeadsetDisconnectState(uint tacHandle, ref bool state);

		public bool GetHeadsetDisconnectState()
		{
			bool result = false;

			if (TestHandle())
				testResult(GetHeadsetDisconnectState(_tacHandle, ref result));

			return result;
		}

		[DllImport("TACDev.dll")]
		private static extern uint SetName(uint tacHandle, string newName);

		public void SetName(string newName)
		{
			if (TestHandle())
				testResult(SetName(_tacHandle, newName));
		}

		public uint resetCount
		{
			get { return GetResetCount(); }
		}

		[DllImport("TACDev.dll")]
		private static extern uint GetResetCount(uint tacHandle, ref uint resetCount);

		public uint GetResetCount()
		{
			uint result = 0;

			if (TestHandle())
				testResult(GetResetCount(_tacHandle, ref result));

			return result;
		}

		[DllImport("TACDev.dll")]
		private static extern uint ClearResetCount(uint tacHandle);

		public void ClearResetCount()
		{
			if (TestHandle())
				testResult(ClearResetCount(_tacHandle));
		}

		[DllImport("TACDev.dll")]
		private static extern uint PowerOnButton(uint tacHandle);

		public void PowerOnButton()
		{
			if (TestHandle())
				testResult(PowerOnButton(_tacHandle));
		}

		[DllImport("TACDev.dll")]
		private static extern uint PowerOffButton(uint tacHandle);

		public void PowerOffButton()
		{
			if (TestHandle())
				testResult(PowerOffButton(_tacHandle));
		}

		[DllImport("TACDev.dll")]
		private static extern uint BootToFastBootButton(uint tacHandle);

		public void BootToFastBootButton()
		{
			if (TestHandle())
				testResult(BootToFastBootButton(_tacHandle));
		}

		[DllImport("TACDev.dll")]
		private static extern uint BootToUEFIMenuButton(uint tacHandle);

		public void BootToUEFIMenuButton()
		{
			if (TestHandle())
				testResult(BootToUEFIMenuButton(_tacHandle));
		}

		[DllImport("TACDev.dll")]
		private static extern uint BootToEDLButton(uint tacHandle);

		public void BootToEDLButton()
		{
			if (TestHandle())
				testResult(BootToEDLButton(_tacHandle));
		}

		[DllImport("TACDev.dll")]
		private static extern uint BootToSecondaryEDLButton(uint tacHandle);

		public void BootToSecondaryEDLButton()
		{
			if (TestHandle())
				testResult(BootToSecondaryEDLButton(_tacHandle));
		}

		// Some Handy Constants
		private const uint kBufferSize = 1024;
		private const uint BAD_TAC_HANDLE = 0;

		private const uint NO_TAC_ERROR = 0;
		private const uint TAC_BUFFER_TOO_SMALL = 1;
		private const uint TAC_BAD_TAC_HANDLE = 2;
		private const uint TAC_CMD_NOT_FOUND = 3;
		private const uint TAC_BAD_INDEX = 4; 
		private const uint TACDEV_INIT_FAILED = 5;

		// Instance variables
		private uint _tacHandle = 0;
		private string _portName;
		private string _description;
		private string _serialNumber;

		static private void testResult(uint tacResult)
		{
			switch (tacResult)
			{
				case NO_TAC_ERROR:
					break;

				case TAC_BAD_TAC_HANDLE:
					throw new TACException("Bad TAC Handle");

				case TAC_BUFFER_TOO_SMALL:
					throw new TACException("TAC Buffer Too Small");

				case TAC_CMD_NOT_FOUND:
					throw new TACException("TAC Command Not Found");

				case TAC_BAD_INDEX:
					throw new TACException("TAC Bad Index");

				case TACDEV_INIT_FAILED:
					throw new TACException("TACInit Failed");

				default:
					throw new TACException("TAC Error: " + tacResult);
			}
		}

		private bool TestHandle()
		{
			bool result = false;

			if (_tacHandle != BAD_TAC_HANDLE)
			{
				result = true;
			}
			else
			{
				throw new TACException("Handle is bad.  Is the TAC Device is closed?");
			}

			return result;
		}
	}
}
