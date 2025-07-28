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
// Copyright 2022-2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "AlpacaDevice.h"
#include "TacException.h"
#include "FTDIDevice.h"


// QCommon
#include "AppCore.h"
#include "StringUtilities.h"

const QByteArray kSendCommandError("sendCommand error: Attempted operation on inactive device");
const QByteArray kSetPinError("setPinState error: Attempted operation on inactive device");
const QByteArray kQuickCommandError("quickCommand error: Attempted operation on inactive device");


QMutex _AlpacaDevice::_mutex;
AlpacaDevices _AlpacaDevice::_alpacaDevices;

_AlpacaDevice::~_AlpacaDevice()
{
	if (isOpen())
		close();
}

void _AlpacaDevice::getAlpacaDevices
(
	AlpacaDevices& alpacaDevices,
	DebugBoardType debugBoardTypeFilter  // = eUnknownDebugBoard
)
{
	alpacaDevices.clear();

	QMutexLocker lock(&_mutex);

	for (auto& alpacaDevice: _AlpacaDevice::_alpacaDevices)
	{
		if (alpacaDevice->active())
		{
			if (debugBoardTypeFilter == eUnknownDebugBoard)
				alpacaDevices.push_back(alpacaDevice);
			else if (alpacaDevice->debugBoardType() == debugBoardTypeFilter)
				alpacaDevices.push_back(alpacaDevice);
		}
	}
}

quint32 _AlpacaDevice::updateAlpacaDevices()
{
	_AlpacaDevice::_alpacaDevices.clear();

	QMutexLocker lock(&_mutex);

	FTDIDevice::updateAlpacaDevices();

	return _AlpacaDevice::_alpacaDevices.count();
}

AlpacaDevice _AlpacaDevice::findAlpacaDevice
(
	HashType hash
)
{
	for (const auto& alpacaDevice: _AlpacaDevice::_alpacaDevices)
	{
		if (alpacaDevice->_hash == hash)
			return alpacaDevice;
	}

	return AlpacaDevice(Q_NULLPTR);
}

AlpacaDevice _AlpacaDevice::findAlpacaDevice
(
	const QByteArray& portName
)
{
	QByteArray searchTerm{portName.toLower()};

	for (const auto& alpacaDevice: _AlpacaDevice::_alpacaDevices)
	{
		if (alpacaDevice->_portName.toLower().contains(searchTerm))
			return alpacaDevice;

		if (alpacaDevice->_serialNumber.toLower().contains(searchTerm))
			return alpacaDevice;
	}

	return AlpacaDevice(Q_NULLPTR);
}

AlpacaDevice _AlpacaDevice::findAlpacaDeviceBySerialNumber
(
	const QByteArray& serialNumber,
	bool usePartial
)
{
	for (const auto& alpacaDevice: _AlpacaDevice::_alpacaDevices)
	{
		QByteArray testSerial{serialNumber.toLower()};
		QByteArray serialNumber{alpacaDevice->_serialNumber.toLower()};

		if (usePartial)
		{
			if (testSerial.contains(serialNumber))
				return alpacaDevice;
		}
		else
		{
			if (testSerial == serialNumber)
				return alpacaDevice;
		}
	}

	return AlpacaDevice(Q_NULLPTR);
}

AlpacaDevice _AlpacaDevice::findAlpacaDeviceByDescription
(
	const QByteArray &description,
	bool usePartial // = true
)
{
	for (const auto& tacDevice: _AlpacaDevice::_alpacaDevices)
	{
		QByteArray testDescription{description.toLower()};
		QByteArray deviceDescription{tacDevice->description().toLower()};

		if (usePartial)
		{
			if (testDescription.contains(deviceDescription))
				return tacDevice;
		}
		else
		{
			if (testDescription == deviceDescription)
				return tacDevice;
		}
	}

	return AlpacaDevice(Q_NULLPTR);
}

AlpacaDevice _AlpacaDevice::findAlpacaDeviceByUSBDescriptor
(
	const QByteArray &descriptor,
	bool usePartial
)
{
	for (const auto& alpacaDevice: std::as_const(_AlpacaDevice::_alpacaDevices))
	{
		QByteArray testDescriptor{descriptor.toLower()};
		QByteArray deviceDescriptor{alpacaDevice->_usbDescriptor.toLower()};

		if (usePartial)
		{
			if (testDescriptor.contains(deviceDescriptor))
				return alpacaDevice;
		}
		else
		{
			if (testDescriptor == deviceDescriptor)
				return alpacaDevice;
		}
	}

	return AlpacaDevice(Q_NULLPTR);
}

QByteArray _AlpacaDevice::getLastError()
{
	QByteArray temp = _lastError;
	_lastError.clear();
	return temp;
}

bool _AlpacaDevice::isOpen()
{
	return _driveThread != Q_NULLPTR;
}

void _AlpacaDevice::close()
{
	if (_driveThread != Q_NULLPTR)
	{
		_driveThread->shutDown();

		if (_driveThread != Q_NULLPTR)
			_driveThread->deleteLater();

		_driveThread = Q_NULLPTR;
	}
}

void _AlpacaDevice::buildQuickSettings()
{
	if (_alpacaScript.count() == 0)
	{
		QByteArray alpacaScript = _platformConfiguration->getAlpacaScript();
		ScriptVariables scriptVariables = _platformConfiguration->getVariables();
		if (_alpacaScript.parseScript(alpacaScript, scriptVariables, commandList()) == false)
		{
			// todo, write errors to log
		}
	}
}

quint32 _AlpacaDevice::commandCount()
{
	commandList();

	return _commandList.count();
}

TacCommand _AlpacaDevice::commandEntry
(
	quint32 commandIndex
)
{
	TacCommand result;

	if (commandIndex < static_cast<quint32>(_commandList.size()))
	{
		result = _commandList.at(commandIndex);
	}

	return result;
}

TacCommands _AlpacaDevice::commandList()
{
	if (_commandList.isEmpty())
	{
		for (const auto& commandEntry: std::as_const(_commands))
			_commandList.push_back(commandEntry);

		auto sortLambda = [] (const TacCommand& ce1, const TacCommand& ce2) -> bool
		{
			return ce1._command < ce2._command;
		};

		std::stable_sort(_commandList.begin(), _commandList.end(), sortLambda);
	}

	return _commandList;
}

quint32 _AlpacaDevice::quickCommandCount()
{
	if (_quickCommandList.isEmpty())
	{
		ButtonList buttons = _platformConfiguration->getButtons();

		if (buttons.count() > 0)
		{
			for (const auto& button: std::as_const(buttons))
			{
				QByteArray quickCommand;

				quickCommand = button._label.toLatin1();
				quickCommand += ";" + button._command.toLatin1();
				quickCommand += ";" + button._toolTip.toLatin1();
				quickCommand += ";" + QByteArray::number(button._cellLocation.x());
				quickCommand += "," + QByteArray::number(button._cellLocation.y());

				_quickCommandList.append(quickCommand);
			}
		}
	}

	return _quickCommandList.count();
}

QByteArray _AlpacaDevice::getQuickCommand(quint32 index)
{
	QByteArray result;

	if (index < static_cast<quint32>(_quickCommandList.count()))
		result = _quickCommandList[index];

	return result;
}

quint32 _AlpacaDevice::scriptVariableCount()
{
	return _platformConfiguration->getVariables().count();
}

QByteArray _AlpacaDevice::getScriptVariable(const quint32 index)
{
	QByteArray scriptVariableString;
	ScriptVariables scriptVariables = _platformConfiguration->getVariables();
	QList<ScriptVariable> scriptVariableList;

	// The variables list will be populated with the same data every time API is called.
	// Generally the number of variables is expected to be low. Hence, the reason to keep loop instead of global variable
	for (const auto &scriptVar : scriptVariables)
		scriptVariableList.append(scriptVar);

	if (index < scriptVariableList.count())
	{
		scriptVariableString = scriptVariableList[index]._name.toLatin1();
		scriptVariableString += ";" + scriptVariableList[index]._label.toLatin1();
		scriptVariableString += ";" + scriptVariableList[index]._tooltip.toLatin1();
		scriptVariableString += ";" + variableTypeToString(scriptVariableList[index]._type).toLatin1();
		scriptVariableString += ";" + scriptVariableList[index]._defaultValue.toString().toLatin1();
		scriptVariableString += ";" + QString::number(scriptVariableList[index]._cellLocation.x()).toLatin1();
		scriptVariableString += "," + QString::number(scriptVariableList[index]._cellLocation.y()).toLatin1();
	}
	return scriptVariableString;
}

bool _AlpacaDevice::updateScriptVariableValue(const QByteArray &scriptVariable, const QByteArray &value)
{
	bool result{false};
	ScriptVariables scriptVariables = _platformConfiguration->getVariables();
	if (scriptVariables.contains(scriptVariable))
	{
		result = _platformConfiguration->setVariableDefaultValue(scriptVariable, value);
	}
	else
	{
		AppCore::writeToApplicationLogLine("[updateScriptVariableValue()]: The variable with name: '" + scriptVariable + "' was not found.");
		throw TacException(TAC_SCRIPT_VARIABLE_NOT_FOUND, QString("Script variable %1 not Found.").arg(scriptVariable));
	}
	return result;
}

bool _AlpacaDevice::getCommandState
(
	const QByteArray& command
)
{
	bool result{false};

	if (_commands.find(command) != _commands.end())
		result = _commands[command]._currentState;
	else
	{
		AppCore::writeToApplicationLogLine("getCommandState " + command + " is not found in QTAC Script.");
		throw TacException(TAC_COMMAND_NOT_FOUND, QString("Command %1 not Found.").arg(command.data()));
	}

	return result;
}

bool _AlpacaDevice::sendCommand
(
	const QByteArray& command,
	bool state
)
{
	bool result{false};

	if (_driveThread != Q_NULLPTR)
	{
		if (_commands.find(command) != _commands.end())
		{
			setPinState(_commands[command]._pin, state);
			result = true;
		}
		else if (result == false)
		{
			result = quickCommand(command);
		}

		// this should always get called to end the blocking call
		if (_driveThread != Q_NULLPTR)
		{
			_driveThread->clearWaitForCompletion();
		}

		if (result == false)
		{
			AppCore::writeToApplicationLogLine(command + " is not found in QTAC Script.");

			throw TacException(TAC_COMMAND_NOT_FOUND, QString("sendCommand error: Command %1 not found.").arg(command.data()));
		}
		else
		{
			if (active() == false)
			{
				AppCore::writeToApplicationLogLine(QString("_AlpacaDevice::sendCommand(%1, %2) failed. Operation on inactive device").arg(command).arg(state));
				throw TacException(TAC_DEVICE_INACTIVE, kSendCommandError);
			}
		}
	}

	return result;
}

bool _AlpacaDevice::quickCommand
(
	const QByteArray& command
)
{
	bool result{false};

	if (_driveThread != Q_NULLPTR)
	{
		if (active() == true)
		{
			if (_alpacaScript.hasCommand(command))
			{
				ScriptVariables scriptVariables = _platformConfiguration->getVariables();
				CommandEntries commandEntries = _alpacaScript.getCommandEntries(command);
				CommandEntries substitutedCommandEntries = _alpacaScript.replaceTokens(scriptVariables, commandEntries);
				_driveThread->sendCommandSequence(substitutedCommandEntries);

				result = true;
			}
			else
			{
				AppCore::writeToApplicationLogLine(command + " is not found in QTAC Script. Quick Command not executed");
				AppCore::writeToApplicationLogLine("Available Commands");
				QStringList availableCommands = _alpacaScript.availableCommands();
				for (const auto& scriptCommand: availableCommands)
					AppCore::writeToApplicationLogLine("   " + scriptCommand);

				// TODO: Throw on else: command not found but check if there are python issues
			}
		}
		else
		{
			AppCore::writeToApplicationLogLine(QString("_AlpacaDevice::quickCommand(%1) failed. Operation on inactive device").arg(command));
			throw TacException(TAC_DEVICE_INACTIVE, kQuickCommandError);
		}
	}

	return result;
}

QByteArray _AlpacaDevice::getHelp()
{
	QString result;

	if (_helpText.isEmpty())
	{
		const QString kEndOfLine("\n");
		const QString kTab("\t");

		// lets reserve a bit
		result.reserve(4096);

		result = "Name: " + _platformConfiguration->name() + kEndOfLine + kEndOfLine;
		result += "Platform: " + _platformConfiguration->getPlatformString() + kEndOfLine;
		result += "Author: " + _platformConfiguration->author() + kEndOfLine;
		result += "Description: " + _platformConfiguration->description() + kEndOfLine;
		result += "Modification Date: " + _platformConfiguration->modificationDate() + kEndOfLine;


		Pins pins = _platformConfiguration->getPins();
		if (pins.count() > 0)
		{
			result += kEndOfLine;
			result += "Commands" + kEndOfLine + kEndOfLine;
			for (const auto& pin: std::as_const(pins))
			{
				if (pin._cellLocation != QPoint(-1, -1) || pin._pinCommand.compare("TC_READY_N", Qt::CaseInsensitive) == 0)
					result += kTab + pin._pinLabel + QString(" Command: %1(%2)").arg(pin._pinCommand).arg(pin._pin) + ", " + pin._pinTooltip + kEndOfLine;
			}
		}

		ButtonList buttons = _platformConfiguration->getButtons();
		if (buttons.count() > 0)
		{
			result += kEndOfLine;
			result += "Quick Commands" + kEndOfLine + kEndOfLine;
			for (const auto& button: std::as_const(buttons))
			{
				result += kTab + button._label + " Command: " + button._command + ", " + button._toolTip + kEndOfLine;
			}
		}

		result.squeeze();

		_helpText = result.toLatin1();
	}

	return _helpText;
}

void _AlpacaDevice::setPinState
(
	PinID pin,
	bool state
)
{
	if (_driveThread != Q_NULLPTR)
	{
		if (active() == true)
		{
			_driveThread->setPinState(pin, state);
			if (AppCore::getAppCore()->appLoggingActive())
			{
				TacCommand tacCommand = TacCommand::find(pin, _commandList);

				AppCore::writeToApplicationLogLine("_AlpacaDevice::setPinState(" + QString::number(pin) +")  Command:" + tacCommand._command);
			}
		}
		else
		{
			AppCore::writeToApplicationLogLine(QString("_AlpacaDevice::setPinState(%1, %2) failed. Operation on inactive device").arg(QString::number(pin)).arg(state));
			throw TacException(TAC_DEVICE_INACTIVE, kSetPinError);
		}
	}
	else
	{
		AppCore::writeToApplicationLogLine("_AlpacaDevice::setPinState _driveThread is NULL");
	}
}

void _AlpacaDevice::setWaitForCompletion()
{
	if (_driveThread != Q_NULLPTR)
		_driveThread->setWaitForCompletion();
}

bool _AlpacaDevice::active()
{
	return _active;
}

HashType _AlpacaDevice::hash()
{
	return _hash;
}

QByteArray _AlpacaDevice::description()
{
	return _description;
}

QByteArray _AlpacaDevice::usbDescriptor()
{
	return _usbDescriptor;
}

QByteArray _AlpacaDevice::serialNumber()
{
	return _serialNumber;
}

PlatformID _AlpacaDevice::platformID()
{
	return _platformID;
}

QByteArray _AlpacaDevice::macAddress()
{
	if (_driveThread)
		return _driveThread->macAddress();

	return QByteArray();
}

PlatformConfiguration _AlpacaDevice::platformConfiguration()
{
	return _platformConfiguration;
}

DebugBoardType _AlpacaDevice::debugBoardType()
{
	if (_boardType == eUnknownDebugBoard)
	{
		if (_driveThread)
			_boardType = _driveThread->debugBoardType();
	}

	return _boardType;
}

QString _AlpacaDevice::debugBoardTypeString()
{
	if (_driveThread)
		return _driveThread->debugBoardTypeString();

	return QString();
}

QString _AlpacaDevice::hardwareVersionString()
{
	if (_driveThread)
		return _driveThread->hardwareVersionString();

	return QString();
}

QString _AlpacaDevice::firmwareVersion()
{
	if (_driveThread)
		return _driveThread->firmwareVersion();

	return QString();
}

uint _AlpacaDevice::majorVersion()
{
	if (_driveThread)
		return _driveThread->majorVersion();

	return 0;
}

QString _AlpacaDevice::chipVersion()
{
	QString result{"None"};

	switch (_chipVersion)
	{
	case 3:
		result = "LP038";
		break;

	case 4:
		result = "LP030";
		break;

	case 10000:
		result = "FTDI";
	}

	return result;
}

uint _AlpacaDevice::minorVersion()
{
	if (_driveThread)
		return _driveThread->minorVersion();

	return 0;
}

uint _AlpacaDevice::revisionVersion()
{
	if (_driveThread)
		return _driveThread->revisionVersion();

	return 0;
}

void _AlpacaDevice::externalPowerControl(bool state)
{
	if (_commands.find("extpower") != _commands.end())
	{
		setPinState(_commands["extpower"]._pin, state);
	}
	else
	{
		throw TacException(TAC_COMMAND_NOT_FOUND, "Command extpower not Found.");
	}
}

QByteArray _AlpacaDevice::name() const
{
	if (_driveThread)
		return _driveThread->name();

	return _serialNumber;
}

void _AlpacaDevice::setName(const QByteArray &newName)
{
	if (_driveThread)
		_driveThread->setName(newName);
}

QSize _AlpacaDevice::windowDimension()
{
	if (_platformConfiguration != Q_NULLPTR)
		return _platformConfiguration->getFormDimension();
	return QSize(560, 750);
}

QByteArray _AlpacaDevice::portName() const
{
//	if (_portName.isEmpty())
//	{
//		if (_driveThread)
//			_portName = _driveThread->portName();
//	}

	return _portName;
}

void _AlpacaDevice::setPortName(const QByteArray &portName)
{
	_portName = portName;

	if (_driveThread)
		_driveThread->setPortName(portName);
}

QString _AlpacaDevice::description() const
{
	if (_driveThread)
		return _driveThread->description();

	return QString();
}

void _AlpacaDevice::setDescription(const QString &description)
{
	if (_driveThread)
		_driveThread->setDescription(description);
}

QString _AlpacaDevice::serialNumber() const
{
	if (_driveThread)
		return _driveThread->serialNumber();

	return QString();
}

void _AlpacaDevice::setSerialNumber(QString serialNumber)
{
	_serialNumber = serialNumber.toLatin1();

	if (_driveThread)
		_driveThread->setSerialNumber(serialNumber);
}

QString _AlpacaDevice::uuid()
{
	if (_driveThread)
		return _driveThread->uuid();

	return QString();
}

void _AlpacaDevice::on_pinStateChanged
(
		quint64 pin,
		bool state
)
{
	for (auto& command: _commands)
	{
		if (command._pin == pin)
		{
			command._currentState = state;
			break;
		}
	}

	emit pinStateChanged(pin, state);
}

void _AlpacaDevice::onDeviceDisconnect()
{
	close();
}

int _AlpacaDevice::getResetCount()
{
	int result{0};

	if (_driveThread != Q_NULLPTR)
		result = _driveThread->getResetCount();

	return result;
}

void _AlpacaDevice::clearResetCount()
{
	if (_driveThread != Q_NULLPTR)
		_driveThread->clearResetCount();
}

void _AlpacaDevice::i2CReadRegister(quint32 addr, quint32 reg)
{
	if (_driveThread != Q_NULLPTR)
		_driveThread->i2CReadRegister(addr, reg);
}

void _AlpacaDevice::i2CWriteRegister(quint32 addr, quint32 reg, quint32 data)
{
	if (_driveThread != Q_NULLPTR)
		_driveThread->i2CWriteRegister(addr, reg, data);
}

