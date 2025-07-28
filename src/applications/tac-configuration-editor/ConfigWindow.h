#ifndef TACCONFIGWINDOW_H
#define TACCONFIGWINDOW_H
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
// Copyright 2024 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_ConfigWindow.h"
#include "EditorView.h"

// ui-common
#include "NotificationWidget.h"

// libTAC
#include "PlatformConfiguration.h"

// QCommon
#include "RecentFiles.h"

// Qt
#include <QTableWidgetItem>
#include <QTimer>


class CodeEditor;
class TACPreviewWindow;

class ConfigWindow :
		public QMainWindow,
		public Ui::ConfigWindow
{
	Q_OBJECT

public:
	ConfigWindow(QWidget* parent = Q_NULLPTR);
	~ConfigWindow();

	void setTACConfigFile(PlatformConfiguration tacConfigFile);
	bool openFile(const QString& filePath);

protected:
	virtual bool event(QEvent* e);
	virtual void changeEvent(QEvent* e);
	virtual void closeEvent(QCloseEvent* event);

private slots:
	void on__actionNew_triggered();
	void on__actionOpen_triggered();

	void on__actionSave_triggered();
	void on__actionQuit_triggered();
	void on__actionBugWriter_triggered();

	void on__actionPreview_triggered();
	void on__actionManage_Tabs_triggered();
	void on__actionOpen_Script_Editor_triggered();

	void on__actionRefresh_triggered(bool checked);
	void on__actionWhatsThis_triggered();

	void on__actionPINEExport_triggered();
	void on__pineVersion_textChanged(const QString &arg1);

	void on__usbDescriptor_editingFinished();
	void on__author_editingFinished();

	void onButtonsTableUpdated();
	void onScriptVariablesUpdated();

	void on__actionDefaultQuickSettings_triggered();
	void on__actionDefaultScriptAndVariables_triggered();

protected slots:
	void on__name_textChanged(const QString& newText);
	void on__author_textChanged(const QString& newText);
	void on__description_textChanged(const QString &newText);
	void on__platformId_textChanged(const QString& newText);
	void on__usbDescriptor_textChanged(const QString& newText);
	void on__resetCountEnable_clicked(bool checked);

	void updatePlatformId(PlatformID platformId);

	void onConfigurationWriteComplete(const QString& filePath);
	void onConfigurationWriteError(const QString& error);

	void onActionRecentMenuTriggered();

	void onNotificationStarted(const QString& message);

private:
	RecentFiles					_recentConfigFiles;
	QMenu*						_recentFilesMenu{Q_NULLPTR};
	bool						_inUse{false};
	PlatformConfiguration		_platformConfiguration;
	EditorView*					_editorView{Q_NULLPTR};
	QVBoxLayout*				_editorViewLayout{Q_NULLPTR};
	TACPreviewWindow*			_tacPreview{Q_NULLPTR};
	CodeEditor*					_codeEditor{Q_NULLPTR};
	QTimer                      _timer;
	NotificationWidget*         _nwgt{Q_NULLPTR};

	void restoreSettings();
	void saveSettings();

	void rebuildRecents();
	bool open();

	bool save();

	void populateFields();
	bool testForTACFile();
	void enableEditorActions();

	void uploadConfiguration();
	void updateConfiguration();
};

#endif // TACCONFIGWINDOW_H
