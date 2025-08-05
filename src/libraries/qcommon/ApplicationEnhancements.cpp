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

#include "ApplicationEnhancements.h"
#include "AppCore.h"
#include "Range.h"

// Qt
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <QLibrary>
#include <QMdiSubWindow>
#include <QMenu>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QTableWidget>
#include <QWidget>
#include <QDesktopServices>
#include <QUrl>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#pragma comment(lib, "shell32")
#endif

void adjustMdiSubWindow
(
	QWidget* childWindow,
	QMdiSubWindow* subMdiWindow
)
{
	QSize subMdiWindowSize = childWindow->size(); // start with our child window's size
	int mdiTitleBarHeight = QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight);
	int mdiFrameWidth = QApplication::style()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth);

	subMdiWindowSize.setHeight(subMdiWindowSize.height() + mdiTitleBarHeight + mdiFrameWidth);
	subMdiWindowSize.setWidth(subMdiWindowSize.width() + mdiFrameWidth);

	subMdiWindow->resize(subMdiWindowSize);
	if (subMdiWindow->pos().x() < 20 || subMdiWindow->pos().y() < 20)
		subMdiWindow->move(20, 20);
}

void buildRecentsMenu
(
	QMenu* recentMenu,
	RecentFiles& recentFiles,
	const QObject* receiver,
	const char* member
)
{
	if (recentMenu != Q_NULLPTR)
	{
		recentMenu->clear();

		for (auto index: range(recentFiles.fileCount()))
		{
			QString filePath = recentFiles.getFilePath(index);
			QString fileName = recentFiles.getFileName(index);

			if (fileName.isEmpty() == false)
			{
				QAction* action = recentMenu->addAction(fileName, receiver, member);
				action->setToolTip(filePath);
				action->setData(filePath);
			}
		}
	}
}

void setupApplicationStyle()
{
	QApplication::setStyle(QStyleFactory::create("Fusion"));

	QApplication* instance = qobject_cast<QApplication*>(QApplication::instance());
	if (instance == Q_NULLPTR)
	{
	}
	else
	{
		QString appPath = QCoreApplication::applicationDirPath();
		if (appPath.isEmpty() == false)
		{
			QFile styleFile(appPath + QDir::separator() + "QStyle.qss");
			if (styleFile.exists() && styleFile.open(QIODevice::ReadOnly))
			{
				QString styleSheet = styleFile.readAll();
				instance->setStyleSheet(styleSheet);

				styleFile.close();
			}
		}
	}
}

void resizeTableColumns
(
	QTableWidget* setMeUp
)
{
	QHeaderView* horizontalHeader = setMeUp->horizontalHeader();
	int tableWidth = horizontalHeader->width();
	int cummulativeColumnWidth(0);
	int columnCount = setMeUp->columnCount();
	for (auto column: range(columnCount - 1))
	{
		int columnWidth;

		setMeUp->resizeColumnToContents(column);
		columnWidth = setMeUp->columnWidth(column);
		cummulativeColumnWidth += columnWidth;
	}

	setMeUp->setColumnWidth(columnCount - 1, tableWidth - cummulativeColumnWidth);
}

void launchFolder
(
	const QString &folderName
)
{
	if (QDir().exists(folderName))
	{
#ifdef Q_OS_WIN
	ShellExecuteA(NULL, "open", folderName.toLatin1().data(), NULL, NULL, SW_SHOWMINIMIZED);
#endif
	}
}

void startLocalBrowser(const QString &filePath)
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

QString docsRoot()
{
	QString result;
	QString appName;

	appName = QCoreApplication::applicationName();
	if (appName.isEmpty())
		appName = "QTAC";

#ifdef Q_OS_WINDOWS
	result = "C:\\Program Files (x86)\\Qualcomm\\" + appName + "\\docs\\";
#endif
#ifdef Q_OS_LINUX
	result = "opt/qcom/" + appName + "/docs/";
#endif
	return result;
}
