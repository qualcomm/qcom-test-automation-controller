#ifndef CONFIGDIALOGPATHS_H
#define CONFIGDIALOGPATHS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// Qt
#include <QDir>
#include <QFileInfo>
#include <QStringList>

// Chooses the folder a file dialog starts in.
//  1. lastPath, if it still exists. A stored file path is reduced to its folder, so
//     values written by older versions (which stored the file) keep working.
//  2. The first candidate folder that already contains files matching nameFilter,
//     so a first-time user lands where the configuration files actually are.
//  3. The first candidate folder that exists.
//  4. The home folder.
inline QString pickDialogFolder
(
	const QString& lastPath,
	const QStringList& candidates,
	const QString& nameFilter
)
{
	if (lastPath.isEmpty() == false)
	{
		const QFileInfo info(lastPath);

		if (info.isDir())
			return QDir::cleanPath(info.absoluteFilePath());

		// A file that still exists, or a stale file path whose folder is still there.
		if ((info.isFile() || info.suffix().isEmpty() == false) && info.dir().exists())
			return QDir::cleanPath(info.absolutePath());
	}

	for (const QString& candidate : candidates)
	{
		if (candidate.isEmpty())
			continue;

		const QDir dir(candidate);
		if (dir.exists() && dir.entryList(QStringList(nameFilter), QDir::Files).isEmpty() == false)
			return QDir::cleanPath(dir.absolutePath());
	}

	for (const QString& candidate : candidates)
	{
		if (candidate.isEmpty() == false && QDir(candidate).exists())
			return QDir::cleanPath(QDir(candidate).absolutePath());
	}

	return QDir::homePath();
}

#endif // CONFIGDIALOGPATHS_H
