// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACPIC32CXDriveThread.h"
#include "AlpacaDevice.h"

// QCommon
#include "AppCore.h"
#include "PIC32CXSerialTableModel.h"
#include "StringUtilities.h"
#include "TACPIC32CXCommand.h"
#include "TACCommandHashes.h"

#include <cstdint>
#include <string>
#include <vector>

const std::string kTACSerialDriveTrainName("TAC PIC32CX Drive Train");

TACPIC32CXDriveThread::TACPIC32CXDriveThread(uint hash) :
	TACDriveThread(hash)
{
	_driveTrainName = kTACSerialDriveTrainName;
	setProtocolInterface(&_tacProtocol);
	_tacProtocol.setTACDriveTrain(this);

	AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
	if (alpacaDevice && alpacaDevice->active())
	{
		_hardwareType = ePIC32CXAuto;

		_firmwareMajor = 1;
		_firmwareMinor = 1;
		_firmwareRevision = 1;
		_description = "PIC32CX (Automotive) Board";
		_uuid = "PIC32CX - No UUID";

		_portName = alpacaDevice->portName();
		_serialNumber = alpacaDevice->serialNumber();
		_description = alpacaDevice->description();

		connect(this, &TACDriveThread::pinStateChanged, alpacaDevice.get(), &_AlpacaDevice::on_pinStateChanged);
		connect(this, &TACDriveThread::progress, alpacaDevice.get(), &_AlpacaDevice::progress);
	}
}

TACPIC32CXDriveThread::~TACPIC32CXDriveThread()
{
}

bool TACPIC32CXDriveThread::openSerialDevice()
{
	bool result(false);

	PIC32CXSerialTableModel tableModel;

	_tacPortInfo = tableModel.getSerialPortFromName(_portName);

	if (_tacPortInfo.isNull())
		_tacPortInfo = tableModel.getSerialPortFromSerialNumber(_portName);

	if (_tacPortInfo.isNull() == false)
	{
		SerialPortSettings serialSettings;

		_serialPort = new SerialPort(_tacPortInfo);

		serialSettings = _serialPort->getSerialPortSettings();
		serialSettings._baudRate = QSerialPort::Baud115200;
		serialSettings._timeout = 500;

		_serialPort->setSerialPortSettings(serialSettings);

		if (_serialPort->open(QIODevice::ReadWrite) == true)
		{
			setSerialNumber(_tacPortInfo.serialNumber().toStdString());
			setPortName(_tacPortInfo.portName().toStdString());

			emit serialNumUpdate(_serialNumber);

			AppCore::writeToApplicationLogLine("Device " + _serialNumber + " opened");
			AppCore::writeToApplicationLogLine("Com port " + portName() + "\n");

			{
				TACPIC32CXCommand tacCommand(this, this);
				tacCommand.clearBuffer();
			}

			result = true;
		}
		else
		{
			_lastErrorMessage = "Unable to open TAC Port. " + serialPortError();

			AppCore::writeToApplicationLogLine(_lastErrorMessage);

			emit errorOnOpen("PIC32CX Device Open Failed. Check the Application Log");

			delete _serialPort;
			_serialPort = nullptr;
		}
	}

	return result;
}

void TACPIC32CXDriveThread::sendCommand
(
	const std::string& command,
	bool console, // = false
	ReceiveInterface* receiveInterface, // = nullptr
	bool shouldStore // = true
)
{
	Arguments args;
	std::string decodedCommand = decodeCommand(command, args);

	send(decodedCommand, args, console, receiveInterface, shouldStore);
}

void TACPIC32CXDriveThread::setPinState(uint16_t pin, bool state)
{
	{
		TACPIC32CXCommand tacCommand(this, this);

		tacCommand.setPinState(pin, state);
	}

	waitForCompletion();
}

void TACPIC32CXDriveThread::endTransaction
(
	ReceiveInterface* receiveInterface
)
{
	_tacProtocol.endTransaction(receiveInterface);
}

void TACPIC32CXDriveThread::sendCommandSequence(CommandEntries& commandEntries)
{
	for (const auto& commandEntry: commandEntries)
	{
		{
			TACPIC32CXCommand tacCommand(this, this);

			switch (commandEntry->_commandAction)
			{
			case _CommandEntry::eNotSet:
				break;

			case _CommandEntry::eSetPin:
				tacCommand.setPinState(commandEntry->_pinID, std::get<bool>(commandEntry->_arguement));
				break;

			case _CommandEntry::eLog:
				tacCommand.addLogComment(std::get<std::string>(commandEntry->_arguement) + "\n");
				break;

			case _CommandEntry::eDelay:
				tacCommand.addDelay(std::get<uint32_t>(commandEntry->_arguement));
				break;

			case _CommandEntry::eBaseCommand:
				break;
			}
		}

		emit progress(kProgressActive);
	}
}

int TACPIC32CXDriveThread::getResetCount()
{
	waitForCompletion();

	return _resetCount;
}

void TACPIC32CXDriveThread::clearResetCount()
{
	waitForCompletion();
}

std::string TACPIC32CXDriveThread::serialPortError()
{
	std::string result;

	if (_serialPort != nullptr)
	{
		switch (_serialPort->error())
		{
		case QSerialPort::NoError:
			result = "No error occurred.";
			break;

		case QSerialPort::DeviceNotFoundError:
			result = "Device is non-existant.";
			break;

		case QSerialPort::PermissionError:
			result = "The device is already open in another process or the user does not have sufficient security credentials.";
			break;

		case QSerialPort::OpenError:
			result = "This object already has an open device..";
			break;

		case QSerialPort::NotOpenError:
			result = "This error occurs when an operation is executed that can only be successfully performed if the device is open.";
			break;

		case QSerialPort::WriteError:
			result = "An I/O error occurred while writing the data.";
			break;

		case QSerialPort::ReadError:
			result = "An I/O error occurred while reading the data.";
			break;

		case QSerialPort::ResourceError:
			result = "An I/O error occurred when a resource becomes unavailable, e.g. when the device is unexpectedly removed from the system.";
			break;

		case QSerialPort::UnsupportedOperationError:
			result = "The requested device operation is not supported or prohibited by the running operating system.";
			break;

		case QSerialPort::TimeoutError:
			result = "A timeout error occurred.";
			break;

		case QSerialPort::UnknownError:
		default:
			result = "An unidentified serial port error " + std::to_string(_serialPort->error()) + " has occurred.";
			break;
		}
	}

	return result;
}

void TACPIC32CXDriveThread::handleError(QSerialPort::SerialPortError error)
{
	switch(error)
	{
	case QSerialPort::DeviceNotFoundError:
		_lastErrorMessage = "Device Not Found.";
		break;
	case QSerialPort::PermissionError:
		_lastErrorMessage = "Permission Error.";
		break;
	case QSerialPort::OpenError:
		_lastErrorMessage = "Open Error.";
		break;
	case QSerialPort::WriteError:
		_lastErrorMessage = "Write Error.";
		break;
	case QSerialPort::ReadError:
		_lastErrorMessage = "Read Error.";
		break;
	case QSerialPort::ResourceError:
		_lastErrorMessage = "Resource Error.";
		break;
	case QSerialPort::UnsupportedOperationError:
		_lastErrorMessage = "Unsupported Operation Error.";
		break;
	case QSerialPort::UnknownError:
		_lastErrorMessage = "Unknown Error.";
		break;
	case QSerialPort::TimeoutError:
		_lastErrorMessage = "Timeout Error.";
		return;
	case QSerialPort::NotOpenError:
		_lastErrorMessage = "Not Open Error.";
		break;
	case QSerialPort::NoError:
		break;

	default:
		break;
	}
}

void TACPIC32CXDriveThread::on_readyRead()
{
	_readyRead = true;
}

void TACPIC32CXDriveThread::setName(const std::string& newName)
{
	if (newName.length() <= 32)
	{
		if (newName != _name)
		{
			if (isAlphaNumeric(newName) == true)
			{
				waitForCompletion();
			}
		}
	}
}

uint32_t TACPIC32CXDriveThread::send(const std::string& sendMe, const Arguments& arguments, bool console, ReceiveInterface* recieveInterface, bool store)
{
	return _tacProtocol.sendCommand(sendMe, arguments, console, recieveInterface, store);
}

bool TACPIC32CXDriveThread::ready()
{
	return _tacProtocol.queueSize() == 0;
}

bool TACPIC32CXDriveThread::readSerialData()
{
	bool result(false);

	if (_readyRead == true)
	{
		_readyRead = false;

		_serialBuffer.clear();

		std::string chunk;

		while (true)
		{
			chunk = _serialPort->readAll();
			if (!chunk.empty())
				_serialBuffer.append(chunk);
			else
				break;
		}

		if (!_serialBuffer.empty())
		{
#ifdef DEBUG
			AppCore::writeToApplicationLogLine(_serialBuffer);
#endif
			_protocolInterface->handleRecievedData(_serialBuffer);
			result = true;
		}
	}

	return result;
}

void TACPIC32CXDriveThread::receive(FramePackage& framePackage)
{
	if (framePackage->_endTransaction == true)
	{
		clearWaitForCompletion();
		emit transactionEnded();

		if (_protocolInterface->queueSize() == 0)
			emit progress(kProgressMax);
	}
	else
	{
		if (framePackage->_valid == true)
		{
			switch (framePackage->_requestHash)
			{
			case kPIC32CXVersionCommandHash: handleVersionResponse(framePackage); break;
			case kPIC32CXSetPinCommandHash: handleSetPin(framePackage); break;
			case kPIC32CXClearBufferHash: handleClearBuffer(framePackage); break;
			default:
				break;
			}
		}
		else
		{
			clearWaitForCompletion();

			switch (framePackage->_requestHash)
			{
				case kPIC32CXVersionCommandHash:
				{
					TACPIC32CXCommand tacCommand(this, this);
					tacCommand.platformID();
					break;
				}
				default:
					break;
			}
		}
	}

	log(framePackage);

	_protocolInterface->clearPendingFrame();
}

void TACPIC32CXDriveThread::i2CReadRegister(uint32_t addr, uint32_t reg)
{
	(void)addr;
	(void)reg;
}

void TACPIC32CXDriveThread::i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)
{
	(void)addr;
	(void)reg;
	(void)data;
}

void TACPIC32CXDriveThread::run()
{
	Q_ASSERT(_protocolInterface != nullptr);

	AppCore::getAppCore()->setRunLogging(AppCore::getAppCore()->getPreferences()->loggingActive());

	if (openSerialDevice() == true)
	{
		emit deviceOpen();

		connect(_serialPort, &QSerialPort::errorOccurred, this, &TACPIC32CXDriveThread::handleError, Qt::DirectConnection);
		connect(_serialPort, &QSerialPort::readyRead, this, &TACPIC32CXDriveThread::on_readyRead, Qt::DirectConnection);

		startRunning();

		emit deviceStatusChange(tr("Starting"));

		_readyRead = false;
		_connected = false;

		{
			TACPIC32CXCommand tacCommand(this, this);
			tacCommand.platformID();
		}

		if (weAreRunning())
		{
			bool loopFinished{false};

			while (!loopFinished)
			{
				FramePackage framePackage = _protocolInterface->getNextFramePackage();
				if (framePackage != nullptr)
				{
					if (framePackage->_delayInMilliSeconds != 0 ||
						!framePackage->_comment.empty() ||
						framePackage->_endTransaction == true ||
						checkLocalStore(framePackage) == true)
					{
						receive(framePackage);
					}
					else
					{
#ifdef DEBUG
						AppCore::writeToApplicationLogLine("TACPIC32CXDriveTrain::run()::Write: " + framePackage->_codedRequest);
#endif

						// No funny stuff in serial buffer when executing this command
						if (framePackage->_requestHash == kPIC32CXVersionCommandHash)
						{
							bool status = _serialPort->clear(QSerialPort::AllDirections);

							if (status)
								AppCore::writeToApplicationLogLine("Buffer cleared before identifying PIC32CX board");
						}

						int64_t bytesWritten = _serialPort->write(framePackage->_codedRequest.data(), framePackage->_codedRequest.size());
						if (bytesWritten == -1)
						{
							AppCore::writeToApplicationLogLine("TACPIC32CXDriveTrain::run()::bytesWritten == -1");

							QString errorString = _serialPort->errorString();
							if (errorString.isEmpty() == false)
							{
								errorString = QString("Error on write ") + errorString;
								AppCore::writeToApplicationLogLine(errorString.toStdString());
							}

							stopRunning();
						}
					}
				}
				else
				{
					if (weAreRunning() == false)
						loopFinished = true;
					else
						_serialPort->waitForReadyRead(10);
				}

				if (readSerialData() == true)
				{
					_protocolInterface->idle();
				}
			}
		}

		_serialPort->close();

		_connected = false;
	}
	else
	{
		emit errorOnOpen(lastErrorMessage());
	}

	shutdownLogging();
}

void TACPIC32CXDriveThread::handleSetPin(FramePackage& framePackage)
{
	Argument pinArgument{framePackage->getArgument(1)};
	Argument stateArgument{framePackage->getArgument(0)};

	if (std::holds_alternative<uint32_t>(pinArgument) && std::holds_alternative<bool>(stateArgument))
	{
		uint32_t pin = std::get<uint32_t>(pinArgument);
		std::string pinString = "Pin " + std::to_string(pin) + " ";

		bool pinOn = std::get<bool>(stateArgument);
		if (pinOn)
			framePackage->_synonym = pinString + "on";
		else
			framePackage->_synonym = pinString + "off";

		emit pinStateChanged(static_cast<PinID>(pin), pinOn);
	}
}

void TACPIC32CXDriveThread::handleVersionResponse(FramePackage& framePackage)
{
	if (!framePackage->_responses.empty())
	{
		std::string resp = framePackage->_responses.at(0);
		std::vector<std::string> boardIdentity;
		size_t start = 0, pos = 0;
		while ((pos = resp.find(',', start)) != std::string::npos)
		{
			if (pos > start)
				boardIdentity.push_back(resp.substr(start, pos - start));
			start = pos + 1;
		}
		if (start < resp.size())
			boardIdentity.push_back(resp.substr(start));

		if (boardIdentity.size() < 7)
		{
			AppCore::writeToApplicationLogLine("Incomplete board response: " + resp);
		}
		else
		{
			_name = boardIdentity.at(0);
			AppCore::writeToApplicationLogLine("PIC32CX board name: " + _name);

			_firmwareString = boardIdentity.at(1);
			AppCore::writeToApplicationLogLine("PIC32CX board firmware version: " + _firmwareString);

			_macAddress = boardIdentity.at(3);
			AppCore::writeToApplicationLogLine("PIC32CX board MAC: " + _macAddress);

			try {
				int boardID = std::stoi(boardIdentity.at(4));
				_platformID = static_cast<PlatformID>(boardID);
			} catch (...) {
				_platformID = ALPACA_PIC32CX_ID;
			}

			AppCore::writeToApplicationLogLine("Identified platform id: " + std::to_string(_platformID));

			_serialNumber = boardIdentity.at(5);
			AppCore::writeToApplicationLogLine("PIC32CX board Serial Number: " + _serialNumber);

			_mcnNumber = boardIdentity.at(6);
			AppCore::writeToApplicationLogLine("PIC32CX board MCN: " + _mcnNumber);
		}
	}

	emit platformIDUpdate(static_cast<int>(_platformID));

	setupConnected();
}

void TACPIC32CXDriveThread::handleClearBuffer(FramePackage &framePackage)
{
	if (!framePackage->_responses.empty())
	{
		std::string joined;
		for (size_t i = 0; i < framePackage->_responses.size(); ++i)
		{
			if (i > 0) joined += ",";
			joined += framePackage->_responses[i];
		}

		AppCore::writeToApplicationLogLine("Buffer cleared. Board response: '" + joined + "'");
	}
}

void TACPIC32CXDriveThread::setupConnected()
{
	if (_connected == false)
	{
		emit hardwareVersionUpdate(hardwareVersionString());

		_connected = true;

		// okay, let send our initial requests
		AppCore::writeToApplicationLogLine("");

		TACPIC32CXCommand tacCommand(this, this);
		
		AppCore::writeToApplicationLogLine("tacCommand.platformID()");
		tacCommand.platformID();

		emit deviceConnected();
	}
}

void TACPIC32CXDriveThread::setupDiscovery()
{
	emit hardwareVersionUpdate(hardwareVersionString());

	{
		TACPIC32CXCommand tacCommand(this, this);
		tacCommand.platformID();
	}
}

void TACPIC32CXDriveThread::log(FramePackage& framePackage)
{
	std::string logEntry;

	AppCore::writeToApplicationLogLine("");
	AppCore::writeToApplicationLogLine("Frame Package Start");

	AppCore::writeToApplicationLogLine(framePackage->_valid ? "Frame Valid" : "Frame Invalid");

	if (framePackage->_delayInMilliSeconds > 0)
	{
		logEntry = "Delay " + std::to_string(framePackage->_delayInMilliSeconds) + " in msecs";
		timeStampLogMessage(logEntry);
		AppCore::writeToApplicationLogLine(logEntry);
	}
	else if (!framePackage->_comment.empty())
	{
		logEntry = framePackage->_comment;
		timeStampLogMessage(logEntry);
		AppCore::writeToApplicationLogLine(logEntry);
	}
	else
	{
		if (framePackage->_console == true)
			logEntry = "Request from console: " + framePackage->_request;
		else
			logEntry = "Request: " + framePackage->_request;

		AppCore::writeToApplicationLogLine(logEntry);

		logEntry.clear();
		timeStampLogMessage(logEntry);
		AppCore::writeToApplicationLogLine(logEntry);

		if (!framePackage->_synonym.empty())
		{
			logEntry = "Synonym: " + framePackage->_synonym;
			AppCore::writeToApplicationLogLine(logEntry);
		}

		AppCore::writeToApplicationLogLine("Responses");

		for (const auto& response : framePackage->_responses)
		{
			logEntry = "   " + response;
			AppCore::writeToApplicationLogLine(logEntry);
		}
	}

	AppCore::writeToApplicationLogLine("Frame Package End");
}
