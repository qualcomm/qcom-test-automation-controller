// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ApplicationEnhancements.h"
#include "AppCore.h"
#include "Range.h"

// Qt
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QRegularExpression>
#include <QDebug>
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
	QString appName = "QTAC";
	QDir binDir(QCoreApplication::applicationDirPath());
	if (binDir.exists())
	{
		const QString folderName = binDir.dirName();
		if (folderName.isEmpty() == false)
			appName = folderName;
	}

#ifdef Q_OS_WINDOWS
	result = "C:/Program Files/Qualcomm/" + appName + "/docs/";
#endif
#ifdef Q_OS_LINUX
    result = "/opt/qcom/" + appName + "/docs/";
#endif
	return result;
}

QString docPage(const QString& section, const QString& stem)
{
	// Resolve a documentation page by its descriptive stem (e.g. "EPM-Scope")
	// rather than by a full filename, deliberately ignoring both the "NN-"
	// ordering prefix and the file extension.
	//
	// Each product numbers its documentation set independently - standalone
	// QEPM, standalone QTAC and the bundled Alpaca distribution each order
	// their pages differently - so the same page is "03-EPM-Scope" in one and
	// "02-EPM-Scope" in another. Hardcoding a numbered filename therefore
	// resolves in at most one product and silently opens nothing in the
	// others, and any future renumbering breaks it again. The descriptive
	// stem is stable across all of them, so match on that.
	//
	// The extension is also ignored: the bundle ships built .html, while a
	// standalone package may ship the .md sources verbatim. Prefer .html
	// when both are present, otherwise take whatever is there.
	const QString root = docsRoot();
	const QString sectionDir = QDir::cleanPath(root + "/" + section);

	QDir dir(sectionDir);
	if (dir.exists())
	{
		QString htmlMatch;
		QString otherMatch;

		const QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
		for (const QFileInfo& entry : entries)
		{
			// Strip a leading "NN-" ordering prefix before comparing.
			QString base = entry.completeBaseName();
			static const QRegularExpression orderingPrefix("^\\d+-");
			base.remove(orderingPrefix);

			if (base.compare(stem, Qt::CaseInsensitive) != 0)
				continue;

			if (entry.suffix().compare("html", Qt::CaseInsensitive) == 0)
			{
				htmlMatch = entry.absoluteFilePath();
				break;
			}

			if (otherMatch.isEmpty())
				otherMatch = entry.absoluteFilePath();
		}

		if (htmlMatch.isEmpty() == false)
			return htmlMatch;

		if (otherMatch.isEmpty() == false)
			return otherMatch;
	}

	// Nothing matched. Return the conventional .html path so the caller still
	// has something to report, and log what was actually available so the
	// mismatch is diagnosable rather than a silently dead menu item.
	const QString fallback = QDir::cleanPath(sectionDir + "/" + stem + ".html");
	qWarning().noquote() << "docPage: no documentation page matching stem" << stem
						 << "found in" << sectionDir
						 << "- available:" << (dir.exists() ? dir.entryList(QDir::Files | QDir::NoDotAndDotDot).join(", ")
														    : QString("(directory does not exist)"));
	return fallback;
}
