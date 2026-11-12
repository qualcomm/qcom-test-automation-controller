// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "PlatformConfiguration.h"

#include "PIC32CXPlatformConfiguration.h"
#include "ConsoleApplicationEnhancements.h"
#include "FTDIPlatformConfiguration.h"
#include "FT232HPlatformConfiguration.h"
#include "PSOCPlatformConfiguration.h"
#include "STM32PlatformConfiguration.h"

// QCommon
#include "AlpacaScript.h"
#include "AppCore.h"
#include "PlatformID.h"
#include "Range.h"
#include "StringUtilities.h"

// Qt
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <cctype>
#include <filesystem>

static bool equalsCaseInsensitive(const std::string& left, const std::string& right)
{
	if (left.size() != right.size())
		return false;

	for (size_t index = 0; index < left.size(); index++)
	{
		if (std::tolower(static_cast<unsigned char>(left[index])) != std::tolower(static_cast<unsigned char>(right[index])))
			return false;
	}

	return true;
}

// Parameters in the JSON TAC Configuration
const QString kName(QStringLiteral("name"));
const QString kAuthor(QStringLiteral("author"));
const QString kDescription(QStringLiteral("description"));
const QString kPlatformType(QStringLiteral("platform_type"));
const QString kFileVersion(QStringLiteral("fileVersion"));
const QString kPlatformId(QStringLiteral("platform_id"));
const QString kCreationDate(QStringLiteral("creation_date"));
const QString kModifyDate(QStringLiteral("modification_date"));
const QString kUSBDescriptor(QStringLiteral("usb_descriptor"));
const QString kI2cEnabled(QStringLiteral("i2c_enabled"));
const QString kResetEnabled(QStringLiteral("reset_enabled"));
const QString kFormDimension(QStringLiteral("form_dimension"));

// Tabs dictionary parameters in JSON
const QString kTabs(QStringLiteral("tabs"));
// name is already defined
const QString kUserTab(QStringLiteral("user_tab"));
const QString kMoveable(QStringLiteral("moveable"));
const QString kVisible(QStringLiteral("visible"));
const QString kConfigurable(QStringLiteral("configurable"));
const QString kOrdinal(QStringLiteral("ordinal"));

// Buttons dictionary parameters in JSON
const QString kButtons(QStringLiteral("buttons"));
// name already defined
const QString kCommand(QStringLiteral("command"));
const QString kTab(QStringLiteral("tab"));
const QString kCommandGroup(QStringLiteral("command_group"));
const QString kCellLocation(QStringLiteral("cellLocation"));
const QString kTooltip(QStringLiteral("tooltip"));

// ScriptVariables dictionary parameters in JSON
const QString kScriptVariables(QStringLiteral("variables"));
// name is already defined
const QString kDefaultScriptVarLabel(QStringLiteral("label"));
// tooltip is already defined
const QString kDefaultScriptVarType(QStringLiteral("type"));
const QString kDefaultScriptDefaultValue(QStringLiteral("default_value"));
// cell location is already defined

const QString kScript(QStringLiteral("script"));

const std::string kDefaultLabel("<type a label name>");
const std::string kDefaultTooltip("<add a tooltip>");
const std::string kDefaultCommand("<type a command>");
const QString kDefaultGroup(QStringLiteral("<Select Pin Group>"));
const std::string kDefaultTab("General");
const std::pair<int, int> kDefaultCellLocation(-1,-1);

const std::string kNoVariableValue{"<no value>"};

std::string _PlatformConfiguration::_lastError;
Buttons _PlatformConfiguration::_classicButtons;
bool _PlatformConfiguration::_dynamicConfigurationsInitialized{false};
TACPlatformEntries _PlatformConfiguration::_tacPlatformEntries;
USBDescriptors _PlatformConfiguration::_usbDescriptors;

_PlatformConfiguration::_PlatformConfiguration()
{
	initialize();

	_buttons = _PlatformConfiguration::_classicButtons;

	defaultAlpacaScript();
	defaultScriptVariables();

	_creationDate = currentDateTimeString();
	_modifyDate = currentDateTimeString();
}

_PlatformConfiguration::~_PlatformConfiguration()
{
}

PlatformConfiguration _PlatformConfiguration::createPlatformConfiguration(DebugBoardType debugBoardType, int chipCount, PSOCVariant psocVariant)
{
	PlatformConfiguration result;

	switch (debugBoardType)
	{
	case ePSOC:
		result = PlatformConfiguration(new _PSOCPlatformConfiguration(psocVariant));
		break;

	case eFTDI:
		result = PlatformConfiguration(new _FTDIPlatformConfiguration(chipCount));
		break;

	case ePIC32CXAuto:
		result = PlatformConfiguration(new _PIC32CXPlatformConfiguration);
		break;

	case eFT232H:
		result = PlatformConfiguration(new _FT232HPlatformConfiguration);
		break;

	case eSTM32:
		result = PlatformConfiguration(new _STM32PlatformConfiguration);
		break;

	default: ;
	}

	return result;
}

PlatformConfiguration _PlatformConfiguration::openPlatformConfiguration
(
	const std::string& filePath
)
{
	PlatformConfiguration result;

	QFileInfo fileInfo(QString::fromStdString(filePath));

	if (fileInfo.isFile())
	{
		QString fileName = fileInfo.fileName();
		if (fileName.contains("_psoc_", Qt::CaseInsensitive))
		{
			result = PlatformConfiguration(new _PSOCPlatformConfiguration(ePSOCUnknown));
		}
		else if (fileName.contains("_ftdi_", Qt::CaseInsensitive))
		{
			result = PlatformConfiguration(new _FTDIPlatformConfiguration(0));
		}
		else if (fileName.contains("_pic32cxauto_", Qt::CaseInsensitive))
		{
			result = PlatformConfiguration(new _PIC32CXPlatformConfiguration);
		}
		else if (fileName.contains("_ft232h_", Qt::CaseInsensitive))
		{
			result = PlatformConfiguration(new _FT232HPlatformConfiguration);
		}
		else if (fileName.contains("_stm32_", Qt::CaseInsensitive))
		{
			result = PlatformConfiguration(new _STM32PlatformConfiguration);
		}
		else
		{
			_PlatformConfiguration::_lastError = "Unknown Platform";
		}

		if (result)
		{
			result->load(filePath);
		}
	}

	return result;
}

std::vector<PlatformID> _PlatformConfiguration::platformEntryIds()
{
	std::vector<PlatformID> result;

	for (const auto& [id, entry] : _tacPlatformEntries)
		result.push_back(id);

	return result;
}

TACPlatformEntry _PlatformConfiguration::getEntry
(
	PlatformID platformID
)
{
	initializeDynamicPlatform();

	TACPlatformEntry result;

	if (_tacPlatformEntries.find(platformID) != _tacPlatformEntries.end())
	{
		result = _tacPlatformEntries[platformID];
	}

	return result;
}

void _PlatformConfiguration::initializeDynamicPlatform()
{
	if (_dynamicConfigurationsInitialized == false)
	{
		PlatformContainer::initialize();
		PlatformIDList platformIDList = PlatformContainer::getEntries();
		for (const auto& platformID: platformIDList)
		{
			TACPlatformEntry tacPlatformEntry;

			tacPlatformEntry._platformEntry = platformID;

			_tacPlatformEntries[platformID->_platformID] = tacPlatformEntry;
		}

		_dynamicConfigurationsInitialized = true;
	}
}

std::string _PlatformConfiguration::makeConfigName(const std::string &chip, PlatformID platformID)
{
	return "TAC_" + chip + "_" + std::to_string(platformID) + ".tcnf";
}

bool _PlatformConfiguration::parseConfigName
(
	const std::string& fileName,
	DebugBoardType& debugBoardType,
	PlatformID& platformID
)
{
	bool result{false};

	QFileInfo fileInfo(QString::fromStdString(fileName));

	if (fileInfo.suffix().toLower() == "tcnf")
	{
		std::string baseName = fileInfo.baseName().toLower().toStdString();
		std::vector<std::string> fileNameParts;
		std::string token;
		std::stringstream ss(baseName);
		while (std::getline(ss, token, '_'))
		{
			if (!token.empty())
				fileNameParts.push_back(token);
		}

		if (fileNameParts.size() >= 3)
		{
			if (fileNameParts.at(0) == "tac")
			{
				debugBoardType = debugBoardTypeFromString(fileNameParts.at(1));
				platformID = std::stol(fileNameParts.at(2));

				result = true;
			}
		}
	}

	return result;
}

PlatformID _PlatformConfiguration::getUSBDescriptor(const std::string& usbDescriptorString)
{
	PlatformID result{0};

	std::string test{usbDescriptorString};
	for (char& c : test) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	for (const auto& [id, tacEntry]: _tacPlatformEntries)
	{
		std::string candidate{tacEntry._platformEntry->_usbDescriptor};
		for (char& c : candidate) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		if (candidate == test)
		{
			result = tacEntry._platformEntry->_platformID;
			break;
		}
	}

	if (result == 0)
	{
		if (usbDescriptorString.rfind("ALPACA-LITE ", 0) == 0)
			result = ALPACA_LITE_ID;
	}

	return result;
}

bool _PlatformConfiguration::containsUSBDescriptor
(
	const std::string &usbDescriptorString
)
{
	bool result{false};

	std::string test{usbDescriptorString};
	for (char& c : test) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	initializeDynamicPlatform();

	for (const auto& [id, tacEntry]: _tacPlatformEntries)
	{
		std::string candidate{tacEntry._platformEntry->_usbDescriptor};
		for (char& c : candidate) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		if (candidate == test)
		{
			result = true;
			break;
		}
	}

	return result;
}

std::string _PlatformConfiguration::lastError()
{
	std::string result{_PlatformConfiguration::_lastError};

	_PlatformConfiguration::_lastError.clear();

	return result;
}

void _PlatformConfiguration::setName
(
	const std::string& name
)
{
	if (_name != name)
	{
		_name = name;
		_dirty = true;
	}
}

void _PlatformConfiguration::setAuthor
(
	const std::string& author
)
{
	if (_author != author)
	{
		_author = author;
		_dirty = true;
	}
}

void _PlatformConfiguration::setDescription
(
	const std::string& description
)
{
	if (_description != description)
	{
		_description = description;
		_dirty = true;
	}
}

void _PlatformConfiguration::setPlatform(DebugBoardType platform)
{
	if (_platform != platform)
	{
		_platform = platform;
		_dirty = true;
	}
}

void _PlatformConfiguration::deleteTabs(Tabs &tabs)
{
	for (const auto& tab: tabs)
	{
		cascadeTabDelete(tab._name);
	}
}

void _PlatformConfiguration::updateTabs
(
	Tabs &tabs
)
{
	_tabs.clear();

	// rename
	for (const auto& tab: tabs)
	{
		if (tab._userTab == true)
		{
			if (tab._newText.empty() == false && tab._newText != tab._name)
				cascadeTabRename(tab._name, tab._newText);
		}
	}

	// now add the tabs to the list
	for (const auto& tab: tabs)
	{
		if (tab._deleted == false)
		{
			_tabs.push_back(tab);
		}
	}

	_dirty = true;
}

void _PlatformConfiguration::setPlatform(const std::string& platform)
{
	if (platform == "psoc" || platform == "PSOC")
		_platform = ePSOC;
	else if (platform == "ftdi" || platform == "FTDI")
		_platform = eFTDI;
	else if (platform == "pic32cx (automotive)" || platform == "PIC32CX (AUTOMOTIVE)")
		_platform = ePIC32CXAuto;
}

PSOCVariant _PlatformConfiguration::variant()
{
	return ePSOCUnknown;
}

PlatformID _PlatformConfiguration::getPlatformId()
{
	return _platformId;
}

std::string _PlatformConfiguration::getFileVersion()
{
	return std::to_string(_fileVersion);
}

uint32_t _PlatformConfiguration::fileVersion()
{
	return _fileVersion;
}

std::string _PlatformConfiguration::getPlatformString()
{
	return debugBoardTypeToString(_platform);
}

void _PlatformConfiguration::setPlatformID(PlatformID platformId)
{
	_platformId = platformId;
	_platformFile = makeConfigName(getPlatformString(), _platformId);
}

Tabs _PlatformConfiguration::getTabs()
{
	return _tabs;
}

std::vector<std::string> _PlatformConfiguration::getAllTabs()
{
	std::vector<std::string> result;

	for (auto& tab: _tabs)
	{
		result.push_back(tab._name);
	}

	return result;
}

std::vector<std::string> _PlatformConfiguration::getVisibleTabs()
{
	std::vector<std::string> result;

	for (auto& tab: _tabs)
	{
		if (tab._visible)
			result.push_back(tab._name);
	}

	return result;
}

void _PlatformConfiguration::renameTab
(
	const std::string& oldName,
	const std::string& newName
)
{
	for (auto& tab: _tabs)
	{
		if (tab._name == oldName)
		{
			tab._name = newName;
			break;
		}
	}

	cascadeTabRename(oldName, newName);
}

void _PlatformConfiguration::setTabVisible
(
	const std::string &tabName,
	bool visible
)
{
	for (auto& tab: _tabs)
	{
		if (tab._name == tabName)
		{
			tab._visible = visible;
			break;
		}
	}
}

Pins _PlatformConfiguration::getPins()
{
	Pins result;

	return result;
}

ButtonList _PlatformConfiguration::getButtons()
{
	ButtonList result;

	for (const auto& entry: _buttons)
		result.push_back(entry.second);

	auto sortLambda = [] (Button& p1, Button& p2) -> bool
	{
		if (p1._tab != p2._tab)
			return p1._tab < p2._tab;

		return p1._commandGroup < p2._commandGroup;
	};

	std::sort(result.begin(), result.end(), sortLambda);

	return result;
}

Buttons _PlatformConfiguration::getButtonsMap()
{
	Buttons result;

	result = _buttons;

	return result;
}

ScriptVariables _PlatformConfiguration::getVariables()
{
	ScriptVariables result;

	result = _scriptVariables;

	return result;
}

bool _PlatformConfiguration::setVariableName(const std::string &variableName)
{
	bool result{false};
	if (equalsCaseInsensitive(variableName, kDefaultVariableName) == false)
	{
		if (_scriptVariables.find(variableName) == _scriptVariables.end())
		{
			ScriptVariable scriptVariable;
			scriptVariable._name = variableName;
			_scriptVariables.insert({variableName, scriptVariable});
		}

		result = true;
	}
	return result;
}

void _PlatformConfiguration::setVariableLabel(const std::string &variableName, const std::string &variableLabel)
{
	if (setVariableName(variableName))
		_scriptVariables[variableName]._label = variableLabel;
}

void _PlatformConfiguration::setVariableTooltip(const std::string &variableName, const std::string &variableTooltip)
{
	if (setVariableName(variableName))
		_scriptVariables[variableName]._tooltip = variableTooltip;
}

void _PlatformConfiguration::setVariableType(const std::string &variableName, const VariableType variableType)
{
	if (setVariableName(variableName))
		_scriptVariables[variableName]._type = variableType;
}

bool _PlatformConfiguration::setVariableDefaultValue(const std::string &variableName, const std::string &variableDefaultValue)
{
	bool result{true};

	if (equalsCaseInsensitive(variableDefaultValue, kNoVariableValue) == false)
	{
		if (setVariableName(variableName))
			_scriptVariables[variableName]._defaultValue = variableDefaultValue;
		else
			result = false;
	}

	return result;
}

void _PlatformConfiguration::setVariableCellLocation(const std::string &variableName, const std::pair<int, int> &variableCellLocation)
{
	if (setVariableName(variableName))
		_scriptVariables[variableName]._cellLocation = variableCellLocation;
}

void _PlatformConfiguration::deleteVariable(const std::string &variableName)
{
	if (_scriptVariables.find(variableName) != _scriptVariables.end())
	{
		_scriptVariables.erase(variableName);
	}
}

void _PlatformConfiguration::deleteButton(HashType hash)
{
	if (_buttons.find(hash) != _buttons.end())
	{
		_buttons.erase(hash);
	}
}

HashType _PlatformConfiguration::addButtonLabel(const std::string &buttonLabel, const std::string& tabName)
{
	HashType hash{0};

	if (buttonLabel != kDefaultLabel || tabName != kDefaultTab)
	{
		Button button;
		bool hashExists{false};

		button._label = buttonLabel;
		button._tab = tabName;
		hash = Button::makeHash(button);

		// After calculating the proper hash, re-check if a partial entry exists in memory.
		// If yes, update the existing button object instead inserting a new one in its place.
		for (auto& entry : _buttons)
		{
			if (entry.second._hash == hash)
			{
				button = entry.second;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		_buttons[button._hash] = button;
	}

	return hash;
}

bool _PlatformConfiguration::setButtonLabel(HashType hash, const std::string &buttonLabel)
{
	bool result{false};
	for (auto& entry: _buttons)
	{
		if (entry.second._hash == hash)
		{
			entry.second._label = buttonLabel;
			entry.second._hash = Button::makeHash(entry.second);
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonTooltip(const std::string &labelName, const std::string &tabName, const std::string &toolTip)
{
	HashType hash{0};

	if (labelName != kDefaultLabel || tabName != kDefaultTab || toolTip != kDefaultTooltip)
	{
		Button button;
		bool hashExists{false};

		button._label = labelName;
		button._tab = tabName;
		hash = Button::makeHash(button);

		// After calculating the proper hash, re-check if a partial entry exists in memory.
		// If yes, update the existing button object instead inserting a new one in its place.
		for (auto& entry : _buttons)
		{
			if (entry.second._hash == hash)
			{
				button = entry.second;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		button._toolTip = toolTip;
		_buttons[button._hash] = button;
	}

	return hash;
}

bool _PlatformConfiguration::setButtonTooltip(HashType hash, const std::string &toolTip)
{
	bool result{false};
	for (auto& entry: _buttons)
	{
		if (entry.second._hash == hash)
		{
			entry.second._toolTip = toolTip;
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonCommand(const std::string& labelName, const std::string& tabName, const std::string &command)
{
	HashType hash{0};

	if (labelName != kDefaultLabel || tabName != kDefaultTab || command != kDefaultCommand)
	{
		Button button;
		bool hashExists{false};

		button._label = labelName;
		button._tab = tabName;
		hash = Button::makeHash(button);

		// After calculating the proper hash, re-check if a partial entry exists in memory.
		// If yes, update the existing button object instead inserting a new one in its place.
		for (auto& entry : _buttons)
		{
			if (entry.second._hash == hash)
			{
				button = entry.second;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		button._command = command;
		_buttons[button._hash] = button;
	}

	return hash;
}

bool _PlatformConfiguration::setButtonCommand(HashType hash, const std::string &command)
{
	bool result{false};
	for (auto& entry: _buttons)
	{
		if (entry.second._hash == hash)
		{
			entry.second._command = command;
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonCommandGroup(const std::string& labelName, const std::string& tabName, const CommandGroups &commandGroup)
{
	HashType hash{0};

	if (labelName != kDefaultLabel || tabName != kDefaultTab || commandGroup != eUnknownCommandGroup)
	{
		Button button;
		bool hashExists{false};

		button._label = labelName;
		button._tab = tabName;
		hash = Button::makeHash(button);

		// After calculating the proper hash, re-check if a partial entry exists in memory.
		// If yes, update the existing button object instead inserting a new one in its place.
		for (auto& entry : _buttons)
		{
			if (entry.second._hash == hash)
			{
				button = entry.second;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		button._commandGroup = commandGroup;
		_buttons[button._hash] = button;
	}

	return hash;
}

bool _PlatformConfiguration::setButtonCommandGroup(HashType hash, const CommandGroups& commandGroup)
{
	bool result{false};
	for (auto& entry: _buttons)
	{
		if (entry.second._hash == hash)
		{
			entry.second._commandGroup = commandGroup;
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonTab(const std::string &labelName, const std::string &tabName)
{
	HashType hash{0};

	if (labelName != kDefaultLabel || tabName != kDefaultTab)
	{
		Button button;
		bool hashExists{false};

		button._label = labelName;
		button._tab = tabName;
		hash = Button::makeHash(button);

		// After calculating the proper hash, re-check if a partial entry exists in memory.
		// If yes, update the existing button object instead inserting a new one in its place.
		for (auto& entry : _buttons)
		{
			if (entry.second._hash == hash)
			{
				button = entry.second;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		_buttons[button._hash] = button;
	}

	return hash;
}

bool _PlatformConfiguration::setButtonTab(HashType hash, const std::string &tabName)
{
	bool result{false};
	for (auto& entry: _buttons)
	{
		if (entry.second._hash == hash)
		{
			entry.second._tab = tabName;
			entry.second._hash = Button::makeHash(entry.second);
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonCellLocation(const std::string& labelName, const std::string& tabName, const std::pair<int, int> &cellLocation)
{
	HashType hash{0};

	if (labelName != kDefaultLabel || tabName != kDefaultTab || cellLocation != kDefaultCellLocation)
	{
		Button button;
		bool hashExists{false};

		button._label = labelName;
		button._tab = tabName;
		hash = Button::makeHash(button);

		// After calculating the proper hash, re-check if a partial entry exists in memory.
		// If yes, update the existing button object instead inserting a new one in its place.
		for (auto& entry : _buttons)
		{
			if (entry.second._hash == hash)
			{
				button = entry.second;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		button._cellLocation = cellLocation;
		_buttons[button._hash] = button;
	}

	return hash;
}

bool _PlatformConfiguration::setButtonCellLocation(HashType hash, const std::pair<int, int> &cellLocation)
{
	bool result{false};
	for (auto& entry: _buttons)
	{
		if (entry.second._hash == hash)
		{
			entry.second._cellLocation = cellLocation;
			result = true;

			break;
		}
	}
	return result;
}

std::pair<int, int> _PlatformConfiguration::getFormDimension()
{
	if (_formDimension.first > 0 && _formDimension.second > 0)
		return _formDimension;

	return kClassicDimension;
}

void _PlatformConfiguration::setFormDimension
(
	const std::pair<int, int>& formDimension
)
{
	if (formDimension.first > 0 && formDimension.second > 0)
	{
		if (formDimension != kClassicDimension)
			_formDimension = formDimension;
	}
}

bool _PlatformConfiguration::getResetEnabledState()
{
	return _resetEnabled;
}

void _PlatformConfiguration::setResetEnabledState(bool newState)
{
	if (newState != _resetEnabled)
	{
		_dirty = true;
		_resetEnabled = newState;
	}
}

std::string _PlatformConfiguration::getUSBDescriptor()
{
	return _usbDescriptor;
}

const std::string& _PlatformConfiguration::getAlpacaScript()
{
	return _alpacaScript;
}

void _PlatformConfiguration::setAlpacaScript
(
	const std::string& alpacaScript
)
{
	_alpacaScript = alpacaScript;

	_dirty = true;
}

std::string _PlatformConfiguration::filePath()
{
	return (std::filesystem::path(_platformPath) / _platformFile).lexically_normal().string();
}

void _PlatformConfiguration::setSupportedFirmwareVer(const std::vector<uint32_t> &firmwareList)
{
	_supportedFirmwareVer = firmwareList;
}

std::vector<uint32_t> _PlatformConfiguration::supportedFirmwareVer()
{
	return _supportedFirmwareVer;
}

void _PlatformConfiguration::setFilePath(const std::string &filePath)
{
	std::filesystem::path pathObj(filePath);
	std::string absPath = std::filesystem::absolute(pathObj).lexically_normal().string();

	std::string lowerPath = absPath;
	for (char &c : lowerPath)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	if (lowerPath.rfind("c:/programdata/qualcomm/qtac", 0) == 0 ||
		lowerPath.rfind("c:\\programdata\\qualcomm\\qtac", 0) == 0)
	{
		if (_platform == ePSOC && _platformId < 255)
		{
			_platformPath = documentsDataPath("TAC Configurations");
			_platformFile = pathObj.filename().string();
		}
		else if (_platform == eFTDI && _platformId < 90000)
		{
			_platformPath = documentsDataPath("TAC Configurations");
			_platformFile = pathObj.filename().string();
		}
	}
	else
	{
		_platformPath = pathObj.parent_path().string();
		_platformFile = pathObj.filename().string();
	}
}

void _PlatformConfiguration::setUSBDescriptor(const std::string& usbDescriptor)
{
	if (usbDescriptor != _usbDescriptor)
	{
		_usbDescriptor = usbDescriptor;
		_dirty = true;
	}
}

bool _PlatformConfiguration::load(const std::string& filePath)
{
	bool result(false);

	QJsonDocument document;
	QFile jsonFile(QString::fromStdString(filePath));

	if (jsonFile.open(QIODevice::ReadOnly))
	{
		QByteArray jsonFileContents = jsonFile.readAll();
		if (jsonFileContents.isEmpty() == false)
		{
			QJsonParseError parserError;

			document = QJsonDocument::fromJson(jsonFileContents, &parserError);
			if (parserError.error == QJsonParseError::NoError)
			{
				if (document.isNull() == false)
				{
					result = true;

					QJsonObject rootLevel = document.object();
					read(rootLevel);

					setFilePath(filePath);
				}
				else
				{
					_lastError = "JSON Document is NULL";
				}
			}
			else
			{
				_lastError = (QObject::tr("Error parsing configuration file") + parserError.errorString()).toStdString();
			}
		}
	}
	else
	{
		_lastError = (QObject::tr("Unable to open platform configuration file ") + QString::fromStdString(filePath)).toStdString();
	}

	return result;
}

void _PlatformConfiguration::save()
{
	_modifyDate = currentDateTimeString();

	if (_platformPath.empty())
	{
		if (_platform == ePSOC)
		{
			if (_platformId < 255)
				_platformPath = documentsDataPath("TAC Configurations");
			else
				_platformPath = tacConfigRoot();
		}
		else
		{
			if (_platformId < 90000)
				_platformPath = documentsDataPath("TAC Configurations");
			else
				_platformPath = tacConfigRoot();
		}
	}

	if (_platformFile.empty())
		_platformFile = makeConfigName(getPlatformString(), _platformId);

	std::string targetFilePath = (std::filesystem::path(_platformPath) / _platformFile).lexically_normal().string();

	QFile targetFile(QString::fromStdString(targetFilePath));
	if (targetFile.open(QIODevice::WriteOnly) == true)
	{
		QJsonObject rootLevel;
		QJsonDocument document;

		write(rootLevel);

		document = QJsonDocument(rootLevel);

		if (document.isNull() == false)
		{
			QByteArray jsonDocument = document.toJson(QJsonDocument::Indented);
			if (jsonDocument.isEmpty() == false)
			{
				QFile jsonFile(targetFilePath);

				if (jsonFile.open(QIODevice::WriteOnly) == true)
				{
					jsonFile.write(jsonDocument);
					jsonFile.close();

					_dirty = false;

					setFilePath(targetFilePath);
				}
			}
		}
	}
}

bool _PlatformConfiguration::read(QJsonObject& parentLevel)
{
	bool result{false};
	QJsonValue jsonValue;

	jsonValue = parentLevel.value(kName);
	if (jsonValue.isNull() == false)
		_name = jsonValue.toString();

	jsonValue = parentLevel.value(kAuthor);
	if (jsonValue.isNull() == false)
		_author = jsonValue.toString();

	jsonValue = parentLevel.value(kDescription);
	if (jsonValue.isNull() == false)
		_description = jsonValue.toString();

	jsonValue = parentLevel.value(kPlatformId);
	if (jsonValue.isNull() == false)
	{
		_platformId = static_cast<uint32_t>(jsonValue.toInt());
		result = true;
	}

	jsonValue = parentLevel.value(kPlatformType);
	if (jsonValue.isNull() == false)
	{
		setPlatform(jsonValue.toString());
		result = true;
	}

	jsonValue = parentLevel.value(kFileVersion);
	if (jsonValue.isNull() == false)
	{
		_fileVersion = static_cast<uint32_t>(jsonValue.toInt());
	}

	jsonValue = parentLevel.value(kCreationDate);
	if (jsonValue.isNull() == false)
		_creationDate = jsonValue.toString();

	jsonValue = parentLevel.value(kModifyDate);
	if (jsonValue.isNull() == false)
		_modifyDate = jsonValue.toString();

	jsonValue = parentLevel.value(kUSBDescriptor);
	if (jsonValue.isNull() == false)
		_usbDescriptor = jsonValue.toString().toLatin1();

	jsonValue = parentLevel.value(kResetEnabled);
	if (jsonValue.isNull() == false)
		_resetEnabled = jsonValue.toBool();

	jsonValue = parentLevel.value(kFormDimension);
	if (jsonValue.isNull() == false)
	{
		QString formStr = jsonValue.toString();
		auto parts = formStr.split(",", Qt::SkipEmptyParts);
		if (parts.size() >= 2)
		{
			bool okay;
			int width = parts.at(0).toInt(&okay);
			if (okay)
			{
				int height = parts.at(1).toInt(&okay);
				if (okay)
					_formDimension = QSize(width, height);
			}
		}
	}

	QJsonArray tabs = parentLevel.value(kTabs).toArray();
	if (tabs.count() > 0)
	{
		_tabs.clear();

		for (auto tabIndex: range(tabs.count()))
		{
			Tab tab;

			QJsonObject tabData = tabs.at(tabIndex).toObject();
			if (tabData.isEmpty() == false)
			{
				tab._name = tabData[kName].toString().toStdString();
				tab._userTab = tabData[kUserTab].toBool();
				tab._moveable = tabData[kMoveable].toBool();
				tab._visible = tabData[kVisible].toBool();
				tab._configurable = tabData[kConfigurable].toBool();
				tab._ordinal = tabData[kOrdinal].toInt();

				tab._hash = Tab::makeHash(tab);

				_tabs.push_back(tab);
			}
		}
	}

	QJsonArray buttons = parentLevel.value(kButtons).toArray();

	_buttons.clear();

	if (buttons.count() > 0)
	{
		for (auto buttonIndex: range(buttons.count()))
		{
			Button button;

			QJsonObject buttonData = buttons.at(buttonIndex).toObject();
			if (buttonData.isEmpty() == false)
			{
				button._label = buttonData[kName].toString().toStdString();
				button._command = buttonData[kCommand].toString().toStdString();
				button._commandGroup = CommandGroups(buttonData.value(kCommandGroup).toInt());
				button._tab = buttonData[kTab].toString().toStdString();
				button._cellLocation = toQPoint(buttonData[kCellLocation].toString().toStdString());
				button._toolTip = buttonData[kTooltip].toString().toStdString();

				button._hash = Button::makeHash(button);

				_buttons[button._hash] = button;
			}
		}
	}

	QJsonArray scriptVariables = parentLevel.value(kScriptVariables).toArray();

	_scriptVariables.clear();

	if (scriptVariables.count() > 0)
	{
		for (auto scriptVarIdx: range(scriptVariables.count()))
		{
			ScriptVariable scriptVariable;

			QJsonObject scriptVarData = scriptVariables.at(scriptVarIdx).toObject();
			if (scriptVarData.isEmpty() == false)
			{
				// TODO: Use inbuilt functions to populate the variable
				scriptVariable._name = scriptVarData[kName].toString().toStdString();
				scriptVariable._label = scriptVarData[kDefaultScriptVarLabel].toString().toStdString();
				scriptVariable._tooltip = scriptVarData[kTooltip].toString().toStdString();
				scriptVariable._type = VariableType(scriptVarData[kDefaultScriptVarType].toInt());
				scriptVariable._defaultValue = scriptVarData[kDefaultScriptDefaultValue].toString().toStdString();
				scriptVariable._cellLocation = toQPoint(scriptVarData[kCellLocation].toString().toStdString());

				_scriptVariables[scriptVariable._name] = scriptVariable;
			}
		}
	}

	jsonValue = parentLevel.value(kScript);

	_alpacaScript.clear();

	if (jsonValue.isNull() == false)
	{
		_alpacaScript = jsonValue.toVariant().toByteArray().toStdString();
	}

	_dirty = false;

	return result;
}

void _PlatformConfiguration::write(QJsonObject& parentLevel)
{
	parentLevel[kPlatformId] = static_cast<int>(_platformId);
	parentLevel[kName] = _name;
	parentLevel[kAuthor] = _author;
	parentLevel[kDescription] = _description;
	parentLevel[kFileVersion] = static_cast<int>(++_fileVersion);
	parentLevel[kCreationDate] = _creationDate;
	parentLevel[kModifyDate] = _modifyDate;
	parentLevel[kPlatformType] = QString::fromStdString(debugBoardTypeToString(_platform));
	parentLevel[kUSBDescriptor] = QString(_usbDescriptor);
	parentLevel[kResetEnabled] = _resetEnabled;
	parentLevel[kScript] = QString::fromStdString(_alpacaScript);

	if (_formDimension.isValid() && _formDimension != kClassicDimension)
		parentLevel[kFormDimension] = QString("%1,%2").arg(_formDimension.width()).arg(_formDimension.height());

	QJsonArray tabList;

	for (const auto& tab : std::as_const(_tabs))
	{
		QJsonObject element;

		element[kName] = QString::fromStdString(tab._name);
		element[kUserTab] = tab._userTab;
		element[kMoveable] = tab._moveable;
		element[kVisible] = tab._visible;
		element[kConfigurable] = tab._configurable;
		element[kOrdinal] = tab._ordinal;

		tabList.append(element);
	}

	parentLevel.insert(kTabs, tabList);

	QJsonArray buttonList;

	for (const auto& entry: std::as_const(_buttons))
	{
		const Button& button = entry.second;
		QJsonObject element;

		element[kName] = QString::fromStdString(button._label);
		element[kCommand] = QString::fromStdString(button._command);
		element[kCommandGroup] = button._commandGroup;
		element[kTab] = QString::fromStdString(button._tab);
		element[kCellLocation] = QString::fromStdString(fromQPoint(button._cellLocation));
		element[kTooltip] = QString::fromStdString(button._toolTip);

		buttonList.append(element);
	}

	parentLevel.insert(kButtons, buttonList);

	QJsonArray scriptVarList;

	for (const auto& entry: _scriptVariables)
	{
		const ScriptVariable& scriptVar = entry.second;
		QJsonObject element;

		std::string defaultValueString;
		if (std::holds_alternative<bool>(scriptVar._defaultValue))
			defaultValueString = std::get<bool>(scriptVar._defaultValue) ? "true" : "false";
		else if (std::holds_alternative<uint32_t>(scriptVar._defaultValue))
			defaultValueString = std::to_string(std::get<uint32_t>(scriptVar._defaultValue));
		else if (std::holds_alternative<std::string>(scriptVar._defaultValue))
			defaultValueString = std::get<std::string>(scriptVar._defaultValue);

		element[kName] = QString::fromStdString(scriptVar._name);
		element[kDefaultScriptVarLabel] = QString::fromStdString(scriptVar._label);
		element[kTooltip] = QString::fromStdString(scriptVar._tooltip);
		element[kDefaultScriptVarType] = scriptVar._type;
		element[kDefaultScriptDefaultValue] = QString::fromStdString(defaultValueString);
		element[kCellLocation] = QString::fromStdString(fromQPoint(scriptVar._cellLocation));

		scriptVarList.append(element);
	}

	parentLevel.insert(kScriptVariables, scriptVarList);

}

void _PlatformConfiguration::initialize()
{
	initializeDynamicPlatform();

	if (_classicButtons.empty())
	{
		Button button;

		button._label = "Power On";
		button._tab = "General";
		button._cellLocation = std::make_pair(0, 0);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Powers on the MTP/Device";
		button._command = "powerOn";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Power Off";
		button._tab = "General";
		button._cellLocation = std::make_pair(1, 0);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Powers off the MTP/Device";
		button._command = "powerOff";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Boot to EDL";
		button._tab = "General";
		button._cellLocation = std::make_pair(2, 0);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Boots the device to emergency download";
		button._command = "bootToEDL";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Boot to Fastboot";
		button._tab = "General";
		button._cellLocation = std::make_pair(0, 1);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Boots the device to fastboot";
		button._command = "bootToFastboot";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Boot to UEFI";
		button._tab = "General";
		button._cellLocation = std::make_pair(1, 1);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Boots the device to UEFI Menu";
		button._command = "bootToUEFI";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Boot to Secondary EDL";
		button._tab = "General";
		button._cellLocation = std::make_pair(2, 1);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Boots the device to secondary emergency download";
		button._command = "bootToSecondaryEDL";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;
	}
}

void _PlatformConfiguration::defaultAlpacaScript()
{
	_alpacaScript = AlpacaScript::defaultScript();
}

void _PlatformConfiguration::defaultScriptVariables()
{
	if (_scriptVariables.empty())
	{
		ScriptVariable variable;

		variable._name = "edl";
		variable._label = "EDL timing (ms)";
		variable._tooltip = "Configurable Boot to EDL timing in milliseconds";
		variable._type = eIntegerType;
		variable._defaultValue = 1300;
		variable._cellLocation = {0,0};

		_scriptVariables.insert({variable._name, variable});

		variable._name = "uefi";
		variable._label = "UEFI timing (ms)";
		variable._tooltip = "Configurable Boot to UEFI timing in milliseconds";
		variable._type = eIntegerType;
		variable._defaultValue = 8000;
		variable._cellLocation = {0,1};

		_scriptVariables.insert({variable._name, variable});

		variable._name = "fastboot";
		variable._label = "Fastboot timing (ms)";
		variable._tooltip = "Configurable Boot to fastboot timing in milliseconds";
		variable._type = eIntegerType;
		variable._defaultValue = 8000;
		variable._cellLocation = {1,0};

		_scriptVariables.insert({variable._name, variable});
	}
}

PlatformConfiguration TACPlatformEntry::getConfiguration()
{
	if (_platformConfiguration.isNull())
	{
		if (_platformEntry.isNull() == false)
		{
			const std::string& platformPath = _platformEntry->_path;

			if (platformPath.empty() == false)
			{
				_platformConfiguration = _PlatformConfiguration::openPlatformConfiguration(platformPath);
				if (_platformConfiguration.isNull() == false)
				{
					AppCore::writeToApplicationLog("TACPlatformEntry::getConfiguration() Platform " + platformPath + " loaded\n");
				}
				else
				{
					AppCore::writeToApplicationLog("TACPlatformEntry::getConfiguration() Platform " + platformPath + " load failed\n");
				}
			}
			else
			{
				_platformConfiguration = _PlatformConfiguration::createPlatformConfiguration(_platformEntry->_boardtype, 1);
				if (_platformConfiguration.isNull() == false)
				{
					AppCore::writeToApplicationLog("TACPlatformEntry::getConfiguration() Platform " + platformPath + " created\n");
				}
				else
				{
					AppCore::writeToApplicationLog("TACPlatformEntry::getConfiguration() Platform " + platformPath + " create failed\n");
				}
			}
		}
	}

	return _platformConfiguration;
}

void _PlatformConfiguration::cascadeTabDelete(const std::string &deleteMe)
{
	Q_UNUSED(deleteMe)

	Q_ASSERT("This should never be executed");
}

void _PlatformConfiguration::cascadeTabRename(const std::string &oldName, const std::string &newName)
{
	Q_UNUSED(oldName)
	Q_UNUSED(newName)

	Q_ASSERT("This should never be executed");
}
