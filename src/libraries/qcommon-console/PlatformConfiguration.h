#ifndef PLATFORMCONFIGURATION_H
#define PLATFORMCONFIGURATION_H
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
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
			Michael Simpson (msimpson@qti.qualcomm.com)
*/

// qcommon-console
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


const int kDefaultPlatformId{99999};
const QSize kClassicDimension(560, 725);

typedef qint16 ChipIndex;

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

typedef QMap<PlatformID, TACPlatformEntry> TacPlatformEntries;

class QCOMMONCONSOLE_EXPORT _PlatformConfiguration
{
public:
	_PlatformConfiguration();
	_PlatformConfiguration(const _PlatformConfiguration&) = delete;
	_PlatformConfiguration& operator=(const _PlatformConfiguration&) = delete;

	virtual ~_PlatformConfiguration();

	static void initializeDynamicPlatform();

	static PlatformConfiguration createPlatformConfiguration(DebugBoardType debugBoardType, int chipCount = 1);
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
	quint32 fileVersion();

	QString getPineVersion();
	void setPineVersion(quint32 pineVersion);

	// Returns string version of platform type for UI elements
	QString getPlatformString();

	// Setter method to set the platform type
	void setPlatform(DebugBoardType platform);

	// The function is required to update _platform value
	// in memory after reading the string from file
	void setPlatform(const QString& platform);

	// Returns the id of the platform
	PlatformID getPlatformId();
	// Sets the id of the platform
	void setPlatformID(PlatformID platformId);

	quint32 getRevision()
	{
		return _fileVersion;
	}

	void deleteTabs(Tabs& tabs);
	void updateTabs(Tabs& tabs);
	Tabs getTabs();
	QStringList getAllTabs();
	QStringList getVisibleTabs();

	void renameTab(const QString& oldName, const QString& newName);
	void setTabVisible(const QString& tabName, bool visible);

	virtual Pins getPins();

	ButtonList getButtons();
	Buttons getButtonsMap();

	ScriptVariables getVariables();
	bool setVariableName(const QString& variableName);
	void setVariableLabel(const QString& variableName, const QString& variableLabel);
	void setVariableTooltip(const QString& variableName, const QString& variableTooltip);
	void setVariableType(const QString& variableName, const VariableType variableType);
	bool setVariableDefaultValue(const QString& variableName, const QVariant& variableDefaultValue);
	void setVariableCellLocation(const QString& variableName, const QPoint& variableCellLocation);
	void deleteVariable(const QString& variableName);

	void deleteButton(HashType hash);

	HashType addButtonLabel(const QString& buttonLabel, const QString& tabName);
	bool setButtonLabel(HashType hash, const QString& buttonLabel);

	HashType addButtonTooltip(const QString& labelName, const QString& tabName, const QString& toolTip);
	bool setButtonTooltip(HashType hash, const QString& toolTip);

	HashType addButtonCommand(const QString& labelName, const QString& tabName, const QString& command);
	bool setButtonCommand(HashType hash, const QString& command);

	HashType addButtonCommandGroup(const QString& labelName, const QString& tabName, const CommandGroups& commandGroup);
	bool setButtonCommandGroup(HashType hash, const CommandGroups& commandGroup);

	HashType addButtonTab(const QString& labelName, const QString& tabName);
	bool setButtonTab(HashType hash, const QString& tabName);

	HashType addButtonCellLocation(const QString& labelName, const QString& tabName, const QPoint& cellLocation);
	bool setButtonCellLocation(HashType hash, const QPoint& cellLocation);

	QSize getFormDimension();
	void setFormDimension(const QSize& formDimension);

	bool resetActive() { return _resetActive; }
	bool getResetEnabledState();
	void setResetEnabledState(bool newState);

	void setUSBDescriptor(const QByteArray& usbDescriptor);
	QByteArray getUSBDescriptor();

	const QByteArray& getAlpacaScript();
	void setAlpacaScript(const QByteArray& alpacaScript);

	QString filePath();

	void setFilePath(const QString& filePath);

protected:

	virtual void cascadeTabDelete(const QString& deleteMe);
	virtual void cascadeTabRename(const QString& oldName, const QString& newName);

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
	quint32						_fileVersion{0};
	quint32                     _pineVersion{1};
	QString						_name;
	QString						_author;
	QString						_description;
	QString						_creationDate;
	QString						_modifyDate;
	DebugBoardType				_platform;
	QByteArray					_usbDescriptor;
	QByteArray					_alpacaScript;

	Tabs						_tabs;
	Buttons						_buttons;
	ScriptVariables             _scriptVariables;
	QSize						_formDimension;
	bool						_resetActive{false};
	bool						_resetEnabled{false};

	static TacPlatformEntries	_tacPlatformEntries;

private:
	static void initialize();
	void copyToPineDataPath(const QString &savePath);

	static USBDescriptors		_usbDescriptors;
	static Buttons				_classicButtons;
};

#endif // PLATFORMCONFIGURATION_H
