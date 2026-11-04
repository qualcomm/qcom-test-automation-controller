// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACSTM32DriveThread.h"

#include "AlpacaDevice.h"
#include "STM32Device.h"
#include "TACCommandHashes.h"
#include "TACSTM32Command.h"

// QCommon
#include "AppCore.h"

// Qt
#include <QDateTime>
#include <QDir>

const QString kTACSTM32DriveTrainName{QStringLiteral("TAC STM32 Drive Train")};

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

	if (alpacaDevice->active())
	{
		_stm32Device = static_cast<STM32Device*>(alpacaDevice.data());

		_portName = alpacaDevice->portName();
		_platformID = alpacaDevice->platformID();
		_hardwareType = alpacaDevice->debugBoardType();
		_name = alpacaDevice->serialNumber();
		_description = "Arduino BugHopper V2 Board";
		_uuid = "Bug Hopper V2 - No UUID";

		setSerialNumber(alpacaDevice->serialNumber());
		setPortName(alpacaDevice->portName());

		connect(this, &TACDriveThread::deviceDisconnected, alpacaDevice.data(), &_AlpacaDevice::onDeviceDisconnect, Qt::DirectConnection);
		connect(this, &TACDriveThread::progress, alpacaDevice.data(), &_AlpacaDevice::progress);
	}
}

TACSTM32DriveThread::~TACSTM32DriveThread()
{
}

bool TACSTM32DriveThread::openSTM32Device()
{
	bool result{false};

	if (_stm32Device != Q_NULLPTR)
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

void TACSTM32DriveThread::setPinState(quint16 pin, bool state)
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
				tacCommand.setPinState(commandEntry->_pinID, commandEntry->_arguement.toBool());
				break;

			case _CommandEntry::eLog:
				tacCommand.addLogComment(commandEntry->_arguement.toString().toLatin1());
				break;

			case _CommandEntry::eDelay:
				tacCommand.addDelay(commandEntry->_arguement.toUInt());
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
	quint32 addr,
	quint32 reg
)
{
	Q_UNUSED(addr)
	Q_UNUSED(reg)
}

void TACSTM32DriveThread::i2CWriteRegister
(
	quint32 addr,
	quint32 reg,
	quint32 data
)
{
	Q_UNUSED(addr)
	Q_UNUSED(reg)
	Q_UNUSED(data)
}

void TACSTM32DriveThread::setName
(
	const QByteArray& newName
)
{
	Q_UNUSED(newName)
}

void TACSTM32DriveThread::sendCommand
(
	const QByteArray& command,
	bool console, // = false
	ReceiveInterface* receiveInterface, // = Q_NULLPTR
	bool shouldStore // = true
)
{
	Q_UNUSED(console)
	Q_UNUSED(receiveInterface)
	Q_UNUSED(shouldStore)

	Arguments args;
	QByteArray decodedCommand = decodeCommand(command, args);

	send(decodedCommand, args, console, receiveInterface, shouldStore);
}

quint32 TACSTM32DriveThread::send
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
				QVariant state{framePackage->getArgument(0)};
				QVariant pin{framePackage->getArgument(1)};

				framePackage->_synonym = "Set Pin " + pin.toByteArray() + " " + state.toByteArray();

				emit pinStateChanged(pin.toULongLong(), state.toBool());
			}
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
			if (framePackage.isNull() == false)
			{
				if (framePackage->_delayInMilliSeconds != 0 ||
					framePackage->_comment.isEmpty() == false ||
					framePackage->_endTransaction == true ||
					checkLocalStore(framePackage) == true)
				{
					receive(framePackage);
				}
				else if (_stm32Device != Q_NULLPTR)
				{
					if (_stm32Device->write(framePackage->_codedRequest) == true)
					{
						receive(framePackage);
					}
					else
					{
						AppCore::writeToApplicationLogLine(QString("TACSTM32DriveThread::run() write failure for request %1").arg(framePackage->_request.data()));

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

	if (_stm32Device != Q_NULLPTR)
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
