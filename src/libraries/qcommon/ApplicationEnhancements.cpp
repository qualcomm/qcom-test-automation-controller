// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

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
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleFactory>
#include <QTableWidget>
#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

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
	const QString nativePath = QDir::toNativeSeparators(QDir::cleanPath(filePath));

	if (QFileInfo::exists(filePath) == false)
	{
		qWarning().noquote() << "startLocalBrowser: help file not found:" << nativePath;
		QMessageBox::warning(QApplication::activeWindow(), QStringLiteral("Help Not Found"),
			QStringLiteral("The help file could not be found:\n\n%1\n\n"
						   "The documentation may not be installed with this package.").arg(nativePath));
		return;
	}

	if (QDesktopServices::openUrl(QUrl::fromLocalFile(filePath)) == false)
	{
		qWarning().noquote() << "startLocalBrowser: could not open" << nativePath;
		QMessageBox::warning(QApplication::activeWindow(), QStringLiteral("Unable to Open Help"),
			QStringLiteral("No application could open the help file:\n\n%1").arg(nativePath));
	}
}

QString bugWriterProgram()
{
	const QString binDir = QCoreApplication::applicationDirPath();
	return QStandardPaths::findExecutable(QStringLiteral("BugWriter"), {binDir});
}

bool bugWriterAvailable()
{
	return bugWriterProgram().isEmpty() == false;
}

bool startBugWriter(const QStringList& arguments, QWidget* parent)
{
	const QString program = bugWriterProgram();

	if (program.isEmpty())
	{
		// The menu entry is hidden when BugWriter is absent, so this only
		// happens if it was removed while the application was running.
		qWarning().noquote() << "startBugWriter: BugWriter not found in"
							 << QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
		return false;
	}

	if (QProcess::startDetached(program, arguments) == false)
	{
		const QString nativeProgram = QDir::toNativeSeparators(program);
		qWarning().noquote() << "startBugWriter: failed to start" << nativeProgram;
		QMessageBox::warning(parent, QStringLiteral("Submit Bug Report"),
			QStringLiteral("BugWriter could not be started:\n\n%1").arg(nativeProgram));
		return false;
	}

	return true;
}

// Returns the folder that holds this product's documentation (the folder that
// contains "getting-started/"), with a trailing '/'.
//
// The docs are located relative to the executable, not derived from the name of
// the folder the executable lives in. That name is "Alpaca" for the bundled
// Windows install and "bin" for the Linux install (/opt/qcom/Alpaca/bin), so it
// cannot be used as a product name. Layouts handled:
//   Windows install:  <install>/<exe>          docs in <install>/docs/QTAC/
//   Linux install:    <install>/bin/<exe>      docs in <install>/docs/QTAC/ or <install>/docs/QTAC/QTAC/
//   Developer build:  <repo>/__Builds/.../bin  docs in <repo>/docs/
QString docsRoot()
{
	const QString kProduct = "QTAC";
	const QString kMarker = "/getting-started";

	auto hasBuiltHtmlDocs = [&kMarker](const QString& root) -> bool
	{
		const QDir markerDir(root + kMarker);
		return markerDir.exists() &&
			   markerDir.entryList({QStringLiteral("*.html")}, QDir::Files).isEmpty() == false;
	};

	QStringList candidates;
	const QString exeDir = QCoreApplication::applicationDirPath();
	if (exeDir.isEmpty() == false)
	{
		for (const QString& base : {exeDir, exeDir + "/.."})
		{
			candidates << base + "/docs/" + kProduct;
			candidates << base + "/docs/" + kProduct + "/" + kProduct;
			candidates << base + "/docs";
		}

		// Developer build tree: walk up from the executable looking for built HTML docs
		// (__Builds/docs or docs/tools/_build/html) before raw docs/.
		QDir dir(exeDir);
		for (int level = 0; level < 6 && dir.cdUp(); ++level)
		{
			const QString up = dir.absolutePath();
			candidates << up + "/__Builds/docs";
			candidates << up + "/docs/tools/_build/html";
			candidates << up + "/docs/" + kProduct;
			candidates << up + "/docs";
		}
	}

#ifdef Q_OS_WINDOWS
	candidates << "C:/Program Files/Qualcomm/" + kProduct + "/docs/" + kProduct;
	candidates << "C:/Program Files/Qualcomm/Alpaca/docs/" + kProduct;
#endif
#ifdef Q_OS_LINUX
	candidates << "/opt/qcom/" + kProduct + "/docs/" + kProduct;
	candidates << "/opt/qcom/Alpaca/docs/" + kProduct + "/" + kProduct;
	candidates << "/opt/qcom/Alpaca/docs/" + kProduct;
#endif

	for (const QString& candidate : candidates)
	{
		const QString root = QDir::cleanPath(candidate);
		if (hasBuiltHtmlDocs(root))
			return root + "/";
	}

	// Legacy fixed install locations, kept as the last resort.
	QString result;
#ifdef Q_OS_WINDOWS
	result = "C:/Program Files/Qualcomm/" + kProduct + "/docs/" + kProduct + "/";
#endif
#ifdef Q_OS_LINUX
	result = "/opt/qcom/" + kProduct + "/docs/" + kProduct + "/";
#endif
	qWarning().noquote() << "docsRoot: no built HTML documentation folder found; searched" << candidates.join(", ");
	return result;
}
