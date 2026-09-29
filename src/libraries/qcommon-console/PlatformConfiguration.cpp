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
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <QStringList>
#include <QTime>

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

const QString kDefaultLabel(QStringLiteral("<type a label name>"));
const QString kDefaultTooltip(QStringLiteral("<add a tooltip>"));
const QString kDefaultCommand(QStringLiteral("<type a command>"));
const QString kDefaultGroup(QStringLiteral("<Select Pin Group>"));
const QString kDefaultTab(QStringLiteral("General"));
const QPoint kDefaultCellLocation(-1,-1);

const QString kNoVariableValue(QStringLiteral("<no value>"));

// Self-describing envelope for the shared hardware pinout file. External tools
// key off "format" == "tac-pinout".
const QString kSchema(QStringLiteral("$schema"));
const QString kSchemaUrl(QStringLiteral("https://qualcomm.github.io/tac/schemas/pinout-1.0.json"));
const QString kFormat(QStringLiteral("format"));
const QString kFormatValue(QStringLiteral("tac-pinout"));
const QString kSchemaVersion(QStringLiteral("schema_version"));
const QString kSchemaVersionValue(QStringLiteral("1.0"));

// Link from the UI overlay (.tcnf) to its sibling pinout file, and the per-pin
// key the overlay uses to join back to a hardware pin.
const QString kPinoutRef(QStringLiteral("pinout_ref"));
const QString kPinRef(QStringLiteral("ref"));
const QString kPinoutExtension(QStringLiteral(".pinout.json"));

// Keys partitioned between the two files by splitConfiguration()/mergeConfiguration().
const QString kPins(QStringLiteral("pins"));
const QString kBusArray(QStringLiteral("bus"));
const QString kChipCount(QStringLiteral("chip_count"));
const QString kChipIndex(QStringLiteral("chip_index"));
const QString kPinNumber(QStringLiteral("pin_number"));
const QString kPinIdentifier(QStringLiteral("pin"));
const QString kInput(QStringLiteral("input"));
const QString kInverted(QStringLiteral("inverted"));
const QString kInitialValue(QStringLiteral("initial_value"));
const QString kPriority(QStringLiteral("priority"));
const QString kInitializationPriority(QStringLiteral("initialization_priority"));
const QString kClassicAction(QStringLiteral("classic_action"));
const QString kSupportedFirmwareVer(QStringLiteral("supportedFirmwareVer"));
const QString kPSOCVariant(QStringLiteral("variant"));
const QString kI2CSlaveArray(QStringLiteral("slaves"));
const QString kI2CPinArray(QStringLiteral("i2c_addr"));
const QString kI2CSlaveAddress(QStringLiteral("slave_addr"));
const QString kI2CWriteAddress(QStringLiteral("write_addr"));

// Per-pin key fields (identify a physical pin; written flat into the pinout file
// and inside the "ref" object in the overlay). Platforms disagree on how a pin is
// addressed - FTDI/PSOC/PIC32CX use chip_index + bus + pin_number, STM32 uses a
// single "pin" - so every addressing field is listed here; a pin object carries
// only the ones its own platform writes.
const QStringList kPinKeyFields{kChipIndex, kBusArray, kPinNumber, kPinIdentifier};
// Per-pin hardware fields -> pinout file. Any pin field that is neither a key
// field nor a hardware field (enabled, name, help_hint, group/tab_name,
// command_group, run_priority, and any future UI field) goes to the overlay.
const QStringList kPinHardwareFields{
	kCommand, kInput, kInverted, kInitialValue, kPriority,
	kInitializationPriority, kClassicAction};

// PSOC "GPIO with I2C" boards drive some pins through an I2C GPIO expander. Those
// entries have the same hardware/UI mix as "pins" and are partitioned the same way:
// the expander address identifies the pin, command/inverted/classic_action describe
// what the hardware does, and the rest is presentation.
const QStringList kI2CPinKeyFields{kPinNumber, kI2CSlaveAddress, kI2CWriteAddress};
const QStringList kI2CPinHardwareFields{kCommand, kInverted, kClassicAction};

// Top-level keys that belong only to the shared pinout file. "variant" and "slaves"
// describe the board's I2C topology (which expander chips are fitted), so they
// travel with the hardware rather than the UI.
const QStringList kPinoutOnlyFields{
	kUSBDescriptor, kResetEnabled, kScript, kChipCount, kBusArray, kSupportedFirmwareVer,
	kPSOCVariant, kI2CSlaveArray};
// Top-level identity keys duplicated into both files (overlay stays authoritative).
const QStringList kIdentityFields{kName, kDescription, kPlatformType, kPlatformId};

QString _PlatformConfiguration::_lastError;
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

	_creationDate = QDateTime::currentDateTime().toString();
	_modifyDate = QDateTime::currentDateTime().toString();
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
	const QString& filePath
)
{
	PlatformConfiguration result;

	QFileInfo fileInfo(filePath);

	if (fileInfo.isFile())
	{
		QString fileName = fileInfo.fileName();
		if (fileName.endsWith(kPinoutExtension, Qt::CaseInsensitive))
		{
			// Shared hardware pinout files are loaded as a side effect of opening
			// their UI overlay (.tcnf); they are never opened directly here.
			_PlatformConfiguration::_lastError = "Pinout files are not opened directly";
			return result;
		}

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
			// A configuration is now two files, so opening one can fail on the sibling
			// pinout (missing, unparseable, wrong format) after the .tcnf itself read
			// fine. Discard the half-built configuration in that case: returning it
			// would hand the caller a default-initialised board that looks valid and
			// silently replaces the user's own settings on the next save.
			if (result->load(filePath) == false)
				result = PlatformConfiguration();
		}
	}

	return result;
}

QList<PlatformID> _PlatformConfiguration::platformEntryIds()
{
	QList<PlatformID> result;

	result = _tacPlatformEntries.keys();

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

QString _PlatformConfiguration::makeConfigName(const QString &chip, PlatformID platformID)
{
	return QString("TAC_") + chip + "_" + QString::number(platformID) + ".tcnf";
}

bool _PlatformConfiguration::parseConfigName
(
	const QString& fileName,
	DebugBoardType& debugBoardType,
	PlatformID& platformID
)
{
	bool result{false};

	QFileInfo fileInfo(fileName);

	if (fileInfo.suffix().toLower() == "tcnf")
	{
		QStringList fileNameParts = fileInfo.baseName().toLower().split("_");
		if (fileNameParts.count() >= 3)
		{
			if (fileNameParts.at(0) == "tac")
			{
				QString chip = fileNameParts.at(1);
				debugBoardType = debugBoardTypeFromString(chip);
				platformID = fileNameParts.at(2).toLong();

				result = true;
			}
		}
	}

	return result;
}

PlatformID _PlatformConfiguration::getUSBDescriptor(const QByteArray& usbDescriptorString)
{
	PlatformID result{0};

	QString test{usbDescriptorString.toLower()};

	for (const auto& tacEntry: std::as_const(_tacPlatformEntries))
	{
		QString candidate = tacEntry._platformEntry->_usbDescriptor.toLower();
		if (candidate == test)
		{
			result = tacEntry._platformEntry->_platformID;
			break;
		}
	}

	if (result == 0)
	{
		if (usbDescriptorString.startsWith("ALPACA-LITE "))
			result = ALPACA_LITE_ID;
	}

	return result;
}

bool _PlatformConfiguration::containsUSBDescriptor
(
	const QByteArray &usbDescriptorString
)
{
	bool result{false};

	QString test{usbDescriptorString.toLower()};

	initializeDynamicPlatform();

	for (const auto& tacEntry: std::as_const(_tacPlatformEntries))
	{
		QString candidate = tacEntry._platformEntry->_usbDescriptor.toLower();
		if (candidate == test)
		{
			result = true;
			break;
		}
	}

	return result;
}

QString _PlatformConfiguration::lastError()
{
	QString result{_PlatformConfiguration::_lastError};

	_PlatformConfiguration::_lastError.clear();

	return result;
}

void _PlatformConfiguration::setName
(
	const QString& name
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
	const QString& author
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
	const QString& description
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
			if (tab._newText.isEmpty() == false && tab._newText != tab._name)
				cascadeTabRename(tab._name, tab._newText);
		}
	}

	// now add the tabs to the list
	for (const auto& tab: tabs)
	{
		if (tab._deleted == false)
		{
			_tabs.append(tab);
		}
	}

	_dirty = true;
}

void _PlatformConfiguration::setPlatform(const QString& platform)
{
	if (platform.compare("psoc", Qt::CaseInsensitive) == 0)
		_platform = ePSOC;
	else if (platform.compare("ftdi", Qt::CaseInsensitive) == 0)
		_platform = eFTDI;
	else if (platform.compare("pic32cx (automotive)", Qt::CaseInsensitive) == 0)
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

QString _PlatformConfiguration::getFileVersion()
{
	return QString::number(_fileVersion);
}

quint32 _PlatformConfiguration::fileVersion()
{
	return _fileVersion;
}

QString _PlatformConfiguration::getPlatformString()
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

QStringList _PlatformConfiguration::getAllTabs()
{
	QStringList result;

	for (auto& tab: _tabs)
	{
		result.append(tab._name);
	}

	return result;
}

QStringList _PlatformConfiguration::getVisibleTabs()
{
	QStringList result;

	for (auto& tab: _tabs)
	{
		if (tab._visible)
			result.append(tab._name);
	}

	return result;
}

void _PlatformConfiguration::renameTab
(
	const QString& oldName,
	const QString& newName
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
	const QString &tabName,
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

	result = _buttons.values();

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

bool _PlatformConfiguration::setVariableName(const QString &variableName)
{
	bool result{false};
	if (variableName.compare(kDefaultVariableName, Qt::CaseInsensitive) != 0)
	{
		if (_scriptVariables.contains(variableName) == false)
		{
			ScriptVariable scriptVariable;
			scriptVariable._name = variableName;
			_scriptVariables.insert(variableName, scriptVariable);
		}

		result = true;
	}
	return result;
}

void _PlatformConfiguration::setVariableLabel(const QString &variableName, const QString &variableLabel)
{
	if (setVariableName(variableName))
		_scriptVariables[variableName]._label = variableLabel;
}

void _PlatformConfiguration::setVariableTooltip(const QString &variableName, const QString &variableTooltip)
{
	if (setVariableName(variableName))
		_scriptVariables[variableName]._tooltip = variableTooltip;
}

void _PlatformConfiguration::setVariableType(const QString &variableName, const VariableType variableType)
{
	if (setVariableName(variableName))
		_scriptVariables[variableName]._type = variableType;
}

bool _PlatformConfiguration::setVariableDefaultValue(const QString &variableName, const QVariant &variableDefaultValue)
{
	bool result{true};

	QString defaultValue = variableDefaultValue.toString();
	if (defaultValue.compare(kNoVariableValue, Qt::CaseInsensitive) != 0)
	{
		if (setVariableName(variableName))
			_scriptVariables[variableName]._defaultValue = variableDefaultValue;
		else
			result = false;
	}

	return result;
}

void _PlatformConfiguration::setVariableCellLocation(const QString &variableName, const QPoint &variableCellLocation)
{
	if (setVariableName(variableName))
		_scriptVariables[variableName]._cellLocation = variableCellLocation;
}

void _PlatformConfiguration::deleteVariable(const QString &variableName)
{
	if (_scriptVariables.find(variableName) != _scriptVariables.end())
	{
		_scriptVariables.remove(variableName);
	}
}

void _PlatformConfiguration::deleteButton(HashType hash)
{
	if (_buttons.find(hash) != _buttons.end())
	{
		_buttons.remove(hash);
	}
}

HashType _PlatformConfiguration::addButtonLabel(const QString &buttonLabel, const QString& tabName)
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
		for (auto& btn : _buttons)
		{
			if (btn._hash == hash)
			{
				button = btn;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		_buttons.insert(button._hash, button);
	}

	return hash;
}

bool _PlatformConfiguration::setButtonLabel(HashType hash, const QString &buttonLabel)
{
	bool result{false};
	for (auto& button: _buttons)
	{
		if (button._hash == hash)
		{
			button._label = buttonLabel;
			button._hash = Button::makeHash(button);
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonTooltip(const QString &labelName, const QString &tabName, const QString &toolTip)
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
		for (auto& btn : _buttons)
		{
			if (btn._hash == hash)
			{
				button = btn;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		button._toolTip = toolTip;
		_buttons.insert(button._hash, button);
	}

	return hash;
}

bool _PlatformConfiguration::setButtonTooltip(HashType hash, const QString &toolTip)
{
	bool result{false};
	for (auto& button: _buttons)
	{
		if (button._hash == hash)
		{
			button._toolTip = toolTip;
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonCommand(const QString& labelName, const QString& tabName, const QString &command)
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
		for (auto& btn : _buttons)
		{
			if (btn._hash == hash)
			{
				button = btn;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		button._command = command;
		_buttons.insert(button._hash, button);
	}

	return hash;
}

bool _PlatformConfiguration::setButtonCommand(HashType hash, const QString &command)
{
	bool result{false};
	for (auto& button: _buttons)
	{
		if (button._hash == hash)
		{
			button._command = command;
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonCommandGroup(const QString& labelName, const QString& tabName, const CommandGroups &commandGroup)
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
		for (auto& btn : _buttons)
		{
			if (btn._hash == hash)
			{
				button = btn;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		button._commandGroup = commandGroup;
		_buttons.insert(button._hash, button);
	}

	return hash;
}

bool _PlatformConfiguration::setButtonCommandGroup(HashType hash, const CommandGroups& commandGroup)
{
	bool result{false};
	for (auto& button: _buttons)
	{
		if (button._hash == hash)
		{
			button._commandGroup = commandGroup;
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonTab(const QString &labelName, const QString &tabName)
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
		for (auto& btn : _buttons)
		{
			if (btn._hash == hash)
			{
				button = btn;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		_buttons.insert(button._hash, button);
	}

	return hash;
}

bool _PlatformConfiguration::setButtonTab(HashType hash, const QString &tabName)
{
	bool result{false};
	for (auto& button: _buttons)
	{
		if (button._hash == hash)
		{
			button._tab = tabName;
			button._hash = Button::makeHash(button);
			result = true;

			break;
		}
	}
	return result;
}

HashType _PlatformConfiguration::addButtonCellLocation(const QString& labelName, const QString& tabName, const QPoint &cellLocation)
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
		for (auto& btn : _buttons)
		{
			if (btn._hash == hash)
			{
				button = btn;
				hashExists = true;
				break;
			}
		}
		if (hashExists == false)
		{
			button._hash = hash;
		}

		button._cellLocation = cellLocation;
		_buttons.insert(button._hash, button);
	}

	return hash;
}

bool _PlatformConfiguration::setButtonCellLocation(HashType hash, const QPoint &cellLocation)
{
	bool result{false};
	for (auto& button: _buttons)
	{
		if (button._hash == hash)
		{
			button._cellLocation = cellLocation;
			result = true;

			break;
		}
	}
	return result;
}

QSize _PlatformConfiguration::getFormDimension()
{
	if (_formDimension.isValid())
		return _formDimension;

	return kClassicDimension;
}

void _PlatformConfiguration::setFormDimension
(
	const QSize& formDimension
)
{
	if (formDimension.isValid())
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

QByteArray _PlatformConfiguration::getUSBDescriptor()
{
	return _usbDescriptor;
}

const QByteArray& _PlatformConfiguration::getAlpacaScript()
{
	return _alpacaScript;
}

void _PlatformConfiguration::setAlpacaScript
(
	const QByteArray& alpacaScript
)
{
	_alpacaScript = alpacaScript;

	_dirty = true;
}

QString _PlatformConfiguration::filePath()
{
	return QDir::cleanPath(_platformPath + QDir::separator() + _platformFile);
}

QString _PlatformConfiguration::pinoutFileName()
{
	return _pinoutFile;
}

void _PlatformConfiguration::setSupportedFirmwareVer(const QList<quint32> &firmwareList)
{
	_supportedFirmwareVer = firmwareList;
}

QList<quint32> _PlatformConfiguration::supportedFirmwareVer()
{
	return _supportedFirmwareVer;
}

void _PlatformConfiguration::setFilePath(const QString &filePath)
{
	QFileInfo fileInfo(filePath);

	if (fileInfo.absoluteFilePath().startsWith("C:/ProgramData/Qualcomm/QTAC", Qt::CaseInsensitive))
	{
		if (_platform == ePSOC && _platformId < 255)
		{
			_platformPath = documentsDataPath("TAC Configurations");
			_platformFile = fileInfo.fileName();
		}
		else if (_platform == eFTDI && _platformId < 90000)
		{
			_platformPath = documentsDataPath("TAC Configurations");
			_platformFile = fileInfo.fileName();
		}
	}
	else
	{
		_platformPath = fileInfo.absolutePath();
		_platformFile = fileInfo.fileName();
	}
}

void _PlatformConfiguration::setUSBDescriptor(const QByteArray& usbDescriptor)
{
	if (usbDescriptor != _usbDescriptor)
	{
		_usbDescriptor = usbDescriptor;
		_dirty = true;
	}
}

// Stable identity string for a physical pin, built from whichever key fields are
// present (FTDI: chip_index+bus+pin_number; PSOC/PIC32: pin_number). Used to join
// the hardware pin in the pinout file to its UI counterpart in the overlay.
static QString entryKeyString(const QJsonObject& keyHolder, const QStringList& keyFields)
{
	QStringList parts;
	for (const QString& field : keyFields)
	{
		if (keyHolder.contains(field))
			parts << keyHolder.value(field).toVariant().toString();
	}
	return parts.join(QLatin1Char('|'));
}

// Splits an array whose entries mix hardware and UI fields (pins, i2c_addr) into a
// hardware array for the pinout file and a UI array for the overlay. Key fields are
// written flat into the hardware entry and repeated inside the overlay entry's "ref"
// so the two halves can be rejoined on load.
static void splitEntryArray
(
	const QJsonArray& entries,
	const QStringList& keyFields,
	const QStringList& hardwareFields,
	QJsonArray& hardwareEntries,
	QJsonArray& overlayEntries
)
{
	const QSet<QString> keys(keyFields.cbegin(), keyFields.cend());
	const QSet<QString> hardware(hardwareFields.cbegin(), hardwareFields.cend());

	for (const QJsonValue& value : entries)
	{
		const QJsonObject entry = value.toObject();

		QJsonObject hardwareEntry;
		QJsonObject overlayEntry;
		QJsonObject ref;

		for (auto it = entry.constBegin(); it != entry.constEnd(); ++it)
		{
			const QString& key = it.key();

			if (keys.contains(key))
			{
				hardwareEntry[key] = it.value();
				ref[key] = it.value();
			}
			else if (hardware.contains(key))
			{
				hardwareEntry[key] = it.value();
			}
			else
			{
				overlayEntry[key] = it.value();
			}
		}

		overlayEntry[kPinRef] = ref;

		hardwareEntries.append(hardwareEntry);
		overlayEntries.append(overlayEntry);
	}
}

// Inverse of splitEntryArray(): joins each hardware entry to its overlay counterpart
// on the key fields. Hardware order wins; an overlay entry with no match is dropped,
// exactly as an orphaned UI row would be ignored.
static QJsonArray mergeEntryArray
(
	const QJsonArray& hardwareEntries,
	const QJsonArray& overlayEntries,
	const QStringList& keyFields
)
{
	QHash<QString, QJsonObject> overlayByKey;
	for (const QJsonValue& value : overlayEntries)
	{
		const QJsonObject overlayEntry = value.toObject();
		overlayByKey.insert(entryKeyString(overlayEntry.value(kPinRef).toObject(), keyFields), overlayEntry);
	}

	QJsonArray combined;
	for (const QJsonValue& value : hardwareEntries)
	{
		QJsonObject entry = value.toObject();

		const auto found = overlayByKey.constFind(entryKeyString(entry, keyFields));
		if (found != overlayByKey.constEnd())
		{
			const QJsonObject overlayEntry = found.value();
			for (auto it = overlayEntry.constBegin(); it != overlayEntry.constEnd(); ++it)
			{
				if (it.key() == kPinRef)
					continue;
				entry[it.key()] = it.value();
			}
		}

		combined.append(entry);
	}

	return combined;
}

static bool writeJsonObjectToFile(const QString& path, const QJsonObject& object)
{
	QJsonDocument document(object);
	if (document.isNull())
		return false;

	const QByteArray bytes = document.toJson(QJsonDocument::Indented);
	if (bytes.isEmpty())
		return false;

	QFile file(path);
	if (file.open(QIODevice::WriteOnly) == false)
		return false;

	file.write(bytes);
	file.close();

	return true;
}

QString _PlatformConfiguration::pinoutFileNameFor(const QString& overlayFileName)
{
	QString base = overlayFileName;
	const int dot = base.lastIndexOf(QLatin1Char('.'));
	if (dot > 0)
		base.truncate(dot);

	return base + kPinoutExtension;
}

void _PlatformConfiguration::splitConfiguration
(
	const QJsonObject& combined,
	QJsonObject& pinout,
	QJsonObject& overlay
)
{
	pinout = QJsonObject();
	overlay = QJsonObject();

	// Self-describing envelope so 3rd-party tools can recognise the pinout file.
	pinout[kSchema] = kSchemaUrl;
	pinout[kFormat] = kFormatValue;
	pinout[kSchemaVersion] = kSchemaVersionValue;

	const QSet<QString> pinoutOnly(kPinoutOnlyFields.cbegin(), kPinoutOnlyFields.cend());
	const QSet<QString> identity(kIdentityFields.cbegin(), kIdentityFields.cend());

	// Top-level scalar/array fields. Pins and variables are handled separately
	// because they straddle both files.
	for (auto it = combined.constBegin(); it != combined.constEnd(); ++it)
	{
		const QString& key = it.key();

		if (key == kPins || key == kScriptVariables || key == kI2CPinArray)
			continue;

		if (identity.contains(key))
		{
			pinout[key] = it.value();
			overlay[key] = it.value();
		}
		else if (pinoutOnly.contains(key))
		{
			pinout[key] = it.value();
		}
		else
		{
			overlay[key] = it.value();
		}
	}

	// Pins: hardware fields + key -> pinout; key (as "ref") + UI fields -> overlay.
	QJsonArray pinoutPins;
	QJsonArray overlayPins;
	splitEntryArray(combined.value(kPins).toArray(), kPinKeyFields, kPinHardwareFields,
	                pinoutPins, overlayPins);

	pinout[kPins] = pinoutPins;
	overlay[kPins] = overlayPins;

	// I2C expander pins (PSOC "GPIO with I2C" only): same treatment as pins. Absent
	// on every other board, and an absent key stays absent in both halves.
	if (combined.contains(kI2CPinArray))
	{
		QJsonArray pinoutI2C;
		QJsonArray overlayI2C;
		splitEntryArray(combined.value(kI2CPinArray).toArray(), kI2CPinKeyFields,
		                kI2CPinHardwareFields, pinoutI2C, overlayI2C);

		pinout[kI2CPinArray] = pinoutI2C;
		overlay[kI2CPinArray] = overlayI2C;
	}

	// Variables: defaults travel with the hardware (the script references them);
	// label/tooltip/type/layout stay in the overlay. Joined later on name.
	const QJsonArray variables = combined.value(kScriptVariables).toArray();
	QJsonArray pinoutVars;
	QJsonArray overlayVars;

	for (const QJsonValue& varValue : variables)
	{
		const QJsonObject variable = varValue.toObject();

		QJsonObject hardwareVar;
		QJsonObject uiVar;

		for (auto it = variable.constBegin(); it != variable.constEnd(); ++it)
		{
			const QString& key = it.key();

			if (key == kName)
			{
				hardwareVar[key] = it.value();
				uiVar[key] = it.value();
			}
			else if (key == kDefaultScriptDefaultValue)
			{
				hardwareVar[key] = it.value();
			}
			else
			{
				uiVar[key] = it.value();
			}
		}

		pinoutVars.append(hardwareVar);
		overlayVars.append(uiVar);
	}

	pinout[kScriptVariables] = pinoutVars;
	overlay[kScriptVariables] = overlayVars;
}

QJsonObject _PlatformConfiguration::mergeConfiguration
(
	const QJsonObject& pinout,
	const QJsonObject& overlay
)
{
	QJsonObject combined;

	// Overlay owns metadata, tabs, buttons and UI fields.
	for (auto it = overlay.constBegin(); it != overlay.constEnd(); ++it)
	{
		const QString& key = it.key();
		if (key == kPinoutRef || key == kPins || key == kScriptVariables || key == kI2CPinArray)
			continue;
		combined[key] = it.value();
	}

	// Layer in pinout-owned top-level fields (script, bus, chip_count, usb, ...).
	// Skip the envelope; identity fields already came from the overlay.
	static const QSet<QString> envelope{kSchema, kFormat, kSchemaVersion};
	for (auto it = pinout.constBegin(); it != pinout.constEnd(); ++it)
	{
		const QString& key = it.key();
		if (envelope.contains(key) || key == kPins || key == kScriptVariables || key == kI2CPinArray)
			continue;
		if (combined.contains(key) == false)
			combined[key] = it.value();
	}

	// Join hardware pins to their UI counterparts on the physical-pin key.
	combined[kPins] = mergeEntryArray(pinout.value(kPins).toArray(),
	                                  overlay.value(kPins).toArray(),
	                                  kPinKeyFields);

	// Same for the I2C expander pins, when the board has them.
	if (pinout.contains(kI2CPinArray) || overlay.contains(kI2CPinArray))
	{
		combined[kI2CPinArray] = mergeEntryArray(pinout.value(kI2CPinArray).toArray(),
		                                         overlay.value(kI2CPinArray).toArray(),
		                                         kI2CPinKeyFields);
	}

	// Join variable defaults (pinout) to labels/layout (overlay) on name.
	const QJsonArray hardwareVars = pinout.value(kScriptVariables).toArray();
	const QJsonArray uiVars = overlay.value(kScriptVariables).toArray();

	QHash<QString, QJsonObject> uiVarByName;
	for (const QJsonValue& value : uiVars)
	{
		const QJsonObject uiVar = value.toObject();
		uiVarByName.insert(uiVar.value(kName).toString(), uiVar);
	}

	QJsonArray combinedVars;
	for (const QJsonValue& value : hardwareVars)
	{
		QJsonObject variable = value.toObject();

		const auto found = uiVarByName.constFind(variable.value(kName).toString());
		if (found != uiVarByName.constEnd())
		{
			const QJsonObject uiVar = found.value();
			for (auto it = uiVar.constBegin(); it != uiVar.constEnd(); ++it)
				variable[it.key()] = it.value();
		}

		combinedVars.append(variable);
	}
	combined[kScriptVariables] = combinedVars;

	return combined;
}

bool _PlatformConfiguration::load(const QString& filePath)
{
	QFile jsonFile(filePath);
	if (jsonFile.open(QIODevice::ReadOnly) == false)
	{
		_lastError = QObject::tr("Unable to open platform configuration file ") + filePath;
		return false;
	}

	const QByteArray jsonFileContents = jsonFile.readAll();
	if (jsonFileContents.isEmpty())
	{
		_lastError = QObject::tr("Empty platform configuration file ") + filePath;
		return false;
	}

	QJsonParseError parserError;
	const QJsonDocument document = QJsonDocument::fromJson(jsonFileContents, &parserError);
	if (parserError.error != QJsonParseError::NoError)
	{
		_lastError = QObject::tr("Error parsing configuration file ") + parserError.errorString();
		return false;
	}
	if (document.isNull())
	{
		_lastError = QObject::tr("JSON Document is NULL");
		return false;
	}

	const QJsonObject overlay = document.object();
	QJsonObject combined;

	if (overlay.contains(kPinoutRef))
	{
		// New two-file format: resolve and merge the sibling pinout file.
		const QString pinoutName = overlay.value(kPinoutRef).toString();
		const QString pinoutPath = QDir(QFileInfo(filePath).absolutePath()).filePath(pinoutName);

		QFile pinoutFile(pinoutPath);
		if (pinoutFile.open(QIODevice::ReadOnly) == false)
		{
			_lastError = QObject::tr("Unable to open pinout file ") + pinoutPath;
			return false;
		}

		QJsonParseError pinoutError;
		const QJsonDocument pinoutDocument = QJsonDocument::fromJson(pinoutFile.readAll(), &pinoutError);
		if (pinoutError.error != QJsonParseError::NoError || pinoutDocument.isNull())
		{
			_lastError = QObject::tr("Error parsing pinout file ") + pinoutError.errorString();
			return false;
		}

		const QJsonObject pinout = pinoutDocument.object();
		if (pinout.value(kFormat).toString() != kFormatValue)
		{
			_lastError = QObject::tr("Unexpected pinout file format in ") + pinoutPath;
			return false;
		}

		combined = mergeConfiguration(pinout, overlay);
		_pinoutFile = pinoutName;
	}
	else
	{
		// Legacy combined configuration (pins and script stored inline).
		combined = overlay;
		_pinoutFile.clear();
	}

	read(combined);
	setFilePath(filePath);

	return true;
}

void _PlatformConfiguration::save()
{
	_modifyDate = QDateTime::currentDateTime().toString();

	if (_platformPath.isEmpty())
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

	if (_platformFile.isEmpty())
		_platformFile = makeConfigName(getPlatformString(), _platformId);

	const QString overlayPath = QDir::cleanPath(_platformPath + QDir::separator() + _platformFile);

	// Serialize the unified in-memory model, then split it into the shared pinout
	// file and the UI overlay (.tcnf) that references it.
	QJsonObject combined;
	write(combined);

	QJsonObject pinout;
	QJsonObject overlay;
	splitConfiguration(combined, pinout, overlay);

	const QString pinoutName = pinoutFileNameFor(_platformFile);
	overlay[kPinoutRef] = pinoutName;

	const QString pinoutPath = QDir::cleanPath(_platformPath + QDir::separator() + pinoutName);

	if (writeJsonObjectToFile(pinoutPath, pinout) == false)
	{
		_lastError = QObject::tr("Unable to write pinout file ") + pinoutPath;
		return;
	}

	if (writeJsonObjectToFile(overlayPath, overlay) == false)
	{
		_lastError = QObject::tr("Unable to write configuration file ") + overlayPath;
		return;
	}

	_pinoutFile = pinoutName;
	_dirty = false;

	setFilePath(overlayPath);
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
		_platformId = static_cast<quint32>(jsonValue.toInt());
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
		_fileVersion = static_cast<quint32>(jsonValue.toInt());
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
				tab._name = tabData[kName].toString();
				tab._userTab = tabData[kUserTab].toBool();
				tab._moveable = tabData[kMoveable].toBool();
				tab._visible = tabData[kVisible].toBool();
				tab._configurable = tabData[kConfigurable].toBool();
				tab._ordinal = tabData[kOrdinal].toInt();

				tab._hash = Tab::makeHash(tab);

				_tabs.append(tab);
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
				button._label = buttonData[kName].toString();
				button._command = buttonData[kCommand].toString();
				button._commandGroup = CommandGroups(buttonData.value(kCommandGroup).toInt());
				button._tab = buttonData[kTab].toString();
				button._cellLocation = toQPoint(buttonData[kCellLocation].toString());
				button._toolTip = buttonData[kTooltip].toString();

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
				scriptVariable._name = scriptVarData[kName].toString();
				scriptVariable._label = scriptVarData[kDefaultScriptVarLabel].toString();
				scriptVariable._tooltip = scriptVarData[kTooltip].toString();
				scriptVariable._type = VariableType(scriptVarData[kDefaultScriptVarType].toInt());
				scriptVariable._defaultValue = scriptVarData[kDefaultScriptDefaultValue].toString();
				scriptVariable._cellLocation = toQPoint(scriptVarData[kCellLocation].toString());

				_scriptVariables[scriptVariable._name] = scriptVariable;
			}
		}
	}

	jsonValue = parentLevel.value(kScript);

	_alpacaScript.clear();

	if (jsonValue.isNull() == false)
	{
		_alpacaScript = jsonValue.toVariant().toByteArray();
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
	parentLevel[kPlatformType] = debugBoardTypeToString(_platform);
	parentLevel[kUSBDescriptor] = QString(_usbDescriptor);
	parentLevel[kResetEnabled] = _resetEnabled;
	parentLevel[kScript] = _alpacaScript.data();

	if (_formDimension.isValid() && _formDimension != kClassicDimension)
		parentLevel[kFormDimension] = QString("%1,%2").arg(_formDimension.width()).arg(_formDimension.height());

	QJsonArray tabList;

	for (const auto& tab : std::as_const(_tabs))
	{
		QJsonObject element;

		element[kName] = tab._name;
		element[kUserTab] = tab._userTab;
		element[kMoveable] = tab._moveable;
		element[kVisible] = tab._visible;
		element[kConfigurable] = tab._configurable;
		element[kOrdinal] = tab._ordinal;

		tabList.append(element);
	}

	parentLevel.insert(kTabs, tabList);

	QJsonArray buttonList;

	for (const auto& button: std::as_const(_buttons))
	{
		QJsonObject element;

		element[kName] = button._label;
		element[kCommand] = button._command;
		element[kCommandGroup] = button._commandGroup;
		element[kTab] = button._tab;
		element[kCellLocation] = fromQPoint(button._cellLocation);
		element[kTooltip] = button._toolTip;

		buttonList.append(element);
	}

	parentLevel.insert(kButtons, buttonList);

	QJsonArray scriptVarList;

	for (const auto& scriptVar: std::as_const(_scriptVariables))
	{
		QJsonObject element;

		element[kName] = scriptVar._name;
		element[kDefaultScriptVarLabel] = scriptVar._label;
		element[kTooltip] = scriptVar._tooltip;
		element[kDefaultScriptVarType] = scriptVar._type;
		element[kDefaultScriptDefaultValue] = scriptVar._defaultValue.toString();
		element[kCellLocation] = fromQPoint(scriptVar._cellLocation);

		scriptVarList.append(element);
	}

	parentLevel.insert(kScriptVariables, scriptVarList);

}

void _PlatformConfiguration::initialize()
{
	initializeDynamicPlatform();

	if (_classicButtons.isEmpty())
	{
		Button button;

		button._label = "Power On";
		button._tab = "General";
		button._cellLocation = QPoint(0, 0);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Powers on the MTP/Device";
		button._command = "powerOn";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Power Off";
		button._tab = "General";
		button._cellLocation = QPoint(1, 0);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Powers off the MTP/Device";
		button._command = "powerOff";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Boot to EDL";
		button._tab = "General";
		button._cellLocation = QPoint(2, 0);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Boots the device to emergency download";
		button._command = "bootToEDL";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Boot to Fastboot";
		button._tab = "General";
		button._cellLocation = QPoint(0, 1);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Boots the device to fastboot";
		button._command = "bootToFastboot";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Boot to UEFI";
		button._tab = "General";
		button._cellLocation = QPoint(1, 1);
		button._commandGroup = eQuickSettingsGroup;
		button._toolTip = "Boots the device to UEFI Menu";
		button._command = "bootToUEFI";
		button._hash = Button::makeHash(button);

		_classicButtons[button._hash] = button;

		button._label = "Boot to Secondary EDL";
		button._tab = "General";
		button._cellLocation = QPoint(2, 1);
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
	if (_scriptVariables.isEmpty())
	{
		ScriptVariable variable;

		variable._name = "edl";
		variable._label = "EDL timing (ms)";
		variable._tooltip = "Configurable Boot to EDL timing in milliseconds";
		variable._type = eIntegerType;
		variable._defaultValue = 1300;
		variable._cellLocation = QPoint(0,0);

		_scriptVariables.insert(variable._name, variable);

		variable._name = "uefi";
		variable._label = "UEFI timing (ms)";
		variable._tooltip = "Configurable Boot to UEFI timing in milliseconds";
		variable._type = eIntegerType;
		variable._defaultValue = 8000;
		variable._cellLocation = QPoint(0,1);

		_scriptVariables.insert(variable._name, variable);

		variable._name = "fastboot";
		variable._label = "Fastboot timing (ms)";
		variable._tooltip = "Configurable Boot to fastboot timing in milliseconds";
		variable._type = eIntegerType;
		variable._defaultValue = 8000;
		variable._cellLocation = QPoint(1,0);

		_scriptVariables.insert(variable._name, variable);
	}
}

PlatformConfiguration TACPlatformEntry::getConfiguration()
{
	if (_platformConfiguration.isNull())
	{
		if (_platformEntry.isNull() == false)
		{
			QString platformPath = _platformEntry->_path;

			if (platformPath.isEmpty() == false)
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

void _PlatformConfiguration::cascadeTabDelete(const QString &deleteMe)
{
	Q_UNUSED(deleteMe)

	Q_ASSERT("This should never be executed");
}

void _PlatformConfiguration::cascadeTabRename(const QString &oldName, const QString &newName)
{
	Q_UNUSED(oldName)
	Q_UNUSED(newName)

	Q_ASSERT("This should never be executed");
}
