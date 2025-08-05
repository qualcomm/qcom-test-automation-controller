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

#include "ConfigWindow.h"

// TACConfigEditor
#include "CodeEditor.h"
#include "CreateConfigurationDialog.h"
#include "ConfigEditorApplication.h"
#include "FTDIEditorView.h"
#include "ManageTabsDialog.h"
#include "PineCommandLine.h"
#include "TACPreviewWindow.h"

// libTAC
#include "DebugBoardType.h"
#include "TACDefines.h"

// QCommon
#include "AlpacaDefines.h"
#include "AlpacaSettings.h"
#include "ApplicationEnhancements.h"
#include "PlatformID.h"
#include "SystemInformation.h"

// qcommon-console
#include "ConsoleApplicationEnhancements.h"
#include "CustomValidator.h"

// QT
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QWhatsThis>
#include <QWindow>


const QByteArray kLockState{QByteArrayLiteral("lockState")};
const QByteArray kWindowTitle{QByteArrayLiteral("TAC Configuration Editor")};
const QByteArray kPIC32CXSerialIDLabel(QByteArrayLiteral("<html><body><p><span style=\" font-size:9pt; font-weight:600;\">Serial Number:</span></p></body></html>"));


ConfigWindow::ConfigWindow(QWidget* parent) :
	  QMainWindow(parent),
	  _recentConfigFiles(kTACConfigEditorApp, "TacEditorConfigRecents")
{
	setupUi(this);

	_recentFilesMenu = new QMenu(tr("Recent Configuration Files"));
	_fileMenu->insertMenu(_actionPreview, _recentFilesMenu);

	_actionManage_Tabs->setEnabled(false);
	_actionOpen_Script_Editor->setEnabled(false);

	_editorViewLayout = new QVBoxLayout(_editorFrame);
	_editorViewLayout->setObjectName(QString::fromUtf8("_editorViewLayout"));

	_nwgt = new NotificationWidget(statusbar);
	statusbar->addPermanentWidget(_nwgt);

	// lambda
	connect(_actionContents, &QAction::triggered, [=]{ startLocalBrowser(docsRoot() + "/getting-started/07-TAC-Config-Editor.html");});
	connect(_actionAbout, &QAction::triggered, [=] { ConfigEditorApplication::appInstance()->showAboutDialog();});
	connect(_buttonEditor, &ButtonEditor::buttonsTableUpdated, this, &ConfigWindow::onButtonsTableUpdated);

	restoreSettings();

	_recentConfigFiles.hideExtensions(true);

	rebuildRecents();
}

ConfigWindow::~ConfigWindow()
{
	if (_editorView != Q_NULLPTR)
	{
		delete _editorView;
		_editorView = Q_NULLPTR;
	}

	if (_editorViewLayout != Q_NULLPTR)
	{
		delete _editorViewLayout;
		_editorViewLayout = Q_NULLPTR;
	}

	if (_tacPreview != Q_NULLPTR)
	{
		delete _tacPreview;
		_tacPreview = Q_NULLPTR;
	}

	if (_nwgt != Q_NULLPTR)
	{
		delete _nwgt;
		_nwgt = Q_NULLPTR;
	}
}

void ConfigWindow::rebuildRecents()
{
	buildRecentsMenu(_recentFilesMenu, _recentConfigFiles, this, SLOT(onActionRecentMenuTriggered()));
}

void ConfigWindow::setTACConfigFile
(
	PlatformConfiguration tacConfigFile
)
{
	Q_ASSERT(tacConfigFile.isNull() == false);

	_platformConfiguration = tacConfigFile;

	QString windowTitle(this->windowTitle());
	if (tacConfigFile->filePath().isEmpty() == false)
	{
		QFileInfo tacFileInfo(tacConfigFile->filePath());

		if (!tacFileInfo.fileName().isEmpty())
			windowTitle += " - " +  tacFileInfo.fileName();
		else
			windowTitle += " - New Configuration";
	}

	setWindowTitle(windowTitle);

	_buttonEditor->setPlatformConfiguration(_platformConfiguration);
	_actionManage_Tabs->setEnabled(true);
	_actionOpen_Script_Editor->setEnabled(true);
	_actionPINEExport->setEnabled(true);
	_actionSave->setEnabled(true);
	_menuRestoreDefaults->setToolTipsVisible(true);

	populateFields();
}

bool ConfigWindow::event(QEvent *ev)
{
	if (ev->type() == QEvent::PolishRequest)
	{
		int splitterHeight = _splitter->height();

		_splitter->setSizes(QList<int>() << (splitterHeight * .65) << (splitterHeight * .35));
		return true;
	}

	return QWidget::event(ev);
}

void ConfigWindow::populateFields()
{
	_name->setEnabled(true);
	_name->setText(_platformConfiguration->name());

	SystemInformation sysInfo;
	QString detectedAuthor = sysInfo.userName();

	// We could strictly restrict the author to a single user by disabling the author field but most authors prefer
	// to add their configuration co-author's name. So, we arrive at a difficult decision of providing the freedom
	// to authors vs restricting unauthorized people from submitting configurations on other's behalf. I have left
	// the field open for editing to benefit the configuration author.
	if (!detectedAuthor.isEmpty())
	{
		if (_platformConfiguration->author().isEmpty())
			_platformConfiguration->setAuthor(detectedAuthor);
	}

	_author->setEnabled(true);
	_author->setText(_platformConfiguration->author());

	_description->setEnabled(true);
	_description->setText(_platformConfiguration->description());

	_creationDate->setText(_platformConfiguration->creationDate());
	_modificationDate->setText(_platformConfiguration->modificationDate());

	_platformConfigurationType->setEnabled(true);
	_platformConfigurationType->setText(_platformConfiguration->getPlatformString());

	_platformId->setEnabled(true);
	_platformId->setText(QString::number(_platformConfiguration->getPlatformId()));

	_pineVersion->setEnabled(true);
	_pineVersion->setText(_platformConfiguration->getPineVersion());

	switch (_platformConfiguration->getPlatform())
	{
	case eFTDI:
		_editorView = new FTDIEditorView(_editorFrame);

		_usbDescriptorLabel->setEnabled(true);
		_usbDescriptor->setEnabled(true);
		_usbDescriptor->setText(_platformConfiguration->getUSBDescriptor());
		break;

	default:
		_editorView = Q_NULLPTR;
		break;
	}

	if (_editorView != Q_NULLPTR)
	{
		_editorViewLayout->addWidget(_editorView);
		_editorView->setPlatformConfiguration(_platformConfiguration);
		_editorView->show();

		enableEditorActions();
	}

	_buttonEditor->setPlatformConfiguration(_platformConfiguration);
	_buttonEditor->setEnabled(true);

	CustomValidator platformIdValidator(_platformId, ePlatformIdValidator, _platformConfiguration->getPlatform());
	CustomValidator pineValidator(_pineVersion, ePINEVersionValidator, _platformConfiguration->getPlatform());
}

void ConfigWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;

	default:
		break;
	}
}

void ConfigWindow::closeEvent(QCloseEvent* event)
{
	saveSettings();

	if (_platformConfiguration != Q_NULLPTR)
	{
		if (_platformConfiguration->dirty())
		{
			QMessageBox::StandardButton result = QMessageBox::question(this, "Save Configuration",
				"The configuration file has changed.  Do you want to save it?",
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

			switch (result)
			{
			case QMessageBox::Yes:
				if (save() == false)
					event->ignore();
				break;
			case QMessageBox::No: event->accept(); break;
			case QMessageBox::Cancel: event->ignore(); break;
			default: break;
			}
		}
	}
	else
	{
		QMainWindow::closeEvent(event);
	}
}

void ConfigWindow::on__actionSave_triggered()
{
	save();

	if (_platformConfiguration != Q_NULLPTR)
	{
		if (_platformConfiguration->getPlatform() == eFTDI && _platformConfiguration->getPlatformId() <= 90000) // FTDI IDs greater or equal to 90000 will not be uploaded
		{
			if (_platformConfiguration->fileVersion() == 1)
				uploadConfiguration();
			else
				updateConfiguration();
		}
		else
		{
			// run update device list on experimental configurations
#ifdef Q_OS_LINUX
			QString program = "/opt/qcom/QTAC/bin/UpdateDeviceList";
#else
			QString program = "UpdateDeviceList";
#endif

			QStringList arguments;

			QProcess* process = new QProcess(Q_NULLPTR);

			process->setProgram(program);
			process->setArguments(arguments);
			process->startDetached();
		}
	}
}

void ConfigWindow::on__actionQuit_triggered()
{
	auto topLevelWindows = QGuiApplication::topLevelWindows();
	for(const auto& topWindow: topLevelWindows)
		topWindow->close();

	QCoreApplication::instance()->exit();
}

void ConfigWindow::on__actionBugWriter_triggered()
{
#ifdef Q_OS_LINUX
	QString program = "/opt/qcom/QTAC/bin/BugWriter";
#else
	QString program = "BugWriter";
#endif

	QStringList arguments;
	arguments << "product:TAC";
	arguments << "prodversion:" + kProductVersion;
	arguments << "application:TAC_Configuration_Editor";
	arguments << "appversion:" + kAppVersion;
	arguments << "listports:";
	QProcess* process = new QProcess(Q_NULLPTR);

	process->setProgram(program);
	process->setArguments(arguments);
	process->startDetached();
}

void ConfigWindow::enableEditorActions()
{
	_actionPreview->setEnabled(true);

	bool resetActive{_platformConfiguration->resetActive()};

	_resetCountEnable->setEnabled(resetActive);
	if (resetActive)
		_resetCountEnable->setChecked(_platformConfiguration->getResetEnabledState());
}

void ConfigWindow::uploadConfiguration()
{
	// do nothing
}

void ConfigWindow::updateConfiguration()
{
	// do nothing
}

bool ConfigWindow::testForTACFile()
{
	if (_platformConfiguration == Q_NULLPTR)
	{
		on__actionNew_triggered();
	}

	return _platformConfiguration != Q_NULLPTR;
}

bool ConfigWindow::open()
{
	bool result(false);

	if (_platformConfiguration != Q_NULLPTR)
	{
		ConfigWindow* openWindow;

		openWindow = new ConfigWindow;
		openWindow->show();
		openWindow->open();
	}
	else
	{
		AlpacaSettings settings(kAppName);

		QString initialDir = settings.value(kTACConfigPath, documentsDataPath(kAppName)).toString();

		QString filters = "TAC Configuration File (*.tcnf)";

		QString fileName = QFileDialog::getOpenFileName(this, "Open a TAC Configuration File", initialDir, filters);
		if (fileName.isEmpty() == false)
		{
			QFileInfo fileInfo(fileName);

			settings.setValue(kTACConfigPath, fileInfo.absolutePath());

			result = openFile(fileName);
		}
	}

	return result;
}

void ConfigWindow::on__actionNew_triggered()
{
	CreateConfigurationDialog ccd(this);

	if (ccd.exec() == QDialog::Accepted)
	{
		PlatformConfiguration platformConfig = _PlatformConfiguration::createPlatformConfiguration(ccd.getPlatform(), ccd.getChipCount());
		if (platformConfig)
		{
			if (_platformConfiguration != Q_NULLPTR)
			{
				ConfigWindow* w = new ConfigWindow;
				if (w != Q_NULLPTR)
				{
					w->setTACConfigFile(platformConfig);
					w->show();
				}
			}
			else
			{
				setTACConfigFile(platformConfig);
			}
		}
		else
			QMessageBox::warning(this, "TAC Config Editor", "This platform is not supported at the moment. Please select a different platform.");
	}
}

bool ConfigWindow::openFile(const QString& filePath)
{
	bool result{false};

	_platformConfiguration = _PlatformConfiguration::openPlatformConfiguration(filePath);
	if (_platformConfiguration != Q_NULLPTR)
	{
		setTACConfigFile(_platformConfiguration);

		_recentConfigFiles.addFile(filePath);
		rebuildRecents();

		result = true;
	}

	return result;
}

void ConfigWindow::on__actionPreview_triggered()
{
	if (_tacPreview != Q_NULLPTR)
	{
		delete _tacPreview;
		_tacPreview = Q_NULLPTR;
	}
	_tacPreview = new TACPreviewWindow(_platformConfiguration);
	_tacPreview->show();
}

void ConfigWindow::on__actionOpen_triggered()
{
	open();
}

void ConfigWindow::on__name_textChanged(const QString& newText)
{
	if (_platformConfiguration)
		_platformConfiguration->setName(newText);
}

void ConfigWindow::on__author_textChanged(const QString& newText)
{
	if (_platformConfiguration)
		_platformConfiguration->setAuthor(newText);
}

void ConfigWindow::on__platformId_textChanged(const QString& newText)
{
	PlatformID platformID = newText.toLong();
	if (_platformConfiguration)
	{
		if (platformID != _platformConfiguration->getPlatformId())
		{
			_platformConfiguration->setPlatformID(platformID);
		}
	}
}

void ConfigWindow::on__usbDescriptor_textChanged(const QString& newText)
{
	if (_platformConfiguration)
		_platformConfiguration->setUSBDescriptor(newText.toLatin1());
}

void ConfigWindow::on__resetCountEnable_clicked(bool checked)
{
	if (_platformConfiguration)
		_platformConfiguration->setResetEnabledState(checked);
}

void ConfigWindow::onConfigurationWriteComplete(const QString &filePath)
{
	statusbar->showMessage("Configuration updated: " + filePath, 500);
}

void ConfigWindow::onConfigurationWriteError(const QString &error)
{
	AppCore::writeToApplicationLogLine("[ConfigWindow::onConfigurationWriteError]: " + error);
}

void ConfigWindow::onActionRecentMenuTriggered()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action != Q_NULLPTR)
	{
		QString filePath = action->data().toString();
		if (filePath.isEmpty() == false)
		{
			openFile(filePath);
		}
	}
}

void ConfigWindow::onNotificationStarted(const QString &message)
{
	if (_nwgt != Q_NULLPTR)
	{
		_nwgt->insertNotification(message, eWarnNotification);
	}
}

bool ConfigWindow::save()
{
	bool result(false);

	if (_platformConfiguration != Q_NULLPTR)
	{
		if (_platformConfiguration != NULL)
		{
			_platformConfiguration->save();

			QFileInfo fileInfo(_platformConfiguration->filePath());
			statusbar->showMessage("TAC Configuration File: " + fileInfo.filePath() + " saved", 5000);

			QString windowTitle = kWindowTitle + " - " +  fileInfo.fileName();
			setWindowTitle(windowTitle);

			_recentConfigFiles.addFile(_platformConfiguration->filePath());
			rebuildRecents();

			result = true;

			if (_platformConfiguration->getPlatform() == eFTDI)
				_nwgt->insertNotification("Add the USB Descriptor String to the BRF document of the platform. This will help the hardware get programmed in the factory");
		}
	}

	return result;
}

void ConfigWindow::restoreSettings()
{
	AlpacaSettings settings(kAppName);
	settings.beginGroup("TACConfigEditor");

	QPoint pos = settings.value("pos", QPoint(40, 40)).toPoint();
	move(pos);

	QSize size = settings.value("size", QSize(1200, 800)).toSize();
	resize(size);

	settings.endGroup();
}

void ConfigWindow::saveSettings()
{
	AlpacaSettings settings(kAppName);

	settings.beginGroup("TACConfigEditor");

	settings.setValue("pos", pos());
	settings.setValue("size", size());

	settings.endGroup();
}

void ConfigWindow::on__actionManage_Tabs_triggered()
{
	ManageTabsDialog tabManager(_platformConfiguration, this);

	if (tabManager.exec() == QDialog::Accepted)
	{
		Tabs tabs;

		tabs = tabManager.getDeletedTabs();
		_platformConfiguration->deleteTabs(tabs);

		tabs = tabManager.getTabs();
		_platformConfiguration->updateTabs(tabs);

		_editorView->resetPlatform();
		_buttonEditor->resetPlatform();
	}
}

void ConfigWindow::on__description_textChanged(const QString& newText)
{
	if (_platformConfiguration)
		_platformConfiguration->setDescription(newText);
}

void ConfigWindow::on__actionOpen_Script_Editor_triggered()
{
	if (_codeEditor == Q_NULLPTR)
	{
		_codeEditor = new CodeEditor(_platformConfiguration);
		connect(_codeEditor, &CodeEditor::startNotification, this, &ConfigWindow::onNotificationStarted);
		connect(_codeEditor, &CodeEditor::ScriptVariablesUpdated, this, &ConfigWindow::onScriptVariablesUpdated);
	}

	_codeEditor->show();
}

void ConfigWindow::on__actionRefresh_triggered(bool checked)
{
	Q_UNUSED(checked);
	_timer.start(3000);
}

void ConfigWindow::updatePlatformId(PlatformID platformId)
{
	_platformId->setText(QString::number(platformId));
	_platformId->setEnabled(false);
}

void ConfigWindow::on__actionWhatsThis_triggered()
{
	QWhatsThis::enterWhatsThisMode();
}

void ConfigWindow::on__actionPINEExport_triggered()
{
	PineCommandLine pineCmd;
	pineCmd.invokeCli();
}

void ConfigWindow::on__pineVersion_textChanged(const QString &pineVersionString)
{
	if (_platformConfiguration != Q_NULLPTR)
	{
		if (pineVersionString != _platformConfiguration->getPineVersion())
		{
			bool okay;

			quint32 pineVersion = pineVersionString.toInt(&okay);
			if (okay)
				_platformConfiguration->setPineVersion(pineVersion);
		}
	}
}

void ConfigWindow::on__usbDescriptor_editingFinished()
{
	if (_nwgt)
	{
		if (_usbDescriptor->text().startsWith("ALPACA-LITE ") == false)
			_nwgt->insertNotification("The configuration does not follow the expected USB Descriptor String format: \'ALPACA-LITE <space> <target-name>\'", eWarnNotification);
	}
}

void ConfigWindow::on__author_editingFinished()
{
	if (_nwgt)
	{
		if (_author->text().isEmpty())
			_nwgt->insertNotification("The configuration author name cannot be left empty. Add your username as author. Example: Biswajit Roy <biswroy@qti.qualcomm.com>", eWarnNotification);
	}
}

void ConfigWindow::onButtonsTableUpdated()
{
	_actionDefaultQuickSettings->setEnabled(true);
	_actionDefaultQuickSettings->setToolTip("Clear table and restore default quick settings");
}

void ConfigWindow::onScriptVariablesUpdated()
{
	_actionDefaultScriptAndVariables->setEnabled(true);
	_actionDefaultScriptAndVariables->setToolTip("Clear table and restore default script variables");
}

void ConfigWindow::on__actionDefaultQuickSettings_triggered()
{
	if (_buttonEditor != Q_NULLPTR)
	{
		_buttonEditor->_buttonsTable->setRowCount(0);

		if (_platformConfiguration != Q_NULLPTR)
		{
			PlatformConfiguration platformConfig = _PlatformConfiguration::createPlatformConfiguration(_platformConfiguration->getPlatform());
			_buttonEditor->setPlatformConfiguration(platformConfig);
		}
	}
}

void ConfigWindow::on__actionDefaultScriptAndVariables_triggered()
{
	if (_codeEditor != Q_NULLPTR)
	{
		PlatformConfiguration platformConfig = _PlatformConfiguration::createPlatformConfiguration(_platformConfiguration->getPlatform());

		_codeEditor = new CodeEditor(platformConfig);
	}
}

