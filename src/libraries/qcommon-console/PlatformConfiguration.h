#ifndef PLATFORMCONFIGURATION_H
#define PLATFORMCONFIGURATION_H
/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
