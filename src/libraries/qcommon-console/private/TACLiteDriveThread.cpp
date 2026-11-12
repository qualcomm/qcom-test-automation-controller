// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACLiteDriveThread.h"

#include "TACCommands.h"
#include "AlpacaDevice.h"
#include "TACCommandHashes.h"
#include "TACLiteCommand.h"

// QCommon
#include "AppCore.h"

// Qt
#include <QDir>
#include <QDateTime>
#include <QTextStream>

#define on true
#define off false

const std::string kTACLiteDriveTrainName{"TAC Lite Drive Train"};

TACLiteDriveThread::TACLiteDriveThread
(
	uint hash
) :
	TACDriveThread(hash)
{
	_driveTrainName = kTACLiteDriveTrainName;
	setProtocolInterface(&_tacProtocol);
	_tacProtocol.setTACDriveTrain(this);

	AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);

	if (alpacaDevice->active())
	{
		_ftdiChipset = _FTDIChipset::getDevice(QByteArray::fromStdString(alpacaDevice->portName()));
		if (_ftdiChipset.isNull() == false)
		{
			_portName = _ftdiChipset->portName();
			_platformID = _ftdiChipset->platformID();
			_name = _ftdiChipset->serialNumber();

			_versionString = _ftdiChipset->versionString().toLatin1();
			_firmwareString = _ftdiChipset->firmwareString().toStdString();
			_hardwareType = _ftdiChipset->boardType();
			_firmwareMajor = 1;
			_firmwareChip = 10000;
			_firmwareMinor = 1;
			_firmwareRevision = 1;
			_description = "VTP Port";
			_uuid = "FTDI - No UUID";

			setSerialNumber(_ftdiChipset->serialNumber());
			setPortName(_ftdiChipset->portName());

			connect(this, &TACDriveThread::deviceDisconnected, alpacaDevice.data(), &_AlpacaDevice::onDeviceDisconnect, Qt::DirectConnection);
			connect(this, &TACDriveThread::progress, alpacaDevice.data(), &_AlpacaDevice::progress);
		}
	}
}

TACLiteDriveThread::~TACLiteDriveThread()
{
}

bool TACLiteDriveThread::openFTDIDevice()
{
	bool result{false};

	if (_ftdiChipset.isNull() == false)
	{
		if (_ftdiChipset->isOpen() == false)
		{
			_ftdiChipset->setBoardType(_hardwareType);
			if (_ftdiChipset->open(_pinsets))
			{
				AppCore::writeToApplicationLogLine("Opened port " + _ftdiChipset->portName().toStdString() + "\n");

				result = true;
			}
			else
			{
				AppCore::writeToApplicationLogLine("TACLiteDriveThread::openFTDIDevice _ftdiDevice->open(pinsets) failed");
				emit errorOnOpen("FTDI device open failed. Check the application log");
			}
		}
		else
		{
			AppCore::writeToApplicationLogLine("TACLiteDriveThread::openFTDIDevice _ftdiDevice->isOpen() == false");
			emit errorOnOpen("FTDI device open failed. Check the application log");
		}
	}
	else
	{
		AppCore::writeToApplicationLogLine("TACLiteDriveThread::openFTDIDevice _ftdiDevice.isNull() == false");
		emit errorOnOpen("FTDI device open failed. Check the application log");
	}

	return result;
}

void TACLiteDriveThread::externalPowerControl
(
	bool state
)
{
	{
		TACLiteCommand tacCommand(this, this);

		tacCommand.externalPowerControl(state);
	}

	waitForCompletion();
}

void TACLiteDriveThread::setPinState
(
	uint16_t pin,
	bool state
)
{
	{
		TACLiteCommand tacCommand(this, this);

		tacCommand.setPinState(pin, state);
	}

	waitForCompletion();
}

void TACLiteDriveThread::sendCommandSequence
(
	CommandEntries& commandEntries
)
{
	{
		TACLiteCommand tacCommand(this, this);

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
				tacCommand.addLogComment(QByteArray::fromStdString(std::get<std::string>(commandEntry->_arguement)));
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

int TACLiteDriveThread::getResetCount()
{
	return 0;
}

void TACLiteDriveThread::clearResetCount()
{
}

void TACLiteDriveThread::i2CReadRegister
(
	uint32_t addr,
	uint32_t reg
)
{
	Q_UNUSED(addr)
	Q_UNUSED(reg)
}

void TACLiteDriveThread::i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)
{
	Q_UNUSED(addr)
	Q_UNUSED(reg)
	Q_UNUSED(data)
}

void TACLiteDriveThread::setName
(
	const QByteArray& newName
)
{
	Q_UNUSED(newName)
}

uint32_t TACLiteDriveThread::send
(
	const QByteArray& sendMe,
	const Arguments& arguments,
	bool console,
	ReceiveInterface* recieveInterface,
	bool store
)
{
	return _tacProtocol.sendCommand(sendMe, arguments, console, recieveInterface, store);
}

bool TACLiteDriveThread::ready()
{
	return _tacProtocol.queueSize() == 0;
}

void TACLiteDriveThread::receive(FramePackage &framePackage)
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
		case kVersionCommandHash:
			handleVersionResponse(framePackage);
			break;

		case kGetPlatformIDCommandHash:
			handlePlatformID(framePackage);
			break;

		case kGetUUIDCommandHash:
			handleUUIDResponse(framePackage);
			break;

		case kSetNameCommandHash:
			handleSetName(framePackage);
			break;

		case kSetPinCommandHash:
			handleSetPin(framePackage);
			break;

		default:
			break;
		}
	}
	else
	{
		clearWaitForCompletion();

		if (framePackage->_lastError.isEmpty() == false)
			emit errorOnOpen(framePackage->_lastError);
	}

	log(framePackage);

	_protocolInterface->clearPendingFrame();
}

void TACLiteDriveThread::run()
{
	AppCore::writeToApplicationLogLine("TACLiteDriveThread::run()");

	if (openFTDIDevice() == true)
	{
		AppCore::writeToApplicationLogLine("TACLiteDriveThread::run() openFTDIDevice() == true");
		emit deviceOpen();
		startRunning();
	}

	if (weAreRunning())
	{
		emit hardwareTypeUpdate(_versionString);
		emit hardwareVersionUpdate("0.1.1");	
		emit firmwareVersionUpdate(QString::fromStdString(_firmwareString));

		emit nameUpdate(_name);
		emit uuidUpdate(_uuid);
		emit serialNumUpdate(_serialNumber);
		emit platformIDUpdate(_platformID);

		emit deviceOpen();
		emit deviceStatusChange(tr("Starting"));

		AppCore::writeToApplicationLogLine("setupConnected()");

		setupConnected();
	}
	else
	{
		AppCore::writeToApplicationLogLine("TACLiteDriveThread::run() emit deviceDisconnected()");
		emit deviceDisconnected();
	}

	AppCore::writeToApplicationLogLine("Hardware: " + std::to_string(static_cast<int>(_hardwareType)) + "\n");
	AppCore::writeToApplicationLogLine("Platform ID: " + PlatformContainer::toString(_platformID) + "(" + std::to_string(static_cast<int>(_platformID)) + ")\n");

	if (weAreRunning())
	{
		bool loopFinished{false};

		AppCore::writeToApplicationLogLine("TACLiteDriveThread::run() starting loop");

		while (!loopFinished)
		{
			FramePackage framePackage = _protocolInterface->getNextFramePackage();
			if (framePackage.isNull() == false)
			{
				if (framePackage->_delayInMilliSeconds != 0 ||
					framePackage->_comment.isEmpty() == false ||
					framePackage->_endTransaction == true ||
					checkLocalStore(framePackage) == true)
				{
					receive(framePackage);
				}
				else
				{

					int pin = framePackage->_codedRequest.toInt();
					bool state = getElectrialPinValue(framePackage->_requestHash, framePackage->_arguments);

					AppCore::writeToApplicationLogLine("TACLiteDriveThread::run() write " + framePackage->_codedRequest.toStdString() + " state:" + (state ? "on" : "off"));

					if (_ftdiChipset.isNull() == false)
					{
						if (_ftdiChipset->write(pin, state) == false)
						{
							std::string logMessage = "TACLiteDriveThread::run() write failure pin " + std::to_string(pin);

							AppCore::writeToApplicationLogLine(logMessage);

							stopRunning();
						}
						else
						{
							QByteArray argument;

							if (std::holds_alternative<bool>(framePackage->_arguments.at(0)))
							{
								argument = std::get<bool>(framePackage->_arguments.at(0)) ? "on" : "off";
							}
							else if (std::holds_alternative<std::string>(framePackage->_arguments.at(0)))
							{
								argument = QByteArray::fromStdString(std::get<std::string>(framePackage->_arguments.at(0)));
							}

							framePackage->_responses.push_back(framePackage->_request + " " + argument);

							receive(framePackage);
						}
					}
					else
					{
						AppCore::writeToApplicationLogLine("TACLiteDriveThread::run() ftdiChipSet is null.");
						stopRunning();
					}
				}
			}
			else
			{
				if (weAreRunning() == false)
				{
					AppCore::writeToApplicationLogLine("TACLiteDriveThread::run() loopFinished == true\n");
					loopFinished = true;
				}
				else
				{
					msleep(100);
				}
			}
		}
	}

	AppCore::writeToApplicationLogLine("TACLiteDriveThread::run() _ftdiDevice->close()");

	if (_ftdiChipset.isNull() == false)
		_ftdiChipset->close();

	_connected = false;

	emit deviceDisconnected();

	shutdownLogging();
}

void TACLiteDriveThread::sendCommand
(
	const QByteArray &command,
	bool console,
	ReceiveInterface *receiveInterface,
	bool shouldStore
)
{
	if (command.startsWith(kHelpCommand) == false)
	{
		Arguments args;
		QByteArray decodedCommand = decodeCommand(command, args);

		send(decodedCommand, args, console, receiveInterface, shouldStore);
	}
}

bool TACLiteDriveThread::getElectrialPinValue
(
	uint32_t kCommandHash,
	const Arguments& arguments
)
{
	bool result{0};
	bool argValue = std::get<bool>(arguments.at(0));
	switch (kCommandHash)
	{
	case kSetPinCommandHash:
		result = argValue;
	}

	return result;
}

void TACLiteDriveThread::handleSetName
(
	FramePackage& framePackage
)
{
	Argument argument{framePackage->getArgument(0)};

	if (std::holds_alternative<std::string>(argument))
	{
		_name = QByteArray::fromStdString(std::get<std::string>(argument));
		framePackage->_synonym = "Set Name " + _name;
	}

	emit nameUpdate(_name);
}

void TACLiteDriveThread::handleSetPin
(
	FramePackage &framePackage
)
{
	bool state = std::get<bool>(framePackage->getArgument(0));
	uint32_t pin = std::get<uint32_t>(framePackage->getArgument(1));

	framePackage->_synonym = "Set Pin " + QByteArray::number(pin) + " " + QByteArray(state ? "true" : "false");

	emit pinStateChanged(pin, state);
}

void TACLiteDriveThread::handleUUIDResponse
(
	FramePackage& framePackage
)
{
	if (framePackage->_responses.count() > 1)
	{
		QByteArray response = framePackage->_responses.at(1);
		_uuid = response;

		emit uuidUpdate(_uuid);
	}
}

void TACLiteDriveThread::handleVersionResponse
(
	FramePackage& framePackage
)
{
	Q_UNUSED(framePackage)
	emit deviceStatusChange(tr("TAC Version Good"));

	setupConnected();
}

void TACLiteDriveThread::handlePlatformID
(
	FramePackage& framePackage
)
{
	if (framePackage->_responses.count() >= 2)
	{
		bool okay{false};

		int boardID = framePackage->_responses.at(1).toInt(&okay);
		if (okay)
		{
			_platformID = static_cast<PlatformID>(boardID);
		}
	}

	emit platformIDUpdate(static_cast<int>(_platformID));

	setupConnected();
}

void TACLiteDriveThread::setupConnected()
{
	if (_connected == false)
	{
		_connected = true;

		emit deviceConnected();
	}
}

void TACLiteDriveThread::setupDiscovery()
{
		// nothing to be done here
}
