#ifndef TACCONFIGWINDOW_H
#define TACCONFIGWINDOW_H
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
