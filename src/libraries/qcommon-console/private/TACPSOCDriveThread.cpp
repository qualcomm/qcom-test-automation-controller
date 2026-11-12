// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACPSOCDriveThread.h"

#include "AlpacaDevice.h"
#include "TACCommands.h"
#include "TACCommandHashes.h"
#include "TACPSOCCommand.h"

// QCommon
#include "AppCore.h"
#include "PSOCSerialTableModel.h"
#include "StringUtilities.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#define on true
#define off false

const std::string kTACSerialDriveTrainName("TAC Serial Drive Train");

TACPSOCDriveThread::TACPSOCDriveThread
(
	uint hash
) :
	TACDriveThread(hash)
{
	_driveTrainName = kTACSerialDriveTrainName;
	setProtocolInterface(&_tacProtocol);
	_tacProtocol.setTACDriveTrain(this);

	AlpacaDevice alpacaDevice = _AlpacaDevice::findAlpacaDevice(hash);
	if (alpacaDevice && alpacaDevice->active())
	{
		_portName = alpacaDevice->portName();
		_serialNumber = alpacaDevice->serialNumber();
		_description = alpacaDevice->description();

		connect(this, &TACDriveThread::pinStateChanged, alpacaDevice.get(), &_AlpacaDevice::on_pinStateChanged);
		connect(this, &TACDriveThread::progress, alpacaDevice.get(), &_AlpacaDevice::progress);
	}
}

TACPSOCDriveThread::~TACPSOCDriveThread()
{
}

bool TACPSOCDriveThread::openSerialDevice()
{
	bool result(false);

	PSOCSerialTableModel tableModel;

	_tacPortInfo = tableModel.getSerialPortFromName(_portName);

	if (_tacPortInfo.isNull())
		_tacPortInfo = tableModel.getSerialPortFromSerialNumber(_portName);

	if (_tacPortInfo.isNull() == false)
	{
		SerialPortSettings serialSettings;

		_serialPort = new SerialPort(_tacPortInfo.portName().toStdString());

		serialSettings = _serialPort->getSerialPortSettings();
		serialSettings._baudRate = static_cast<int32_t>(SerialBaudRate::Baud115200);
		_serialPort->setSerialPortSettings(serialSettings);

		if (_serialPort->open(QIODevice::ReadWrite) == true)
		{		
			setSerialNumber(_tacPortInfo.serialNumber().toStdString());
			setPortName(_tacPortInfo.portName().toStdString());

			emit serialNumUpdate(_serialNumber);

			AppCore::writeToApplicationLogLine("Device " + _serialNumber + " opened");
			AppCore::writeToApplicationLogLine("Com port " + portName() + "\n");

			result = true;
		}
		else
		{
			_lastErrorMessage = "Unable to open TAC Port. " + serialPortError();

			AppCore::writeToApplicationLogLine(_lastErrorMessage);

			emit errorOnOpen("PSOC Device Open Failed, Check the Application Log");

			delete _serialPort;
			_serialPort = nullptr;
		}
	}

	return result;
}

void TACPSOCDriveThread::sendCommand
(
	const std::string& command,
	bool console, // = false
	ReceiveInterface* receiveInterface, // = nullptr
	bool shouldStore // = true
)
{
	if (command.rfind(kHelpCommand, 0) != 0)
	{
		Arguments args;
		std::string decodedCommand = decodeCommand(command, args);

		send(decodedCommand, args, console, receiveInterface, shouldStore);
	}
}

void TACPSOCDriveThread::endTransaction
(
	ReceiveInterface* receiveInterface
)
{
	_tacProtocol.endTransaction(receiveInterface);
}

void TACPSOCDriveThread::setPinState
(
	uint16_t pin,
	bool state
)
{
	{
		TACPSOCCommand tacCommand(this, this);

		tacCommand.setPinState(pin, state);
	}

	waitForCompletion();
}

void TACPSOCDriveThread::setAddressPinState
(
	const std::string& i2cAddress,
	uint16_t pin,
	bool state
)
{
	{
		TACPSOCCommand tacCommand(this, this);
		tacCommand.setAddressPinState(i2cAddress, pin, state);
	}

	waitForCompletion();
}

void TACPSOCDriveThread::sendCommandSequence
(
	CommandEntries& commandEntries
)
{
	TACPSOCCommand tacCommand(this, this);

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
			tacCommand.addLogComment(std::get<std::string>(commandEntry->_arguement) + "\n");
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

int TACPSOCDriveThread::getResetCount()
{
	{
		TACPSOCCommand tacCommand(this, this);

		tacCommand.getResetCount();
	}

	waitForCompletion();

	return _resetCount;
}

void TACPSOCDriveThread::clearResetCount()
{
	{
		TACPSOCCommand tacCommand(this, this);

		tacCommand.clearResetCount();
	}

	waitForCompletion();
}

void TACPSOCDriveThread::i2CReadRegister
(
	uint32_t addr,
	uint32_t reg
)
{
	{
		TACPSOCCommand tacCommand(this, this);

		try
		{
			tacCommand.i2CReadRegister(addr, reg);
		}
		catch (...)
		{

		}
	}

	waitForCompletion();
}

void TACPSOCDriveThread::i2CWriteRegister
(
	uint32_t addr,
	uint32_t reg,
	uint32_t data
)
{
	{
		TACPSOCCommand tacCommand(this, this);

		try
		{
			tacCommand.i2CWriteRegister(addr, reg, data);
		}
		catch (...)
		{

		}
	}

	waitForCompletion();
}

std::string TACPSOCDriveThread::serialPortError()
{
	std::string result;

	if (_serialPort != nullptr)
	{
		switch (_serialPort->error())
		{
		case SerialPortError::NoError:
			result = "No error occurred.";
			break;

		case SerialPortError::DeviceNotFoundError:
			result = "Device is non-existant.";
			break;

		case SerialPortError::PermissionError:
			result = "The device is already open in another process or the user does not have sufficient security credentials.";
			break;

		case SerialPortError::OpenError:
			result = "This object already has an open device..";
			break;

		case SerialPortError::NotOpenError:
			result = "This error occurs when an operation is executed that can only be successfully performed if the device is open.";
			break;

		case SerialPortError::WriteError:
			result = "An I/O error occurred while writing the data.";
			break;

		case SerialPortError::ReadError:
			result = "An I/O error occurred while reading the data.";
			break;

		case SerialPortError::ResourceError:
			result = "An I/O error occurred when a resource becomes unavailable, e.g. when the device is unexpectedly removed from the system.";
			break;

		case SerialPortError::UnsupportedOperationError:
			result = "The requested device operation is not supported or prohibited by the running operating system.";
			break;

		case SerialPortError::TimeoutError:
			result = "A timeout error occurred.";
			break;

		case SerialPortError::UnknownError:
		default:
			result = "An unidentified serial port error " + std::to_string(static_cast<int>(_serialPort->error())) + " has occurred.";
			break;
		}
	}

	return result;
}

void TACPSOCDriveThread::handleError
(
	SerialPortError error
)
{
	switch(error)
	{
	case SerialPortError::DeviceNotFoundError:
		_lastErrorMessage = "Device Not Found.";
		break;
	case SerialPortError::PermissionError:
		_lastErrorMessage = "Permission Error.";
		break;
	case SerialPortError::OpenError:
		_lastErrorMessage = "Open Error.";
		break;
	case SerialPortError::WriteError:
		_lastErrorMessage = "Write Error.";
		break;
	case SerialPortError::ReadError:
		_lastErrorMessage = "Read Error.";
		break;
	case SerialPortError::ResourceError:
		_lastErrorMessage = "Resource Error.";
		break;
	case SerialPortError::UnsupportedOperationError:
		_lastErrorMessage = "Unsupported Operation Error.";
		break;
	case SerialPortError::UnknownError:
		_lastErrorMessage = "Unknown Error.";
		break;
	case SerialPortError::TimeoutError:
		_lastErrorMessage = "Timeout Error.";
		return;
	case SerialPortError::NotOpenError:
		_lastErrorMessage = "Not Open Error.";
		break;
	case SerialPortError::NoError:
		break;

	default:
		break;
	}
}

void TACPSOCDriveThread::on_readyRead()
{
	_readyRead = true;
}

void TACPSOCDriveThread::setName
(
	const std::string& newName
)
{
	if (newName.length() <= 32)
	{
		if (newName != _name)
		{
			if (isAlphaNumeric(newName) == true)
			{
				{
					TACPSOCCommand tacCommand(this, this);

					tacCommand.setName(newName);
				}

				waitForCompletion();
			}
		}
	}
}

uint32_t TACPSOCDriveThread::send
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

bool TACPSOCDriveThread::ready()
{
	return _tacProtocol.queueSize() == 0;
}

bool TACPSOCDriveThread::readSerialData()
{
	bool result(false);

	if (_serialPort && (_readyRead || _serialPort->bytesAvailable() > 0))
	{
		_readyRead = false;

		std::string buffer = _serialPort->readAll();

		if (!buffer.empty())
		{
#ifdef DEBUG
			AppCore::writeToApplicationLogLine(buffer);
#endif
			_protocolInterface->handleRecievedData(buffer);
			result = true;
		}
	}

	return result;
}

void TACPSOCDriveThread::receive
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
	else
	{
		if (framePackage->_valid == true)
		{
			switch (framePackage->_requestHash)
			{
			case kVersionCommandHash: handleVersionResponse(framePackage); break;
			case kGetPlatformIDCommandHash: handlePlatformID(framePackage); break;
			case kGetUUIDCommandHash: handleUUIDResponse(framePackage); break;
			case kGetNameCommandHash: handleGetNameResponse(framePackage); break;
			case kSetNameCommandHash: handleSetName(framePackage); break;
			case kClearResetCountCommandHash: emit resetCountCleared(); break;
			case kI2CReadRegisterCommandHash:
			case kI2CReadRegisterValueCommandHash:
				handleI2CRead(framePackage); break;
			case kI2CWriteRegisterCommandHash: handleI2CWrite(framePackage); break;
			case kSetPinCommandHash: handleSetPin(framePackage); break;
			default:
				break;
			}
		}
		else
		{
			clearWaitForCompletion();  // Don't want sync to hang on an error

			switch (framePackage->_requestHash)
			{
			case kGetPlatformIDCommandHash:
			{
				TACPSOCCommand tacCommand(this, this);
				tacCommand.version();
				break;
			}
			case kI2CReadRegisterCommandHash:
			case kI2CReadRegisterValueCommandHash:
				handleI2CRead(framePackage); break;

			case kI2CWriteRegisterCommandHash:
				handleI2CWrite(framePackage);
				break;

			default:
				break;
			}
		}
	}

	log(framePackage);

	_protocolInterface->clearPendingFrame();
}

void TACPSOCDriveThread::run()
{
	Q_ASSERT(_protocolInterface != nullptr);

	AppCore::getAppCore()->setRunLogging(AppCore::getAppCore()->getPreferences()->loggingActive());

	if (openSerialDevice() == true)
	{
		emit deviceOpen();

		startRunning();

		emit deviceStatusChange(tr("Starting"));

		_readyRead = false;
		_connected = false;

		{
			TACPSOCCommand tacCommand(this, this);
			tacCommand.platformID();
			tacCommand.version();
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
						AppCore::writeToApplicationLogLine("TACPSOCDriveThread::run()::Write: " + framePackage->_codedRequest);
#endif

						int64_t bytesWritten = _serialPort->write(framePackage->_codedRequest.data(), framePackage->_codedRequest.size());
						if (bytesWritten == -1)
						{
							AppCore::writeToApplicationLogLine("TACPSOCDriveThread::run()::bytesWritten == -1");

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
		AppCore::writeToApplicationLogLine("TACPSOCDriveThread::run(): Serial port closed");

		_connected = false;
	}
	else
	{
		emit errorOnOpen(lastErrorMessage());
	}

	shutdownLogging();
}

void TACPSOCDriveThread::handleGetNameResponse
(
	FramePackage& framePackage
)
{
	if (framePackage->_responses.size() > 1)
	{
		_name = framePackage->_responses.at(1);

		emit nameUpdate(_name);
	}
}

void TACPSOCDriveThread::handleGetResetCount
(
	FramePackage& framePackage
)
{
	if (framePackage != nullptr)
	{
		if (framePackage->_responses.size() >= 3)
		{
			if (framePackage->_responses.at(0) == "getresetcount")
			{
				if (framePackage->_responses.at(2) == "ok")
				{
					try {
						_resetCount = std::stoul(framePackage->_responses.at(1));
					} catch (...) {}
				}
			}
		}
	}

	emit resetCountUpdate(_resetCount);
}

void TACPSOCDriveThread::handleI2CRead
(
	FramePackage& framePackage
)
{
	if (framePackage->_requestHash == kI2CReadRegisterValueCommandHash)
	{
		if (framePackage->_valid)
		{
			std::string response;

			std::vector<std::string> responses = framePackage->_responses;
			responses.pop_back();

			if (!responses.empty())
				 response = responses.back();
			else if (!framePackage->_responses.empty())
				response = framePackage->_responses.back();

			emit i2CReadResult(response, true);
		}
		else
		{
			emit i2CReadResult(framePackage->_responses.empty() ? "" : framePackage->_responses.back(), false);
		}
	}
}

void TACPSOCDriveThread::handleI2CWrite
(
	FramePackage& framePackage
)
{
	if (framePackage->_valid)
	{
		std::string successResult = framePackage->_request + " " + std::get<std::string>(framePackage->getArgument(0)) + " Successful";
		emit i2CWriteResult(successResult);
	}
	else
	{
		std::string result = "\"" + (framePackage->_responses.empty() ? "" : framePackage->_responses.back()) + "\" Are the parameters correct?";
		emit i2CWriteResult(result);
	}
}

void TACPSOCDriveThread::handleSetPin
(
	FramePackage& framePackage
)
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

void TACPSOCDriveThread::handleSetName
(
	FramePackage& framePackage
)
{
	Argument argument{framePackage->getArgument(0)};

	if (std::holds_alternative<std::string>(argument))
	{
		_name = std::get<std::string>(argument);
		framePackage->_synonym = "Set Name " + _name;
	}

	emit nameUpdate(_name);
}

void TACPSOCDriveThread::handleUUIDResponse
(
	FramePackage& framePackage
)
{
	if (framePackage->_responses.size() > 1)
	{
		_uuid = framePackage->_responses.at(1);

		emit uuidUpdate(_uuid);
	}
}

void TACPSOCDriveThread::handleVersionResponse
(
	FramePackage& framePackage
)
{
	static int retryCount{1};

	if (framePackage->_responses.size() > 1)
	{
		bool isEPM{false};
		std::string versionString = framePackage->_responses.at(0);
		if (versionString.find("EPM") != std::string::npos)
			isEPM = true;
		else
		{
			versionString = framePackage->_responses.at(1);
			if (versionString.find("EPM") != std::string::npos)
				isEPM = true;
		}

		_versionString = versionString;
		AppCore::writeToApplicationLogLine("Version String: " + _versionString);

		retryCount = 1;

		if (isEPM)
		{
			_hardwareType = ePSOC;
		}
		else
		{
			_hardwareType = eSpiderBoard;
		}

		emit hardwareTypeUpdate(debugBoardTypeString());

		std::vector<std::string> versionParts;
		size_t start = 0, pos = 0;
		while ((pos = _versionString.find(' ', start)) != std::string::npos)
		{
			if (pos > start)
				versionParts.push_back(_versionString.substr(start, pos - start));
			start = pos + 1;
		}
		if (start < _versionString.size())
			versionParts.push_back(_versionString.substr(start));

		if (versionParts.size() >= 7)
		{
			auto it = std::find(versionParts.begin(), versionParts.end(), "FW");
			if (it != versionParts.end() && std::distance(versionParts.begin(), it) + 2 < static_cast<ptrdiff_t>(versionParts.size()))
			{
				try
				{
					std::string firmwareString = *(it + 2);

					_firmwareString = firmwareString;

					std::vector<std::string> firmwareVersionParts;
					size_t fStart = 0, fPos = 0;
					while ((fPos = firmwareString.find('.', fStart)) != std::string::npos)
					{
						if (fPos > fStart)
							firmwareVersionParts.push_back(firmwareString.substr(fStart, fPos - fStart));
						fStart = fPos + 1;
					}
					if (fStart < firmwareString.size())
						firmwareVersionParts.push_back(firmwareString.substr(fStart));

					if (firmwareVersionParts.size() >= 4)
					{
						_firmwareMajor = std::stoul(firmwareVersionParts.at(0));
						_firmwareChip = std::stoul(firmwareVersionParts.at(1));
						_firmwareMinor = std::stoul(firmwareVersionParts.at(2));
						_firmwareRevision = std::stoul(firmwareVersionParts.at(3));

						if (_firmwareMinor < 15)
							_oldFirmware = true;
					}

					emit firmwareVersionUpdate(firmwareString);
				}
				catch (...)
				{
					AppCore::writeToApplicationLogLine("Version Parts Failed");
				}
			}
		}

		emit deviceStatusChange(tr("TAC Version Good"));

		setupConnected();
	}
	else if (retryCount < 4)
	{
		AppCore::writeToApplicationLogLine("Version Failed Retry " + std::to_string(retryCount));
		TACPSOCCommand tacCommand(this, this);

		tacCommand.version();

		retryCount++;
	}
}

void TACPSOCDriveThread::handlePlatformID
(
	FramePackage& framePackage
)
{
	if (framePackage->_responses.size() >= 2)
	{
		try {
			int boardID = std::stoi(framePackage->_responses.at(1));
			_platformID = static_cast<PlatformID>(boardID);
		} catch (...) {}
	}

	emit platformIDUpdate(static_cast<int>(_platformID));

	setupConnected();
}

void TACPSOCDriveThread::setupConnected()
{
	if (_connected == false)
	{
		emit hardwareVersionUpdate(hardwareVersionString());

		_connected = true;

		// okay, let send our initial requests
		AppCore::writeToApplicationLogLine("");
		TACPSOCCommand tacCommand(this, this);

		AppCore::writeToApplicationLogLine("tacCommand.uuid()");
		tacCommand.uuid();

		AppCore::writeToApplicationLogLine("tacCommand.name()");
		tacCommand.name();

		emit deviceConnected();
	}
}

void TACPSOCDriveThread::setupDiscovery()
{
	emit hardwareVersionUpdate(hardwareVersionString());

	TACPSOCCommand tacCommand(this, this);
	tacCommand.uuid();
	tacCommand.name();
	tacCommand.platformID();
}

void TACPSOCDriveThread::log
(
	FramePackage& framePackage
)
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
