// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PIC32CXDevice.h"

#include "private/TACPIC32CXDriveThread.h"
#include "SerialPortInfo.h"

#include "AppCore.h"
#include "StringUtilities.h"

#include <chrono>
#include <string>
#include <thread>

const std::string kPIC32CXDescription{"ALPACA PIC32CX Debug Board"};

PIC32CXDevice::~PIC32CXDevice()
{
}

uint32_t PIC32CXDevice::updateAlpacaDevices()
{
	SerialPortInfos availablePorts = SerialPortInfo::availablePorts();
	for (const auto& serialInfo : availablePorts)
	{
		if (serialInfo.hasVendorIdentifier() && serialInfo.hasProductIdentifier())
		{
			if (serialInfo.vendorIdentifier() != 0x04D8 || serialInfo.productIdentifier() != 0x000A)
				continue;
		}

		std::string portName = serialInfo.portName();
		HashType hash = ::arrayHash(portName);

		AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
		if (alpacaDevice == nullptr)
		{
			PIC32CXDevice* pic32cxDevice = new PIC32CXDevice;

			pic32cxDevice->_portName = portName;
			pic32cxDevice->_hash = hash;

			pic32cxDevice->_active = true;
			pic32cxDevice->_boardType = ePIC32CXAuto;
			pic32cxDevice->_platformID = ALPACA_PIC32CX_ID;
			pic32cxDevice->_description = kPIC32CXDescription;

			std::string serialNumber = serialInfo.serialNumber();
			pic32cxDevice->_serialNumber = serialNumber;

			size_t xxPos = serialNumber.find("XX");
			if (xxPos != std::string::npos)
			{
				pic32cxDevice->_usbDescriptor = serialNumber.substr(0, xxPos);

				if (_PlatformConfiguration::containsUSBDescriptor(pic32cxDevice->_usbDescriptor) == true)
					pic32cxDevice->_platformID = _PlatformConfiguration::getUSBDescriptor(pic32cxDevice->_usbDescriptor);
			}

			TACPlatformEntry platformEntry = _PlatformConfiguration::getEntry(pic32cxDevice->_platformID);

			pic32cxDevice->_description = platformEntry._platformEntry->_description;
			pic32cxDevice->_platformConfiguration = platformEntry.getConfiguration();

			_AlpacaDevice::_alpacaDevices.push_back(std::shared_ptr<PIC32CXDevice>(pic32cxDevice));

			AppCore::writeToApplicationLog("Found a PIC32CX device with port name: '" + portName + "'\n");
		}
		else
		{
			alpacaDevice->setActive();
		}
	}

	return static_cast<uint32_t>(_AlpacaDevice::_alpacaDevices.size());
}

bool PIC32CXDevice::open()
{
	bool result{false};

	if (_driveThread == nullptr)
	{
		_driveThread = new TACPIC32CXDriveThread(_hash);
		_driveThread->setThreadDelay(300);
		_driveThread->start();

		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // force DriveTrain thread to start faster

		for (int i{0}; i < 60; i++)
		{
			result = _driveThread->isRunning();
			if (result == true)
				break;
			if (_driveThread->hasFinished())
				break;
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}

		if (result == false)
		{
			std::string threadErr = _driveThread->lastErrorMessage();
			if (!threadErr.empty())
				_lastError = threadErr;
			else if (_lastError.empty())
				_lastError = "Failed to communicate with device on port " + portName();
		}
	}

	if (result == true)
	{
		_pic32cxPlatformConfiguration = static_cast<_PIC32CXPlatformConfiguration*>(_platformConfiguration.get());

		buildMapping();

		_driveThread->onPinStateChanged = [this](int pin, int state) {
			on_pinStateChanged(static_cast<uint64_t>(pin), state != 0);
		};
		_driveThread->onDeviceDisconnected = [this]() {
			onDeviceDisconnect();
		};
		_driveThread->onProgress = [this](int val) {
			if (onProgress) onProgress(val);
		};
		_driveThread->onError = [this](const std::string& err) {
			_lastError = err;
			if (onError) onError(err);
		};
		_driveThread->onDeviceStatusChange = [this](const std::string& status) {
			if (onDeviceStatusChange) onDeviceStatusChange(status);
		};
	}

	_macAddress = _driveThread->macAddress();
	_chipVersion = _driveThread->chipVersion();
	_driveThread->setThreadDelay(0);

	return result;
}

void PIC32CXDevice::buildMapping()
{
	if (_commands.empty() == true)
	{
		Pins pins = _platformConfiguration->getPins();
		for (const auto& pin: std::as_const(pins))
		{
			TACCommand tacCommand;

			tacCommand._pin = pin._pin;
			tacCommand._command = pin._pinCommand;
			tacCommand._helpText = pin._pinTooltip;
			tacCommand._tabName = pin._tabName;
			tacCommand._groupName = CommandGroup::toString(pin._commandGroup);

			tacCommand._cellLocation = fromPoint(pin._cellLocation);

			_commands[tacCommand._command] = tacCommand;
			_commandList.push_back(tacCommand);
		}
	}

	buildQuickSettings();
}
