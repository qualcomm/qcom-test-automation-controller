// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AlpacaDevice.h"
#include "TACException.h"
#include "FTDIDevice.h"
#include "PSOCDevice.h"
#include "PIC32CXDevice.h"
#include "STM32Device.h"

// QCommon
#include "AppCore.h"
#include "StringUtilities.h"

#include <cctype>
#include <iterator>

const std::string kSendCommandError("sendCommand error: Attempted operation on inactive device");
const std::string kSetPinError("setPinState error: Attempted operation on inactive device");
const std::string kQuickCommandError("quickCommand error: Attempted operation on inactive device");
const std::string kCommandStatusError("commandQueueStatus error: Attempted operation on inactive device");

std::mutex _AlpacaDevice::_mutex;
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

	std::lock_guard<std::mutex> lock(_mutex);

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

uint32_t _AlpacaDevice::updateAlpacaDevices()
{
	_AlpacaDevice::_alpacaDevices.clear();

	std::lock_guard<std::mutex> lock(_mutex);

	PSOCDevice::updateAlpacaDevices();
	FTDIDevice::updateAlpacaDevices();
	PIC32CXDevice::updateAlpacaDevices();
	STM32Device::updateAlpacaDevices();

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

static std::string toLowerCopy(const std::string& value)
{
	std::string result{value};

	for (char& c: result)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	return result;
}

static bool equalsCaseInsensitive(const std::string& left, const std::string& right)
{
	return toLowerCopy(left) == toLowerCopy(right);
}

AlpacaDevice _AlpacaDevice::findAlpacaDevice
(
	const std::string& portName
)
{
	std::string searchTerm{toLowerCopy(portName)};

	for (const auto& alpacaDevice: _AlpacaDevice::_alpacaDevices)
	{
		if (toLowerCopy(alpacaDevice->_portName).find(searchTerm) != std::string::npos)
			return alpacaDevice;

		if (toLowerCopy(alpacaDevice->_serialNumber).find(searchTerm) != std::string::npos)
			return alpacaDevice;
	}

	return AlpacaDevice(Q_NULLPTR);
}

AlpacaDevice _AlpacaDevice::findAlpacaDeviceBySerialNumber
(
	const std::string& serialNumber,
	bool usePartial
)
{
	std::string testSerial{toLowerCopy(serialNumber)};

	for (const auto& alpacaDevice: _AlpacaDevice::_alpacaDevices)
	{
		std::string candidateSerial{toLowerCopy(alpacaDevice->_serialNumber)};

		if (usePartial)
		{
			if (testSerial.find(candidateSerial) != std::string::npos)
				return alpacaDevice;
		}
		else
		{
			if (testSerial == candidateSerial)
				return alpacaDevice;
		}
	}

	return AlpacaDevice(Q_NULLPTR);
}

AlpacaDevice _AlpacaDevice::findAlpacaDeviceByDescription
(
	const std::string &description,
	bool usePartial // = true
)
{
	std::string testDescription{toLowerCopy(description)};

	for (const auto& tacDevice: _AlpacaDevice::_alpacaDevices)
	{
		std::string deviceDescription{toLowerCopy(tacDevice->description())};

		if (usePartial)
		{
			if (testDescription.find(deviceDescription) != std::string::npos)
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
	const std::string &descriptor,
	bool usePartial
)
{
	std::string testDescriptor{toLowerCopy(descriptor)};

	for (const auto& alpacaDevice: _AlpacaDevice::_alpacaDevices)
	{
		std::string deviceDescriptor{toLowerCopy(alpacaDevice->_usbDescriptor)};

		if (usePartial)
		{
			if (testDescriptor.find(deviceDescriptor) != std::string::npos)
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

std::string _AlpacaDevice::getLastError()
{
	std::string temp = _lastError;
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
		std::string alpacaScript = _platformConfiguration->getAlpacaScript();
		ScriptVariables scriptVariables = _platformConfiguration->getVariables();
		if (_alpacaScript.parseScript(alpacaScript, scriptVariables, commandList()) == false)
		{
			// todo, write errors to log
		}
	}
}

uint32_t _AlpacaDevice::commandCount()
{
	commandList();

	return static_cast<uint32_t>(_commandList.size());
}

TACCommand _AlpacaDevice::commandEntry
(
	uint32_t commandIndex
)
{
	TACCommand result;

	if (commandIndex < static_cast<uint32_t>(_commandList.size()))
	{
		result = _commandList.at(commandIndex);
	}

	return result;
}

TACCommands _AlpacaDevice::commandList()
{
	if (_commandList.empty())
	{
		for (const auto& commandEntry: std::as_const(_commands))
			_commandList.push_back(commandEntry.second);

		auto sortLambda = [] (const TACCommand& ce1, const TACCommand& ce2) -> bool
		{
			return ce1._command < ce2._command;
		};

		std::stable_sort(_commandList.begin(), _commandList.end(), sortLambda);
	}

	return _commandList;
}

uint32_t _AlpacaDevice::quickCommandCount()
{
	if (_quickCommandList.empty())
	{
		ButtonList buttons = _platformConfiguration->getButtons();

		if (buttons.size() > 0)
		{
			for (const auto& button: buttons)
			{
				std::string quickCommand;

				quickCommand = button._label;
				quickCommand += ";" + button._command;
				quickCommand += ";" + button._toolTip;
				quickCommand += ";" + std::to_string(button._cellLocation.first);
				quickCommand += "," + std::to_string(button._cellLocation.second);

				_quickCommandList.push_back(quickCommand);
			}
		}
	}

	return static_cast<uint32_t>(_quickCommandList.size());
}

std::string _AlpacaDevice::getQuickCommand(uint32_t index)
{
	std::string result;

	if (index < static_cast<uint32_t>(_quickCommandList.size()))
		result = _quickCommandList[index];

	return result;
}

uint32_t _AlpacaDevice::scriptVariableCount()
{
	return static_cast<uint32_t>(_platformConfiguration->getVariables().size());
}

std::string _AlpacaDevice::getScriptVariable(const uint32_t index)
{
	std::string result;
	ScriptVariables scriptVariables = _platformConfiguration->getVariables();

	if (index < scriptVariables.size())
	{
		auto variableIterator = scriptVariables.begin();
		std::advance(variableIterator, index);
		const ScriptVariable& scriptVariable = variableIterator->second;

		std::string defaultValueString;
		if (std::holds_alternative<bool>(scriptVariable._defaultValue))
			defaultValueString = std::get<bool>(scriptVariable._defaultValue) ? "true" : "false";
		else if (std::holds_alternative<uint32_t>(scriptVariable._defaultValue))
			defaultValueString = std::to_string(std::get<uint32_t>(scriptVariable._defaultValue));
		else if (std::holds_alternative<std::string>(scriptVariable._defaultValue))
			defaultValueString = std::get<std::string>(scriptVariable._defaultValue);

		result = scriptVariable._name;
		result += ";" + scriptVariable._label;
		result += ";" + scriptVariable._tooltip;
		result += ";" + variableTypeToString(scriptVariable._type);
		result += ";" + defaultValueString;
		result += ";" + std::to_string(scriptVariable._cellLocation.first);
		result += "," + std::to_string(scriptVariable._cellLocation.second);
	}
	return result;
}

bool _AlpacaDevice::updateScriptVariableValue(const std::string &scriptVariable, const std::string &value)
{
	bool result{false};
	ScriptVariables scriptVariables = _platformConfiguration->getVariables();
	if (scriptVariables.find(scriptVariable) != scriptVariables.end())
	{
		result = _platformConfiguration->setVariableDefaultValue(scriptVariable, value);
	}
	else
	{
		AppCore::writeToApplicationLogLine("[updateScriptVariableValue()]: The variable with name: '" + scriptVariable + "' was not found.");
		throw TACException(TAC_SCRIPT_VARIABLE_NOT_FOUND, "Script variable " + scriptVariable + " not Found.");
	}
	return result;
}

bool _AlpacaDevice::getCommandState
(
	const std::string& command
)
{
	bool result{false};

	auto commandIterator = _commands.find(command);
	if (commandIterator != _commands.end())
		result = commandIterator->second._currentState;
	else
	{
		AppCore::writeToApplicationLogLine("getCommandState " + command + " is not found in Alpaca Script.");
		throw TACException(TAC_COMMAND_NOT_FOUND, "Command " + command + " not Found.");
	}

	return result;
}

bool _AlpacaDevice::sendCommand
(
	const std::string& command,
	bool state
)
{
	bool result{false};

	if (_driveThread != Q_NULLPTR)
	{
		auto commandIterator = _commands.find(command);
		if (commandIterator != _commands.end())
		{
			setPinState(commandIterator->second._pin, state);
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
			AppCore::writeToApplicationLogLine(command + " is not found in Alpaca Script.");

			throw TACException(TAC_COMMAND_NOT_FOUND, "sendCommand error: Command " + command + " not found.");
		}
		else
		{
			if (active() == false)
			{
				AppCore::writeToApplicationLogLine("_AlpacaDevice::sendCommand(" + command + ", " + fromBool(state) + ") failed. Operation on inactive device");
				throw TACException(TAC_DEVICE_INACTIVE, kSendCommandError);
			}
		}
	}

	return result;
}

bool _AlpacaDevice::quickCommand
(
	const std::string& command
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
				AppCore::writeToApplicationLogLine(command + " is not found in Alpaca Script. Quick Command not executed");
				AppCore::writeToApplicationLogLine("Available Commands");
				std::vector<std::string> availableCommands = _alpacaScript.availableCommands();
				for (const auto& scriptCommand: availableCommands)
					AppCore::writeToApplicationLogLine("   " + scriptCommand);

				// TODO: Throw on else: command not found but check if there are python issues
			}
		}
		else
		{
			AppCore::writeToApplicationLogLine("_AlpacaDevice::quickCommand(" + command + ") failed. Operation on inactive device");
			throw TACException(TAC_DEVICE_INACTIVE, kQuickCommandError);
		}
	}

	return result;
}

bool _AlpacaDevice::isCommandQueueClear()
{
	bool result{false};

	if (_driveThread != Q_NULLPTR)
	{
		if (active() == true)
			result = _driveThread->waitForCompletionStatus();
	}
	else
	{
		AppCore::writeToApplicationLogLine("_AlpacaDevice::isCommandQueueClear() failed. Operation on inactive device");
		throw TACException(TAC_DEVICE_INACTIVE, kCommandStatusError);
	}

	return result;
}

std::string _AlpacaDevice::getHelp()
{
	if (_helpText.empty())
	{
		const std::string kEndOfLine("\n");
		const std::string kTab("\t");

		std::string result;
		result.reserve(4096);

		result = "Name: " + _platformConfiguration->name().toStdString() + kEndOfLine + kEndOfLine;
		result += "Platform: " + debugBoardTypeToString(_platformConfiguration->getPlatform()) + kEndOfLine;
		result += "Author: " + _platformConfiguration->author().toStdString() + kEndOfLine;
		result += "Description: " + _platformConfiguration->description().toStdString() + kEndOfLine;
		result += "Modification Date: " + _platformConfiguration->modificationDate().toStdString() + kEndOfLine;

		Pins pins = _platformConfiguration->getPins();
		if (pins.size() > 0)
		{
			result += kEndOfLine;
			result += "Commands" + kEndOfLine + kEndOfLine;
			for (const auto& pin: pins)
			{
				if (pin._cellLocation != std::pair<int, int>(-1, -1) || equalsCaseInsensitive(pin._pinCommand, "TC_READY_N"))
					result += kTab + pin._pinLabel + " Command: " + pin._pinCommand + "(" + std::to_string(pin._pin) + ")" + ", " + pin._pinTooltip + kEndOfLine;
			}
		}

		ButtonList buttons = _platformConfiguration->getButtons();
		if (buttons.size() > 0)
		{
			result += kEndOfLine;
			result += "Quick Commands" + kEndOfLine + kEndOfLine;
			for (const auto& button: buttons)
			{
				result += kTab + button._label + " Command: " + button._command + ", " + button._toolTip + kEndOfLine;
			}
		}

		_helpText = result;
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
				TACCommand tacCommand = TACCommand::find(pin, _commandList);

				AppCore::writeToApplicationLogLine("_AlpacaDevice::setPinState(" + std::to_string(pin) + ")  Command:" + tacCommand._command);
			}
		}
		else
		{
			AppCore::writeToApplicationLogLine("_AlpacaDevice::setPinState(" + std::to_string(pin) + ", " + fromBool(state) + ") failed. Operation on inactive device");
			throw TACException(TAC_DEVICE_INACTIVE, kSetPinError);
		}
	}
	else
	{
		AppCore::writeToApplicationLogLine("_AlpacaDevice::setPinState _driveThread is NULL");
	}
}

void _AlpacaDevice::setAddressPinState
(
	const std::string& i2cAddress,
	uint16_t pin,
	bool state
)
{
	if (_driveThread != Q_NULLPTR)
	{
		if (active() == true)
		{
			_driveThread->setAddressPinState(i2cAddress, pin, state);
			if (AppCore::getAppCore()->appLoggingActive())
			{
				AppCore::writeToApplicationLogLine("_AlpacaDevice::setAddressPinState(addr=" + i2cAddress + ", pin=" + std::to_string(pin) + ", state=" + fromBool(state) + ")");
			}
		}
		else
		{
			AppCore::writeToApplicationLogLine("_AlpacaDevice::setAddressPinState(" + i2cAddress + ", " + std::to_string(pin) + ", " + fromBool(state) + ") failed. Operation on inactive device");
			throw TACException(TAC_DEVICE_INACTIVE, kSetPinError);
		}
	}
	else
	{
		AppCore::writeToApplicationLogLine("_AlpacaDevice::setAddressPinState _driveThread is NULL");
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

std::string _AlpacaDevice::description()
{
	return _description;
}

std::string _AlpacaDevice::usbDescriptor()
{
	return _usbDescriptor;
}

std::string _AlpacaDevice::serialNumber()
{
	return _serialNumber;
}

PlatformID _AlpacaDevice::platformID()
{
	return _platformID;
}

std::string _AlpacaDevice::macAddress()
{
	if (_driveThread)
		return _driveThread->macAddress();

	return std::string();
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

std::string _AlpacaDevice::chipVersion()
{
	std::string result{"None"};

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
	auto commandIterator = _commands.find("extpower");
	if (commandIterator != _commands.end())
	{
		setPinState(commandIterator->second._pin, state);
	}
	else
	{
		throw TACException(TAC_COMMAND_NOT_FOUND, "Command extpower not Found.");
	}
}

std::string _AlpacaDevice::name() const
{
	if (_driveThread)
		return _driveThread->name().toStdString();

	return _serialNumber;
}

void _AlpacaDevice::setName(const std::string &newName)
{
	if (_driveThread)
		_driveThread->setName(QByteArray::fromStdString(newName));
}

QSize _AlpacaDevice::windowDimension()
{
	if (_platformConfiguration != Q_NULLPTR)
		return _platformConfiguration->getFormDimension();
	return QSize(560, 750);
}

std::string _AlpacaDevice::portName() const
{
//	if (_portName.isEmpty())
//	{
//		if (_driveThread)
//			_portName = _driveThread->portName();
//	}

	return _portName;
}

void _AlpacaDevice::setPortName(const std::string &portName)
{
	_portName = portName;

	if (_driveThread)
		_driveThread->setPortName(QByteArray::fromStdString(portName));
}

std::string _AlpacaDevice::description() const
{
	if (_driveThread)
		return _driveThread->description();

	return std::string();
}

void _AlpacaDevice::setDescription(const std::string &description)
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
	_serialNumber = serialNumber.toLatin1().toStdString();

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
	AppCore::writeToApplicationLogLine("_AlpacaDevice::on_pinStateChanged(" + std::to_string(pin) + ", " + fromBool(state) + ")");

	for (auto& entry: _commands)
	{
		if (entry.second._pin == pin)
		{
			entry.second._currentState = state;
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

void _AlpacaDevice::i2CReadRegister(uint32_t addr, uint32_t reg)
{
	if (_driveThread != Q_NULLPTR)
		_driveThread->i2CReadRegister(addr, reg);
}

void _AlpacaDevice::i2CWriteRegister(uint32_t addr, uint32_t reg, uint32_t data)
{
	if (_driveThread != Q_NULLPTR)
		_driveThread->i2CWriteRegister(addr, reg, data);
}

