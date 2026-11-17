// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACSTM32DriveThread.h"

#include "AlpacaDevice.h"
#include "STM32Device.h"
#include "TACCommandHashes.h"
#include "TACSTM32Command.h"

// QCommon
#include "AppCore.h"

#include <chrono>
#include <string>
#include <thread>

const std::string kTACSTM32DriveTrainName{"TAC STM32 Drive Train"};

TACSTM32DriveThread::TACSTM32DriveThread
(
	uint32_t hash
) :
	TACDriveThread(hash)
{
	_driveTrainName = kTACSTM32DriveTrainName;
	setProtocolInterface(&_tacProtocol);
	_tacProtocol.setTACDriveTrain(this);

	AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);

	if (alpacaDevice && alpacaDevice->active())
	{
		_stm32Device = static_cast<STM32Device*>(alpacaDevice.get());

		_portName = alpacaDevice->portName();
		_platformID = alpacaDevice->platformID();
		_hardwareType = alpacaDevice->debugBoardType();
		_name = alpacaDevice->serialNumber();
		_description = "Arduino BugHopper V2 Board";
		_uuid = "Bug Hopper V2 - No UUID";

		setSerialNumber(alpacaDevice->serialNumber());
		setPortName(alpacaDevice->portName());
	}
}

TACSTM32DriveThread::~TACSTM32DriveThread()
{
}

bool TACSTM32DriveThread::openSTM32Device()
{
	bool result{false};

	if (_stm32Device != nullptr)
	{
		result = _stm32Device->openTransport();

		if (result == false)
		{
			if (onError)
				onError("Arduino BugHopper V2 device open failed. Check the application log");
		}
	}
	else
	{
		AppCore::writeToApplicationLogLine("TACSTM32DriveThread::openSTM32Device _stm32Device is null");
		if (onError)
			onError("Arduino BugHopper V2 device open failed. Check the application log");
	}

	return result;
}

void TACSTM32DriveThread::setPinState(uint16_t pin, bool state)
{
	{
		TACSTM32Command tacCommand(this, this);

		tacCommand.setPinState(pin, state);
	}

	waitForCompletion();
}

void TACSTM32DriveThread::sendCommandSequence
(
	CommandEntries& commandEntries
)
{
	{
		TACSTM32Command tacCommand(this, this);

		for (const auto& commandEntry: commandEntries)
		{
			if (!commandEntry)
				continue;

			switch (commandEntry->_commandAction)
			{
			case _CommandEntry::eNotSet:
				break;

			case _CommandEntry::eSetPin:
			{
				bool st = false;
				if (std::holds_alternative<bool>(commandEntry->_arguement))
					st = std::get<bool>(commandEntry->_arguement);
				else if (std::holds_alternative<uint32_t>(commandEntry->_arguement))
					st = (std::get<uint32_t>(commandEntry->_arguement) != 0);
				tacCommand.setPinState(commandEntry->_pinID, st);
				break;
			}

			case _CommandEntry::eLog:
			{
				std::string comment;
				if (std::holds_alternative<std::string>(commandEntry->_arguement))
					comment = std::get<std::string>(commandEntry->_arguement);
				tacCommand.addLogComment(comment);
				break;
			}

			case _CommandEntry::eDelay:
			{
				uint32_t ms = 0;
				if (std::holds_alternative<uint32_t>(commandEntry->_arguement))
					ms = std::get<uint32_t>(commandEntry->_arguement);
				else if (std::holds_alternative<std::string>(commandEntry->_arguement))
					toUInt32(std::get<std::string>(commandEntry->_arguement), ms);
				tacCommand.addDelay(ms);
				break;
			}

			case _CommandEntry::eBaseCommand:
				break;
			}

			if (onProgress)
				onProgress(kProgressActive);
		}
	}

	waitForCompletion();
}

int TACSTM32DriveThread::getResetCount()
{
	return 0;
}

void TACSTM32DriveThread::clearResetCount()
{
}

void TACSTM32DriveThread::i2CReadRegister
(
	uint32_t addr,
	uint32_t reg
)
{
	(void)addr;
	(void)reg;
}

void TACSTM32DriveThread::i2CWriteRegister
(
	uint32_t addr,
	uint32_t reg,
	uint32_t data
)
{
	(void)addr;
	(void)reg;
	(void)data;
}

void TACSTM32DriveThread::setName
(
	const std::string& newName
)
{
	(void)newName;
}

void TACSTM32DriveThread::sendCommand
(
	const std::string& command,
	bool console, // = false
	ReceiveInterface* receiveInterface, // = nullptr
	bool shouldStore // = true
)
{
	(void)console;
	(void)receiveInterface;
	(void)shouldStore;

	Arguments args;
	std::string decodedCommand = decodeCommand(command, args);

	send(decodedCommand, args, console, receiveInterface, shouldStore);
}

uint32_t TACSTM32DriveThread::send
(
	const std::string& sendMe,
	const Arguments& arguments,
	bool console,
	ReceiveInterface* recieveInterface,
	bool store
)
{
	uint32_t res = _tacProtocol.sendCommand(sendMe, arguments, console, recieveInterface, store);
	wake();
	return res;
}

bool TACSTM32DriveThread::ready()
{
	return _tacProtocol.queueSize() == 0;
}

void TACSTM32DriveThread::receive
(
	FramePackage& framePackage
)
{
	if (framePackage->_endTransaction == true)
	{
		clearWaitForCompletion();

		if (onProgress)
			onProgress(kProgressMax);
	}
	else if (framePackage->_valid == true)
	{
		switch (framePackage->_requestHash)
		{
		case kSetPinCommandHash:
			{
				bool state = std::get<bool>(framePackage->getArgument(0));
				uint32_t pin = std::get<uint32_t>(framePackage->getArgument(1));

				framePackage->_synonym = "Set Pin " + std::to_string(pin) + " " + (state ? "true" : "false");

				if (onPinStateChanged)
					onPinStateChanged(pin, state);
			}
			break;

		default:
			break;
		}
	}
	else
	{
		clearWaitForCompletion();

		std::string errMsg = framePackage->_lastError.empty() ?
			("TACSTM32DriveThread: Command failed: " + framePackage->_request) :
			framePackage->_lastError;
		AppCore::writeToApplicationLogLine(errMsg);
		if (onError)
			onError(errMsg);
	}

	log(framePackage);

	_tacProtocol.clearPendingFrame();
}

void TACSTM32DriveThread::run()
{
	AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run()");

	if (openSTM32Device() == true)
	{
		AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run() openSTM32Device() == true");
		if (onDeviceOpen)
			onDeviceOpen();
		startRunning();
	}

	if (weAreRunning())
	{
		if (onDeviceStatusChange)
			onDeviceStatusChange("Starting");

		setupConnected();
	}
	else
	{
		AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run() dispatchDeviceDisconnected()");
		if (onDeviceDisconnected)
			onDeviceDisconnected();
	}

	if (weAreRunning())
	{
		bool loopFinished{false};

		AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run() starting loop");

		while (!loopFinished)
		{
			FramePackage framePackage = _tacProtocol.getNextFramePackage();
			if (framePackage != nullptr)
			{
				if (framePackage->_delayInMilliSeconds != 0 ||
					!framePackage->_comment.empty() ||
					framePackage->_endTransaction == true ||
					checkLocalStore(framePackage) == true)
				{
					receive(framePackage);
				}
				else if (_stm32Device != nullptr)
				{
					if (_stm32Device->write(framePackage->_codedRequest) == true)
					{
						receive(framePackage);
					}
					else
					{
						AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run() write failure for request " + framePackage->_request);

						framePackage->_valid = false;
						framePackage->_lastError = "BugHopper V2 write failed";

						receive(framePackage);
					}
				}
				else
				{
					AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run() _stm32Device is null.");

					stopRunning();
				}
			}
			else
			{
				if (weAreRunning() == false || stopRequested())
				{
					AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run() loopFinished == true\n");
					loopFinished = true;
				}
				else
				{
					sleepForOrStop(std::chrono::milliseconds(5));
				}
			}
		}
	}

	AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run() closeTransport()");

	if (_stm32Device != nullptr)
	{
		_stm32Device->close();
	}

	_connected = false;

	if (onDeviceDisconnected)
		onDeviceDisconnected();

	shutdownLogging();
}

void TACSTM32DriveThread::setupConnected()
{
	if (_connected == false)
	{
		_connected = true;

		if (onDeviceConnected)
			onDeviceConnected();
	}
}

void TACSTM32DriveThread::setupDiscovery()
{
	// nothing to be done here
}
