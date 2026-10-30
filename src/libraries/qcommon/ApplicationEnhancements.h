#ifndef APPLICATIONENHANCEMENTS_H
#define APPLICATIONENHANCEMENTS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// QCommon
#include "QCommonGlobal.h"
#include "RecentFiles.h"

// QT
class QMdiSubWindow;
class QMenu;
#include <QSettings>
#include <QString>
#include <QStringList>
class QTableWidget;
class QWidget;

QString QCOMMON_EXPORT createFilenameTimeStamp();

void QCOMMON_EXPORT adjustMdiSubWindow(QWidget* childWindow, QMdiSubWindow* subMdiWindow);
void QCOMMON_EXPORT buildRecentsMenu(QMenu* recentsMenu, RecentFiles& recentFiles, const QObject* receiver, const char* member);
void QCOMMON_EXPORT setupApplicationStyle();

void QCOMMON_EXPORT resizeTableColumns(QTableWidget* setMeUp);
// Opens a local file (normally the HTML help) in the default browser. Tells
// the user, with the path, when the file is missing or cannot be opened.
void QCOMMON_EXPORT startLocalBrowser(const QString& filePath);

// Bug reporting is offered only by distributions that ship BugWriter next to
// the application. bugWriterProgram() returns its full path, or an empty
// string when it is not installed; windows hide their "Submit Bug Report"
// entry when bugWriterAvailable() is false.
QString QCOMMON_EXPORT bugWriterProgram();
bool QCOMMON_EXPORT bugWriterAvailable();

// Starts BugWriter with the given arguments. Warns the user if it is present
// but fails to start. Returns true when BugWriter was started.
bool QCOMMON_EXPORT startBugWriter(const QStringList& arguments, QWidget* parent = nullptr);
void launchFolder(const QString &folderName);

QString QCOMMON_EXPORT docsRoot();

#endif
