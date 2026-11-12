// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PLATFORMCONFIGURATION_H
#define PLATFORMCONFIGURATION_H

// QCommonConsole
#include "QCommonConsoleGlobal.h"
#include "Button.h"
#include "PinEntry.h"
#include "PlatformID.h"
#include "ScriptVariable.h"
#include "StringUtilities.h"
#include "Tabs.h"
#include "USBDescriptors.h"

// Qt
#include <QJsonDocument>
#include <QMap>
#include <QSharedPointer>
#include <QSize>
#include <QString>

#include <cstdint>

const int kDefaultPlatformId{99999};
const int kDefaultFirmwareVersion{15};
const QSize kClassicDimension(560, 725);

typedef int16_t ChipIndex;

class _PlatformConfiguration;

typedef QSharedPointer<_PlatformConfiguration> PlatformConfiguration;

struct QCOMMONCONSOLE_EXPORT TACPlatformEntry
{
	TACPlatformEntry() = default;

	PlatformEntry				_platformEntry;

	PlatformConfiguration getConfiguration();

protected:
	PlatformConfiguration		_platformConfiguration;
};

typedef QMap<PlatformID, TACPlatformEntry> TACPlatformEntries;

class QCOMMONCONSOLE_EXPORT _PlatformConfiguration
{
public:
	_PlatformConfiguration();
	_PlatformConfiguration(const _PlatformConfiguration&) = delete;
	_PlatformConfiguration& operator=(const _PlatformConfiguration&) = delete;

	virtual ~_PlatformConfiguration();

	static void initializeDynamicPlatform();

	static PlatformConfiguration createPlatformConfiguration(DebugBoardType debugBoardType, int chipCount = 1, PSOCVariant psocVariant = ePSOCUnknown);
	static PlatformConfiguration openPlatformConfiguration(const QString& filePath);

	static QList<PlatformID> platformEntryIds();
	static TACPlatformEntry getEntry(PlatformID platformID);

	static QString makeConfigName(const QString& chip, PlatformID platformID);
	static bool parseConfigName(const QString& fileName, DebugBoardType& debugBoardType, PlatformID &platformID);

	static PlatformID getUSBDescriptor(const QByteArray& usbDescriptorString);
	static bool containsUSBDescriptor(const QByteArray& usbDescriptorString);

	static QString getLastError() { return _lastError; }

	bool dirty() { return _dirty; }

	bool load(const QString& filePath);
	void save();

	static QString lastError();

	QString name() { return _name; }
	void setName(const QString& name);

	QString author() { return _author; }
	void setAuthor(const QString& author);

	QString description() { return _description; }
	void setDescription(const QString& description);

	QString creationDate() { return _creationDate; }

	QString modificationDate() { return _modifyDate; }

	// Getter method to get the platform type
	DebugBoardType getPlatform() { return _platform; }

	// auto incremented file version to provide file history
	QString getFileVersion();
	uint32_t fileVersion();

	// Returns string version of platform type for UI elements
	QString getPlatformString();

	// Setter method to set the platform type
	void setPlatform(DebugBoardType platform);

	// The function is required to update _platform value
	// in memory after reading the string from file
	void setPlatform(const QString& platform);

	virtual PSOCVariant variant();

	// Returns the id of the platform
	PlatformID getPlatformId();
	// Sets the id of the platform
	void setPlatformID(PlatformID platformId);

	void deleteTabs(Tabs& tabs);
	void updateTabs(Tabs& tabs);
	Tabs getTabs();
	QStringList getAllTabs();
	QStringList getVisibleTabs();

	void renameTab(const std::string& oldName, const std::string& newName);
	void setTabVisible(const std::string& tabName, bool visible);

	virtual Pins getPins();

	ButtonList getButtons();
	Buttons getButtonsMap();

	ScriptVariables getVariables();
	bool setVariableName(const std::string& variableName);
	void setVariableLabel(const std::string& variableName, const std::string& variableLabel);
	void setVariableTooltip(const std::string& variableName, const std::string& variableTooltip);
	void setVariableType(const std::string& variableName, const VariableType variableType);
	bool setVariableDefaultValue(const std::string& variableName, const std::string& variableDefaultValue);
	void setVariableCellLocation(const std::string& variableName, const std::pair<int, int>& variableCellLocation);
	void deleteVariable(const std::string& variableName);

	void deleteButton(HashType hash);

	HashType addButtonLabel(const std::string& buttonLabel, const std::string& tabName);
	bool setButtonLabel(HashType hash, const std::string& buttonLabel);

	HashType addButtonTooltip(const std::string& labelName, const std::string& tabName, const std::string& toolTip);
	bool setButtonTooltip(HashType hash, const std::string& toolTip);

	HashType addButtonCommand(const std::string& labelName, const std::string& tabName, const std::string& command);
	bool setButtonCommand(HashType hash, const std::string& command);

	HashType addButtonCommandGroup(const std::string& labelName, const std::string& tabName, const CommandGroups& commandGroup);
	bool setButtonCommandGroup(HashType hash, const CommandGroups& commandGroup);

	HashType addButtonTab(const std::string& labelName, const std::string& tabName);
	bool setButtonTab(HashType hash, const std::string& tabName);

	HashType addButtonCellLocation(const std::string& labelName, const std::string& tabName, const std::pair<int, int>& cellLocation);
	bool setButtonCellLocation(HashType hash, const std::pair<int, int>& cellLocation);

	QSize getFormDimension();
	void setFormDimension(const QSize& formDimension);

	bool resetActive() { return _resetActive; }
	bool getResetEnabledState();
	void setResetEnabledState(bool newState);

	void setUSBDescriptor(const QByteArray& usbDescriptor);
	QByteArray getUSBDescriptor();

	const std::string& getAlpacaScript();
	void setAlpacaScript(const std::string& alpacaScript);

	void setFilePath(const QString& filePath);
	QString filePath();

	void setSupportedFirmwareVer(const QList<uint32_t> &firmwareList);
	QList<uint32_t> supportedFirmwareVer();

protected:

	virtual void cascadeTabDelete(const std::string& deleteMe);
	virtual void cascadeTabRename(const std::string& oldName, const std::string& newName);

	virtual bool read(QJsonObject& parentLevel);
	virtual void write(QJsonObject& parentLevel);

	void defaultAlpacaScript();
	void defaultScriptVariables();

	bool						_dirty{false};
	static QString				_lastError;
	static bool					_dynamicConfigurationsInitialized;

	QString						_platformPath;
	QString						_platformFile;
	PlatformID					_platformId{kDefaultPlatformId};
	uint32_t					_fileVersion{0};
	QString						_name;
	QString						_author;
	QString						_description;
	QString						_creationDate;
	QString						_modifyDate;
	DebugBoardType				_platform;
	QByteArray					_usbDescriptor;
	std::string					_alpacaScript;
	QList<uint32_t>				_supportedFirmwareVer;

	Tabs						_tabs;
	Buttons						_buttons;
	ScriptVariables             _scriptVariables;
	QSize						_formDimension;
	bool						_resetActive{false};
	bool						_resetEnabled{false};

	static TACPlatformEntries	_tacPlatformEntries;

private:
	static void initialize();

	static USBDescriptors		_usbDescriptors;
	static Buttons				_classicButtons;
};

#endif // PLATFORMCONFIGURATION_H
