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
// Copyright 2018-2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "PlatformConfiguration.h"

//libTAC
#include "ConsoleApplicationEnhancements.h"
#include "FTDIPlatformConfiguration.h"

// QCommon
#include "AlpacaScript.h"
#include "AppCore.h"
#include "PlatformID.h"
#include "Range.h"
#include "StringUtilities.h"

// QT
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTime>

// Parameters in the JSON TAC Configuration
const QString kName(QStringLiteral("name"));
const QString kAuthor(QStringLiteral("author"));
const QString kDescription(QStringLiteral("description"));
const QString kPlatformType(QStringLiteral("platform_type"));
const QString kFileVersion(QStringLiteral("fileVersion"));
const QString kPineVersion(QStringLiteral("pineVersion"));
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

QString _PlatformConfiguration::_lastError;
Buttons _PlatformConfiguration::_classicButtons;
bool _PlatformConfiguration::_dynamicConfigurationsInitialized{false};
TacPlatformEntries _PlatformConfiguration::_tacPlatformEntries;
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

PlatformConfiguration _PlatformConfiguration::createPlatformConfiguration
(
	DebugBoardType debugBoardType,
	int chipCount
)
{
	PlatformConfiguration result;

	switch (debugBoardType)
	{
	case eFTDI:
		result = PlatformConfiguration(new _FTDIPlatformConfiguration(chipCount));
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

		if (fileName.contains("_ftdi_", Qt::CaseInsensitive))
		{
			result = PlatformConfiguration(new _FTDIPlatformConfiguration(0));
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

PlatformID _PlatformConfiguration::getUSBDescriptor
(
	const QByteArray& usbDescriptorString
)
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
	if (platform.compare("ftdi", Qt::CaseInsensitive) == 0)
		_platform = eFTDI;
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

QString _PlatformConfiguration::getPineVersion()
{
	return QString::number(_pineVersion);
}

void _PlatformConfiguration::setPineVersion(quint32 pineVersion)
{
	_pineVersion = pineVersion;
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

void _PlatformConfiguration::setFilePath(const QString &filePath)
{
	QFileInfo fileInfo(filePath);

	_platformPath = fileInfo.absolutePath();
	_platformFile = fileInfo.fileName();
}

void _PlatformConfiguration::setUSBDescriptor(const QByteArray& usbDescriptor)
{
	if (usbDescriptor != _usbDescriptor)
	{
		_usbDescriptor = usbDescriptor;
		_dirty = true;
	}
}

bool _PlatformConfiguration::load
(
	const QString& filePath
)
{
	bool result(false);

	QJsonDocument document;
	QFile jsonFile(filePath);

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
					_lastError = QObject::tr("JSON Document is NULL") ;
				}
			}
			else
			{
				_lastError = QObject::tr("Error parsing configuration file") + parserError.errorString();
			}
		}
	}
	else
	{
		_lastError = QObject::tr("Unable to open platform configuration file ") + filePath;
	}

	return result;
}

void _PlatformConfiguration::save()
{
	_modifyDate = QDateTime::currentDateTime().toString();

	if (_platformPath.isEmpty())
	{
		if (_platform == eFTDI)
		{
			if (_platformId < 90000)
				_platformPath = documentsDataPath("TAC Configurations");
			else
				_platformPath = tacConfigRoot();
		}
	}

	if (_platformFile.isEmpty())
		_platformFile = makeConfigName(getPlatformString(), _platformId);

	QString targetFilePath = QDir::cleanPath(_platformPath + QDir::separator() + _platformFile);

	QFile targetFile(targetFilePath);
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
	copyToPineDataPath(targetFilePath);
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

	jsonValue = parentLevel.value(kPineVersion);
	if (jsonValue.isNull() == false)
	{
		_pineVersion = static_cast<quint32>(jsonValue.toInt());
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
	parentLevel[kPineVersion] = static_cast<int>(_pineVersion);
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

void _PlatformConfiguration::copyToPineDataPath(const QString& savePath)
{
#ifdef Q_OS_WIN
	QString pinePath{"C:/Program Files (x86)/Qualcomm/Shared/QTAC"};
	QString fileName = savePath.split("/").back();
	QFileInfo configFile{pinePath + QDir::separator() + fileName};

	// If the Shared/QTAC directory does not exist, create it
	QDir().mkpath(pinePath);
	
	// If the file already exists, remove it
	if (configFile.exists())
		QFile(configFile.filePath()).remove();

	// Copy the updated configuration
	QFile::copy(savePath, configFile.absoluteFilePath());
#endif
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
