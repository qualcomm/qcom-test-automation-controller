// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACSTM32DriveThread.h"

#include "AlpacaDevice.h"
#include "STM32Device.h"
#include "TACCommandHashes.h"
#include "TACSTM32Command.h"

// QCommon
#include "AppCore.h"

#include <string>

const std::string kTACSTM32DriveTrainName{"TAC STM32 Drive Train"};

TACSTM32DriveThread::TACSTM32DriveThread
(
	uint hash
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

		connect(this, &TACDriveThread::deviceDisconnected, alpacaDevice.get(), &_AlpacaDevice::onDeviceDisconnect, Qt::DirectConnection);
		connect(this, &TACDriveThread::progress, alpacaDevice.get(), &_AlpacaDevice::progress);
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
			emit errorOnOpen("Arduino BugHopper V2 device open failed. Check the application log");
	}
	else
	{
		AppCore::writeToApplicationLogLine("TACSTM32DriveThread::openSTM32Device _stm32Device is null");
		emit errorOnOpen("Arduino BugHopper V2 device open failed. Check the application log");
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
			switch (commandEntry->_commandAction)
			{
			case _CommandEntry::eNotSet:
				break;

			case _CommandEntry::eSetPin:
				tacCommand.setPinState(commandEntry->_pinID, std::get<bool>(commandEntry->_arguement));
				break;

			case _CommandEntry::eLog:
				tacCommand.addLogComment(std::get<std::string>(commandEntry->_arguement));
				break;

			case _CommandEntry::eDelay:
				tacCommand.addDelay(std::get<uint32_t>(commandEntry->_arguement));
				break;

			case _CommandEntry::eBaseCommand:
				break;
			}

			emit progress(kProgressActive);
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
	return _tacProtocol.sendCommand(sendMe, arguments, console, recieveInterface, store);
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

		emit transactionEnded();
		emit progress(kProgressMax);
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

				emit pinStateChanged(pin, state);
			}
			break;

		default:
			break;
		}
	}
	else
	{
		clearWaitForCompletion();

		if (!framePackage->_lastError.empty())
			emit errorOnOpen(framePackage->_lastError);
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
		emit deviceOpen();
		startRunning();
	}

	if (weAreRunning())
	{
		emit hardwareVersionUpdate(hardwareVersionString());
		emit nameUpdate(_name);
		emit uuidUpdate(_uuid);
		emit serialNumUpdate(_serialNumber);
		emit platformIDUpdate(static_cast<int>(_platformID));

		emit deviceStatusChange(tr("Starting"));

		setupConnected();
	}
	else
	{
		AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run() emit deviceDisconnected()");
		emit deviceDisconnected();
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
				if (weAreRunning() == false)
				{
					AppCore::writeToApplicationLogLine("TACSTM32DriveThread::run() loopFinished == true\n");
					loopFinished = true;
				}
				else
				{
					msleep(5);
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

	emit deviceDisconnected();

	shutdownLogging();
}

void TACSTM32DriveThread::setupConnected()
{
	if (_connected == false)
	{
		_connected = true;

		emit deviceConnected();
	}
}

void TACSTM32DriveThread::setupDiscovery()
{
	// nothing to be done here
}
