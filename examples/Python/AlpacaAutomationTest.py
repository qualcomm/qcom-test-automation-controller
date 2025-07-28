#!/usr/bin/env python3

import time
import TACDev

def main():	
	alpacaVersion = TACDev.AlpacaVersion()
	print("QTAC Version: ", alpacaVersion)

	tacVersion = TACDev.TACVersion()
	print("TAC Version: ", tacVersion)

	deviceCount  = TACDev.GetDeviceCount()
	if (deviceCount > 0):
		tacDevice = TACDev.GetDevice(0)
		if tacDevice is not None:
			if tacDevice.Open():
				print(f"Power cycling the TAC device with port name: '{tacDevice.PortName()}' and serial number: '{tacDevice.SerialNumber()}'")

				batteryState = tacDevice.GetBatteryState()
				print(f"Current battery state: {batteryState}")

				tacDevice.SetBatteryState(not batteryState)
				batteryState = tacDevice.GetBatteryState()
				print(f"Updated battery state to: {batteryState}")
				
				time.sleep(2)

				tacDevice.SetBatteryState(not batteryState)
				batteryState = tacDevice.GetBatteryState()
				print(f"Updated battery state to: {batteryState}")

				scriptVarCount = tacDevice.GetScriptVariableCount()

				scriptVariables = []
				for idx in range(scriptVarCount):
					scriptVariableEntry = tacDevice.GetScriptVariable(idx)
					print(f"Script variable at index {idx}: {scriptVariableEntry}")
					scriptVariables.append(scriptVariableEntry.split(";")[0])

				if scriptVarCount > 0 and "edl" in scriptVariables:
					tacDevice.UpdateScriptVariableValue("edl", "2000")
					print("Updated the 'edl' script variable value to 2000")

				tacDevice.Close()
			else:
				print("Device " + tacDevice.PortName() + " can't be opened.")
		else:
			print("Device " + tacDevice.PortName() + " not found.")

if __name__ == '__main__':
	main()
