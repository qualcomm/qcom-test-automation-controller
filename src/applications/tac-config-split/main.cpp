// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

// TAC Config Split
//
// Migrates legacy combined *.tcnf configuration files into the two-file layout:
// a shared, self-describing *.pinout.json (hardware pinout + automation script)
// that external tools can consume directly, plus a slimmed *.tcnf UI overlay that
// references it. The split reuses the production load()/save() path so field
// fidelity matches the application's own understanding of the format.

// QCommonConsole
#include "AppCore.h"
#include "PlatformConfiguration.h"
#include "PreferencesBase.h"
#include "QCommonConsole.h"
#include "version.h"

// QT
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

// C++
#include <iostream>

namespace
{
const QByteArray kPinoutRefMarker{"\"pinout_ref\""};
const QString kPinoutSuffix{QStringLiteral(".pinout.json")};

QString pinoutNameFor(const QString& tcnfFileName)
{
	QString base = tcnfFileName;
	const int dot = base.lastIndexOf(QLatin1Char('.'));
	if (dot > 0)
		base.truncate(dot);

	return base + kPinoutSuffix;
}

bool alreadySplit(const QString& filePath)
{
	QFile file(filePath);
	if (file.open(QIODevice::ReadOnly) == false)
		return false;

	const QByteArray contents = file.readAll();
	file.close();

	return contents.contains(kPinoutRefMarker);
}

void printUsage()
{
	std::cout
		<< "Usage: TACConfigSplit [directory] [--dry-run]\n\n"
		<< "Splits each legacy combined *.tcnf in <directory> into a shared\n"
		<< "*.pinout.json (hardware pinout + script) plus a slim *.tcnf UI overlay\n"
		<< "that references it. Files already in the two-file format are skipped.\n\n"
		<< "  directory   Folder to migrate (default: current directory)\n"
		<< "  --dry-run   List the files that would be written without changing anything\n"
		<< "  --help, -h  Show this help\n";
}
} // namespace

int main(int argc, char* argv[])
{
	const QString kAppName{"TACConfigSplit"};

	QCoreApplication app(argc, argv);
	app.setApplicationName(kAppName);
	app.setApplicationVersion(TAC_CONFIG_SPLIT_VERSION);

	AppCore* appCore = AppCore::getAppCore();
	PreferencesBase preferencesBase;
	preferencesBase.setAppName(kAppName.toLatin1(), TAC_CONFIG_SPLIT_VERSION);
	appCore->setPreferences(&preferencesBase);

	InitializeQCommonConsole();

	QStringList args = app.arguments();
	args.removeFirst(); // drop program name

	if (args.contains("--help") || args.contains("-h"))
	{
		printUsage();
		return 0;
	}

	const bool dryRun = (args.removeAll("--dry-run") > 0);
	const QString targetDir = args.isEmpty() ? QDir::currentPath() : args.first();

	QDir configDir(targetDir);
	if (configDir.exists() == false)
	{
		std::cerr << "Directory does not exist: " << targetDir.toStdString() << std::endl;
		return 1;
	}

	const QFileInfoList entries =
		configDir.entryInfoList(QStringList() << "*.tcnf", QDir::Files, QDir::Name);

	int migrated = 0;
	int skipped = 0;
	int failed = 0;

	for (const QFileInfo& entry : entries)
	{
		const QString tcnfPath = entry.filePath();
		const QString pinoutName = pinoutNameFor(entry.fileName());
		const QString pinoutPath = configDir.filePath(pinoutName);

		if (alreadySplit(tcnfPath))
		{
			std::cout << "skip (already split): " << entry.fileName().toStdString() << std::endl;
			++skipped;
			continue;
		}

		if (dryRun)
		{
			std::cout << "would write: " << entry.fileName().toStdString()
			          << "  +  " << pinoutName.toStdString() << std::endl;
			++migrated;
			continue;
		}

		PlatformConfiguration config = _PlatformConfiguration::openPlatformConfiguration(tcnfPath);
		if (config.isNull())
		{
			std::cerr << "FAILED to load: " << entry.fileName().toStdString()
			          << "  (" << _PlatformConfiguration::getLastError().toStdString() << ")"
			          << std::endl;
			++failed;
			continue;
		}

		config->save();

		if (QFile::exists(pinoutPath) == false)
		{
			std::cerr << "FAILED to write pinout for: " << entry.fileName().toStdString()
			          << "  (" << _PlatformConfiguration::getLastError().toStdString() << ")"
			          << std::endl;
			++failed;
			continue;
		}

		std::cout << "split: " << entry.fileName().toStdString()
		          << "  ->  " << entry.fileName().toStdString()
		          << " + " << pinoutName.toStdString() << std::endl;
		++migrated;
	}

	std::cout << "\nDone. "
	          << migrated << (dryRun ? " to migrate, " : " migrated, ")
	          << skipped << " skipped, "
	          << failed << " failed." << std::endl;

	return (failed > 0) ? 1 : 0;
}
