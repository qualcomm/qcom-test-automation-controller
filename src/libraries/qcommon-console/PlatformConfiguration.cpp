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

#include "ConfigurationFile.h"

#include <cctype>
#include <filesystem>
#include <sstream>

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
const std::string kName("name");
const std::string kAuthor("author");
const std::string kDescription("description");
const std::string kPlatformType("platform_type");
const std::string kFileVersion("fileVersion");
const std::string kPlatformId("platform_id");
const std::string kCreationDate("creation_date");
const std::string kModifyDate("modification_date");
const std::string kUSBDescriptor("usb_descriptor");
const std::string kI2cEnabled("i2c_enabled");
const std::string kResetEnabled("reset_enabled");
const std::string kFormDimension("form_dimension");

// Tabs dictionary parameters in JSON
const std::string kTabs("tabs");
// name is already defined
const std::string kUserTab("user_tab");
const std::string kMoveable("moveable");
const std::string kVisible("visible");
const std::string kConfigurable("configurable");
const std::string kOrdinal("ordinal");

// Buttons dictionary parameters in JSON
const std::string kButtons("buttons");
// name already defined
const std::string kCommand("command");
const std::string kTab("tab");
const std::string kCommandGroup("command_group");
const std::string kCellLocation("cellLocation");
const std::string kTooltip("tooltip");

// ScriptVariables dictionary parameters in JSON
const std::string kScriptVariables("variables");
// name is already defined
const std::string kDefaultScriptVarLabel("label");
// tooltip is already defined
const std::string kDefaultScriptVarType("type");
const std::string kDefaultScriptDefaultValue("default_value");
// cell location is already defined

const std::string kScript("script");

const std::string kDefaultLabel("<type a label name>");
const std::string kDefaultTooltip("<add a tooltip>");
const std::string kDefaultCommand("<type a command>");
const std::string kDefaultGroup("<Select Pin Group>");
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

	std::error_code ec;
	if (std::filesystem::is_regular_file(filePath, ec))
	{
		std::string fileName = std::filesystem::path(filePath).filename().string();
		std::string lowerFileName = fileName;
		for (char& c : lowerFileName) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

		if (lowerFileName.find("_psoc_") != std::string::npos)
		{
			result = PlatformConfiguration(new _PSOCPlatformConfiguration(ePSOCUnknown));
		}
		else if (lowerFileName.find("_ftdi_") != std::string::npos)
		{
			result = PlatformConfiguration(new _FTDIPlatformConfiguration(0));
		}
		else if (lowerFileName.find("_pic32cxauto_") != std::string::npos)
		{
			result = PlatformConfiguration(new _PIC32CXPlatformConfiguration);
		}
		else if (lowerFileName.find("_ft232h_") != std::string::npos)
		{
			result = PlatformConfiguration(new _FT232HPlatformConfiguration);
		}
		else if (lowerFileName.find("_stm32_") != std::string::npos)
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

	std::filesystem::path p(fileName);
	std::string ext = p.extension().string();
	for (char& c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

	if (ext == ".tcnf")
	{
		std::string baseName = p.stem().string();
		for (char& c : baseName) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
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
	bool result{false};

	ConfigurationFile config;
	if (!config.load(filePath))
	{
		_lastError = "Unable to open platform configuration file " + filePath;
		return false;
	}

	result = readConfig(config);
	if (result)
	{
		setFilePath(filePath);
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

	ConfigurationFile config;
	writeConfig(config);

	if (config.save(targetFilePath, true))
	{
		_dirty = false;
		setFilePath(targetFilePath);
	}
	else
	{
		_lastError = "Unable to write configuration file " + targetFilePath;
	}
}

bool _PlatformConfiguration::readConfig(const ConfigurationFile& config)
{
	bool result{false};

	if (config.hasProperty(kName))
		_name = config.getProperty(kName);

	if (config.hasProperty(kAuthor))
		_author = config.getProperty(kAuthor);

	if (config.hasProperty(kDescription))
		_description = config.getProperty(kDescription);

	if (config.hasProperty(kPlatformId))
	{
		_platformId = config.getUInt(kPlatformId);
		result = true;
	}

	if (config.hasProperty(kPlatformType))
	{
		setPlatform(config.getProperty(kPlatformType));
		result = true;
	}

	if (config.hasProperty(kFileVersion))
	{
		_fileVersion = config.getUInt(kFileVersion);
	}

	if (config.hasProperty(kCreationDate))
		_creationDate = config.getProperty(kCreationDate);

	if (config.hasProperty(kModifyDate))
		_modifyDate = config.getProperty(kModifyDate);

	if (config.hasProperty(kUSBDescriptor))
		_usbDescriptor = config.getProperty(kUSBDescriptor);

	if (config.hasProperty(kResetEnabled))
		_resetEnabled = config.getBool(kResetEnabled);

	if (config.hasProperty(kFormDimension))
	{
		std::string formStr = config.getProperty(kFormDimension);
		size_t commaPos = formStr.find(',');
		if (commaPos != std::string::npos)
		{
			try
			{
				int width = std::stoi(formStr.substr(0, commaPos));
				int height = std::stoi(formStr.substr(commaPos + 1));
				_formDimension = std::make_pair(width, height);
			}
			catch (...)
			{
			}
		}
	}

	const ConfigurationTable& tabs = config.getTable(kTabs);
	if (!tabs.empty())
	{
		_tabs.clear();

		for (const auto& tabData : tabs)
		{
			if (!tabData.empty())
			{
				Tab tab;
				tab._name = ConfigurationFile::getRecordString(tabData, kName);
				tab._userTab = ConfigurationFile::getRecordBool(tabData, kUserTab);
				tab._moveable = ConfigurationFile::getRecordBool(tabData, kMoveable);
				tab._visible = ConfigurationFile::getRecordBool(tabData, kVisible);
				tab._configurable = ConfigurationFile::getRecordBool(tabData, kConfigurable);
				tab._ordinal = ConfigurationFile::getRecordInt(tabData, kOrdinal);

				tab._hash = Tab::makeHash(tab);

				_tabs.push_back(tab);
			}
		}
	}

	const ConfigurationTable& buttons = config.getTable(kButtons);
	_buttons.clear();

	if (!buttons.empty())
	{
		for (const auto& buttonData : buttons)
		{
			if (!buttonData.empty())
			{
				Button button;
				button._label = ConfigurationFile::getRecordString(buttonData, kName);
				button._command = ConfigurationFile::getRecordString(buttonData, kCommand);
				button._commandGroup = CommandGroups(ConfigurationFile::getRecordInt(buttonData, kCommandGroup));
				button._tab = ConfigurationFile::getRecordString(buttonData, kTab);
				button._cellLocation = toQPoint(ConfigurationFile::getRecordString(buttonData, kCellLocation));
				button._toolTip = ConfigurationFile::getRecordString(buttonData, kTooltip);

				button._hash = Button::makeHash(button);

				_buttons[button._hash] = button;
			}
		}
	}

	const ConfigurationTable& scriptVariables = config.getTable(kScriptVariables);
	_scriptVariables.clear();

	if (!scriptVariables.empty())
	{
		for (const auto& scriptVarData : scriptVariables)
		{
			if (!scriptVarData.empty())
			{
				ScriptVariable scriptVariable;
				scriptVariable._name = ConfigurationFile::getRecordString(scriptVarData, kName);
				scriptVariable._label = ConfigurationFile::getRecordString(scriptVarData, kDefaultScriptVarLabel);
				scriptVariable._tooltip = ConfigurationFile::getRecordString(scriptVarData, kTooltip);
				scriptVariable._type = VariableType(ConfigurationFile::getRecordInt(scriptVarData, kDefaultScriptVarType));
				scriptVariable._defaultValue = ConfigurationFile::getRecordString(scriptVarData, kDefaultScriptDefaultValue);
				scriptVariable._cellLocation = toQPoint(ConfigurationFile::getRecordString(scriptVarData, kCellLocation));

				_scriptVariables[scriptVariable._name] = scriptVariable;
			}
		}
	}

	_alpacaScript.clear();
	if (config.hasProperty(kScript))
	{
		_alpacaScript = config.getProperty(kScript);
	}

	_dirty = false;

	return result;
}

void _PlatformConfiguration::writeConfig(ConfigurationFile& config)
{
	config.setUInt(kPlatformId, _platformId);
	config.setProperty(kName, _name);
	config.setProperty(kAuthor, _author);
	config.setProperty(kDescription, _description);
	config.setUInt(kFileVersion, ++_fileVersion);
	config.setProperty(kCreationDate, _creationDate);
	config.setProperty(kModifyDate, _modifyDate);
	config.setProperty(kPlatformType, debugBoardTypeToString(_platform));
	config.setProperty(kUSBDescriptor, _usbDescriptor);
	config.setBool(kResetEnabled, _resetEnabled);
	config.setProperty(kScript, _alpacaScript);

	if (_formDimension.first > 0 && _formDimension.second > 0 && _formDimension != kClassicDimension)
		config.setProperty(kFormDimension, std::to_string(_formDimension.first) + "," + std::to_string(_formDimension.second));

	ConfigurationTable tabList;

	for (const auto& tab : std::as_const(_tabs))
	{
		ConfigurationRecord element;

		element[kName] = tab._name;
		element[kUserTab] = tab._userTab ? "true" : "false";
		element[kMoveable] = tab._moveable ? "true" : "false";
		element[kVisible] = tab._visible ? "true" : "false";
		element[kConfigurable] = tab._configurable ? "true" : "false";
		element[kOrdinal] = std::to_string(tab._ordinal);

		tabList.push_back(element);
	}

	config.setTable(kTabs, tabList);

	ConfigurationTable buttonList;

	for (const auto& entry: std::as_const(_buttons))
	{
		const Button& button = entry.second;
		ConfigurationRecord element;

		element[kName] = button._label;
		element[kCommand] = button._command;
		element[kCommandGroup] = std::to_string(static_cast<int>(button._commandGroup));
		element[kTab] = button._tab;
		element[kCellLocation] = fromQPoint(button._cellLocation);
		element[kTooltip] = button._toolTip;

		buttonList.push_back(element);
	}

	config.setTable(kButtons, buttonList);

	ConfigurationTable scriptVarList;

	for (const auto& entry: _scriptVariables)
	{
		const ScriptVariable& scriptVar = entry.second;
		ConfigurationRecord element;

		std::string defaultValueString;
		if (std::holds_alternative<bool>(scriptVar._defaultValue))
			defaultValueString = std::get<bool>(scriptVar._defaultValue) ? "true" : "false";
		else if (std::holds_alternative<uint32_t>(scriptVar._defaultValue))
			defaultValueString = std::to_string(std::get<uint32_t>(scriptVar._defaultValue));
		else if (std::holds_alternative<std::string>(scriptVar._defaultValue))
			defaultValueString = std::get<std::string>(scriptVar._defaultValue);

		element[kName] = scriptVar._name;
		element[kDefaultScriptVarLabel] = scriptVar._label;
		element[kTooltip] = scriptVar._tooltip;
		element[kDefaultScriptVarType] = std::to_string(static_cast<int>(scriptVar._type));
		element[kDefaultScriptDefaultValue] = defaultValueString;
		element[kCellLocation] = fromQPoint(scriptVar._cellLocation);

		scriptVarList.push_back(element);
	}

	config.setTable(kScriptVariables, scriptVarList);
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
