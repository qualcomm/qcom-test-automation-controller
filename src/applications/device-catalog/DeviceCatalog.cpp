// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "AppCore.h"
#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"
#include "DeviceCatalog.h"
#include "DeviceSelectionDialog.h"
#include "USBDescriptors.h"

// Qt

#include <QApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QProcess>
#include <QProgressDialog>
#include <QStandardPaths>
#include <QThread>

#ifdef Q_OS_WINDOWS
	#include <Windows.h>
#endif
#ifdef Q_OS_LINUX
	#include <unistd.h>
#endif

const int kConfigNameCol{0};
const int kBoardType{1};
const int kPlatformID{2};

const QByteArray kv16FirmwareNotice(QByteArrayLiteral("You've chosen to program the v16 firmware. This firmware may contain updates not applicable to all teams. Uncheck unless you know what you're doing"));
const QByteArray kv17FirmwareNotice(QByteArrayLiteral("You've chosen to program the v17 firmware. This firmware may contain updates not applicable to all teams. Uncheck unless you know what you're doing"));
const QByteArray kv18FirmwareNotice(QByteArrayLiteral("You've chosen to program the v18 firmware. This firmware may contain updates not applicable to all teams. Uncheck unless you know what you're doing"));
const QByteArray kv19FirmwareNotice(QByteArrayLiteral("You've chosen to program the v19 firmware. This firmware may contain updates not applicable to all teams. Uncheck unless you know what you're doing"));
const QByteArray kDefaultNotice(QByteArrayLiteral("This space is used to share notification to user"));
const QStringList kVariants{"LP030", "LP038"};

// FWUpdate can take a while to flash a board. Killing it mid-flash can leave
// the board unprogrammed, so allow generous time before giving up.
const int kFwUpdateStartTimeoutMs{10000};
const int kFwUpdateFinishTimeoutMs{120000};

const QString kFwUpdateToolTip(QStringLiteral("Update the debug board firmware for connected PSOC debug board(s)"));
const QString kFwUpdateMissingToolTip(QStringLiteral("Firmware update is not available: the FWUpdate utility is not included in this installation"));

// FWUpdate ends with "Press any key to exit..." and waits for a key. Device Catalog
// answers the prompt, and stops the tool if it still has not exited after this time.
const int kFwUpdatePromptGraceMs{3000};
const QString kFwUpdatePromptText(QStringLiteral("Press any key to exit"));
const QString kFwUpdateSuccessText(QStringLiteral("Successfully updated"));
const QString kFwUpdateNoTargetsText(QStringLiteral("Found 0 SPMv4 targets"));
// Printed by FWUpdate when select=<serial> matched no connected board.
const QString kFwUpdateNoMatchText(QStringLiteral("No board matching serial number"));

// Open errors (from PSOCDevice::open()) where the board answered, so a firmware
// update can still reach it and may be the way to recover it.
const QString kBoardIdNotSetText(QStringLiteral("board ID is not set"));
const QString kOldFirmwareText(QStringLiteral("Firmware version is too old"));

namespace
{

// FWUpdate is a separate utility that is not built from this repository and
// is not part of every installation. Returns its full path, or an empty
// string when it is not installed next to this application.
QString fwUpdateProgram()
{
	return QStandardPaths::findExecutable(QStringLiteral("FWUpdate"), {applicationBinPath()});
}

// Firmware update trace: always to the debug output, and to the application
// log when application logging is enabled.
void fwLog(const QString& message)
{
	const QString line = QStringLiteral("DeviceCatalog FWUpdate: ") + message;

	qInfo().noquote() << line;

	AppCore* appCore = AppCore::getAppCore();
	if (appCore != Q_NULLPTR && appCore->appLoggingActive())
		AppCore::writeToApplicationLogLine(line);
}

struct FwUpdateRun
{
	bool started{false};
	bool finished{false};
	bool stoppedAtPrompt{false};
	QProcess::ExitStatus exitStatus{QProcess::NormalExit};
	int exitCode{-1};
	QString output;
	QString errorString;
};

// Runs FWUpdate with a progress dialog, keeping the window responsive. Killing the
// tool mid-flash can leave the board unprogrammed, so there is no Cancel button.
FwUpdateRun runFwUpdate(QWidget* parent, const QString& program, const QStringList& arguments, const QString& label)
{
	FwUpdateRun run;
	QProcess process;

	process.setProgram(program);
	process.setArguments(arguments);
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start();

	if (process.waitForStarted(kFwUpdateStartTimeoutMs) == false)
	{
		run.errorString = process.errorString();
		return run;
	}

	run.started = true;

	QProgressDialog progress(label, QString(), 0, 0, parent);
	progress.setWindowTitle("Updating Firmware");
	progress.setWindowModality(Qt::WindowModal);
	progress.setWindowFlag(Qt::WindowCloseButtonHint, false);
	progress.setCancelButton(Q_NULLPTR);
	progress.setMinimumDuration(0);
	progress.show();
	QApplication::processEvents();

	QElapsedTimer timer;
	qint64 promptSeenAt{-1};

	timer.start();
	while (true)
	{
		if (process.waitForFinished(200))
		{
			run.finished = true;
			break;
		}

		run.output += QString::fromLocal8Bit(process.readAll());
		QApplication::processEvents();

		if (promptSeenAt < 0 && run.output.contains(kFwUpdatePromptText, Qt::CaseInsensitive))
		{
			promptSeenAt = timer.elapsed();
			process.write("\n");
		}

		if (promptSeenAt >= 0 && timer.elapsed() - promptSeenAt > kFwUpdatePromptGraceMs)
		{
			// The tool finished its work and is only waiting for a key.
			run.stoppedAtPrompt = true;
			break;
		}

		if (timer.elapsed() > kFwUpdateFinishTimeoutMs)
			break;
	}

	if (run.finished == false)
	{
		process.kill();
		process.waitForFinished(5000);
	}

	run.output += QString::fromLocal8Bit(process.readAll());
	run.output = run.output.trimmed();
	run.exitStatus = process.exitStatus();
	run.exitCode = process.exitCode();

	progress.close();

	return run;
}

// Newer FWUpdate builds accept select=<serial> and update only that board.
// Older builds ignore unknown arguments and update every connected board,
// so check the help text before relying on it.
bool fwUpdateSupportsSelect(const QString& program)
{
	QProcess process;
	process.setProgram(program);
	process.setArguments({"-h"});
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start();
	if (process.waitForStarted(5000) == false)
		return false;
	if (process.waitForFinished(10000) == false)
	{
		process.kill();
		process.waitForFinished(1000);
		return false;
	}
	return QString::fromLocal8Bit(process.readAll()).contains("select=", Qt::CaseInsensitive);
}

bool fwUpdateSucceeded(const FwUpdateRun& run)
{
	if (run.started == false)
		return false;

	if (run.output.contains(kFwUpdateSuccessText, Qt::CaseInsensitive))
		return true;

	if (run.output.contains(kFwUpdateNoTargetsText, Qt::CaseInsensitive))
		return false;

	// Builds that do not print the success line: trust a clean exit.
	return run.finished && run.exitStatus == QProcess::NormalExit && run.exitCode == 0;
}

} // namespace


DeviceCatalog::DeviceCatalog(QWidget* parent) :QDialog(parent)
{
	const QString kUSBDescriptorPath{tacConfigRoot() + kUSBDescriptorFileName};

	setupUi(this);

	_deviceTable->setFocusPolicy(Qt::NoFocus);

	connect(_deviceTable, &QTableWidget::itemClicked, this, &DeviceCatalog::onConfigurationLinkClicked);
	connect(_deviceTable, &QTableWidget::customContextMenuRequested, this, &DeviceCatalog::onCustomContextMenuRequested);

	USBDescriptors usbDescriptors;

	if (usbDescriptors.load(kUSBDescriptorPath) == true)
	{
		int row{0};
		DescriptorList descriptorList = usbDescriptors.getDescriptors();

		_deviceTable->setRowCount(descriptorList.count());
		for (const auto& descriptor: std::as_const(descriptorList))
		{
			QTableWidgetItem* twi;

			twi = new QTableWidgetItem(descriptor._name);
			twi->setTextAlignment(Qt::AlignHCenter);
			twi->setData(Qt::UserRole, descriptor._name);
			_deviceTable->setItem(row, kConfigNameCol, twi);

			twi = new QTableWidgetItem(debugBoardTypeToString(descriptor._debugBoardType));
			twi->setTextAlignment(Qt::AlignHCenter);
			twi->setData(Qt::UserRole, descriptor._debugBoardType);
			_deviceTable->setItem(row, kBoardType, twi);

			twi = new QTableWidgetItem(QString::number(descriptor._platformID));
			twi->setTextAlignment(Qt::AlignHCenter);
			twi->setData(Qt::UserRole, descriptor._platformID);
			_deviceTable->setItem(row, kPlatformID, twi);

			if (descriptor._usbDescriptor.isEmpty() == false)
			{
				twi = new QTableWidgetItem(QString(descriptor._usbDescriptor));
				twi->setTextAlignment(Qt::AlignHCenter);
				_deviceTable->setItem(row, 3, twi);
			}
			else
			{
				twi = new QTableWidgetItem("-");
				twi->setTextAlignment(Qt::AlignHCenter);
				_deviceTable->setItem(row, 3, twi);
			}

			if (descriptor._revision)
			{
				twi = new QTableWidgetItem(QString(QString::number(descriptor._revision)));
				twi->setTextAlignment(Qt::AlignHCenter);
				_deviceTable->setItem(row, 4, twi);
			}
			else
			{
				twi = new QTableWidgetItem("0");
				twi->setTextAlignment(Qt::AlignHCenter);
				_deviceTable->setItem(row, 4, twi);
			}

			if (descriptor._configurationFilePath.isEmpty() == false)
			{
				twi = new QTableWidgetItem(QString(descriptor._configurationFilePath));
				twi->setToolTip("Click to open the configuration in TAC Configuration Editor");
				_deviceTable->setItem(row, 5, twi);
			}
			else
			{
				twi = new QTableWidgetItem("-");
				twi->setToolTip("Configuration file is not available for this platform");
				twi->setTextAlignment(Qt::AlignHCenter);
				_deviceTable->setItem(row, 5, twi);
			}

			row++;
		}

		_deviceTable->resizeColumnsToContents();
	}

	_firmwareDir = applicationDataPath().toLatin1() + QDir::separator().toLatin1() + "firmware/1.x.15.0";

	connect(_infoCloseBtn, &QPushButton::clicked, this, &::DeviceCatalog::onInfoGroupCloseBtnClicked);
	connect(_infoLabelText, &QLabel::linkActivated, this, &::DeviceCatalog::onInfoGroupLinkClicked);

	// Default splitter properties (left frame expanded)
	_splitter->setSizes({1,0});
}

DeviceCatalog::~DeviceCatalog()
{
}

AlpacaDevices DeviceCatalog::enumerateDevices(const DebugBoardType type)
{
	AlpacaDevices alpacaDevices;

	_AlpacaDevice::updateAlpacaDevices();
	_AlpacaDevice::getAlpacaDevices(alpacaDevices, type);

	return alpacaDevices;
}

void DeviceCatalog::deviceSelectionDialog(const DebugBoardType type)
{
	AlpacaDevices alpacaDevices = enumerateDevices(type);
	DeviceSelectionDialog* selectionDialog = new DeviceSelectionDialog;
	selectionDialog->setDevices(alpacaDevices);

	if (selectionDialog->exec() == QDialog::Accepted)
	{
		_currentSerialNumber = selectionDialog->currentSerialNumber();
		invokeProgrammer(type);
	}
}

void DeviceCatalog::processProgramming(const DebugBoardType& type, const QPoint& pos, int row)
{
	auto configName = _deviceTable->item(row, kConfigNameCol)->data(Qt::UserRole).toString();

	AlpacaDevices alpacaDevices = enumerateDevices(type);

	QMenu menu;
	QAction* result;

	if (alpacaDevices.count() > 0)
	{
		for (const auto& device: std::as_const(alpacaDevices))
		{
			// Show the USB serial too: a board whose board ID is not set may have no name.
			QAction* programAction = menu.addAction(QString("Program Device %1 (%2) with Platform ID: %3")
				.arg(device->name().data(), QString::fromLatin1(device->serialNumber())).arg(_currentPlatformId));
			programAction->setData(device->name() + ":" + device->serialNumber());
		}

		result = menu.exec(_deviceTable->mapToGlobal(pos));
		if (result != Q_NULLPTR)
		{
			QStringList actionData = result->data().toString().split(":", Qt::SkipEmptyParts);
			QString deviceName = actionData.at(0);

			_currentSerialNumber = actionData.at(1);

			QMessageBox::StandardButton mbResult = QMessageBox::question(this, "Program the Device?",
				QString("Program Device %1 with Platform %2(%3)?").arg(deviceName, configName).arg(_currentPlatformId));

			if (mbResult == QMessageBox::Yes)
				invokeProgrammer(type);
		}
	}
	else
	{
		QAction* programAction = menu.addAction("No "+ debugBoardTypeToString(type) +" Devices Found");
		programAction->setEnabled(false);
		menu.exec(_deviceTable->mapToGlobal(pos));
	}
}

void DeviceCatalog::invokeProgrammer(const DebugBoardType type)
{
	switch(type)
	{
		case ePSOC:
			invokePSOCProgrammer(_currentSerialNumber, _currentPlatformId);
			break;
		case eFTDI:
			invokeLiteProgrammer(_currentSerialNumber, _currentPlatformId);
			break;
		case eUnknownDebugBoard:
		case eSpiderBoard:
        case ePIC32CXAuto:
			break;
	}
}

void DeviceCatalog::invokeLiteProgrammer(const QString& serialNumber, const PlatformID platformId)
{
	// Must match the target name in src/applications/programmers/lite-programmer/CMakeLists.txt;
	// Linux file names are case sensitive.
	QString program = applicationBinPath() + "LITEProgrammer";
	QStringList arguments; //-p platformid=18 serial=FT6G3Z6Y
	arguments << "-p";
	arguments << "serial=" + serialNumber;
	arguments << "platformid=" + QString::number(platformId);

	QProcess process;

	process.setProgram(program);
	process.setArguments(arguments);
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start();

	if (process.waitForStarted(10000) == false)
	{
		QMessageBox::warning(this, "Lite Device Not Programmed", QString("Could not start the Lite programmer:\n%1\n\n"
																		 "The device with serial number %2 was not changed.")
																	 .arg(QDir::toNativeSeparators(program), serialNumber));
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	bool finished = process.waitForFinished(60000);
	QApplication::restoreOverrideCursor();

	if (finished == false)
	{
		process.kill();
		process.waitForFinished(5000);
	}

	if (finished == false || process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0)
	{
		QString output = QString::fromLocal8Bit(process.readAll()).trimmed();
		if (output.isEmpty())
		{
			output = finished ? QString("Exit code %1").arg(process.exitCode()) : QString("The programmer did not finish in time.");
		}

		QMessageBox::warning(this, "Lite Device Not Programmed", QString("Programming the device with serial number %1 failed:\n\n%2")
																	 .arg(serialNumber, output));
		return;
	}

	QMessageBox::information(this, "Lite Device Programmed", QString("The device with serial number %1 needs to be"
																	 " unplugged and plugged back in.").arg(serialNumber));
}

void DeviceCatalog::invokePSOCProgrammer(const QString &serialNumber, const PlatformID platformId)
{
	QString program = applicationBinPath() + "PSOCProgrammer";

	// select= chooses the board, so it can be programmed while other boards stay connected.
	// Do not pass serial=: PSOCProgrammer writes that value into the board's EEPROM as a new
	// serial number. Only add select= when several boards are connected, so a single board is
	// programmed even if its USB serial is not reported (older PSOCProgrammer, some Linux setups).
	const int psocCount = enumerateDevices(ePSOC).count();

	QStringList arguments; //-p platformid=7 select=C90002030F162D84
	arguments << "-p";
	arguments << "platformid=" + QString::number(platformId);
	if (psocCount > 1)
		arguments << "select=" + serialNumber;

	QProcess process;
	process.setProgram(program);
	process.setArguments(arguments);
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start();

	if (process.waitForStarted(10000) == false)
	{
		QMessageBox::warning(this, "PSOC Device Not Programmed", QString("The PSOC programmer is not installed or could not be started:\n%1\n\n"
																		 "The device with serial number %2 was not changed.")
																	 .arg(QDir::toNativeSeparators(program), serialNumber));
		return;
	}

	QApplication::setOverrideCursor(Qt::WaitCursor);
	bool finished = process.waitForFinished(60000);
	QApplication::restoreOverrideCursor();

	if (finished == false)
	{
		process.kill();
		process.waitForFinished(5000);
	}

	const QString output = QString::fromLocal8Bit(process.readAll()).trimmed();
	fwLog(QString("PSOCProgrammer %1 -> finished=%2 exit=%3\n%4")
		.arg(arguments.join(' ')).arg(finished).arg(process.exitCode()).arg(output));

	// Older PSOCProgrammer builds always exit 0, so also check the output for success.
	const bool succeeded = finished && process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0 &&
		output.contains("EEPROM Updated");

	if (succeeded == false)
	{
		QString details = output;
		if (details.isEmpty())
			details = finished ? QString("Exit code %1").arg(process.exitCode()) : QString("The programmer did not finish in time.");

		if (output.contains("One, and only one device"))
			details += "\n\nThis PSOC programmer can only program one board at a time. "
					   "Disconnect the other PSOC boards and try again.";

		QMessageBox::warning(this, "PSOC Device Not Programmed", QString("Programming the device with serial number %1 failed:\n\n%2")
																	 .arg(serialNumber, details));
		return;
	}

	QMessageBox::information(this, "PSOC Device Programmed", QString("The device with serial number %1 was programmed with platform ID %2.\n\n"
																	 "Unplug it and plug it back in.").arg(serialNumber).arg(platformId));
}

void DeviceCatalog::onConfigurationLinkClicked(QTableWidgetItem* twi)
{
	QString program = applicationBinPath() + "TACConfigEditor";
	if (twi->column() == 5)
	{
		QString configPath = twi->text();
		if (configPath.compare("-", Qt::CaseInsensitive) != 0)
		{
			QStringList arguments;
			arguments << configPath;

			QProcess* process = new QProcess(Q_NULLPTR);

			process->setProgram(program);
			process->setArguments(arguments);
			process->startDetached();
		}
	}
}

void DeviceCatalog::onCustomContextMenuRequested(const QPoint& pos)
{
	if (_deviceTable != Q_NULLPTR)
	{
		QTableWidgetItem* twi = _deviceTable->itemAt(pos);
		if (twi != Q_NULLPTR)
		{
			int row = twi->row();
			if (row != -1)
			{
				auto boardType = DebugBoardType(_deviceTable->item(row, kBoardType)->data(Qt::UserRole).toUInt());
				processProgramming(boardType, pos, row);
			}
		}
	}
}

void DeviceCatalog::on__deviceTable_itemClicked(QTableWidgetItem *item)
{
	if (item != Q_NULLPTR)
	{
		_programBtn->setEnabled(true);
		int row = item->row();
		QTableWidgetItem* twi = _deviceTable->item(row, kBoardType);

		if (twi)
		{
			if (twi->text() == "PSOC")
			{
				const bool fwUpdateAvailable = (fwUpdateProgram().isEmpty() == false);

				_firmwareUpdateBtn->setEnabled(fwUpdateAvailable);
				_firmwareUpdateBtn->setToolTip(fwUpdateAvailable ? kFwUpdateToolTip : kFwUpdateMissingToolTip);
				_firmwareLabel->setEnabled(fwUpdateAvailable);
				_firmwareSelect->setEnabled(fwUpdateAvailable);
			}
			else
			{
				_firmwareUpdateBtn->setEnabled(false);
				_firmwareUpdateBtn->setToolTip(kFwUpdateToolTip);
				_firmwareLabel->setEnabled(false);
				_firmwareSelect->setEnabled(false);
			}

			_currentPlatformId = _deviceTable->item(row, kPlatformID)->data(Qt::UserRole).toUInt();
		}
	}
}

void DeviceCatalog::on__programBtn_clicked()
{
	if (_deviceTable != Q_NULLPTR)
	{
		QTableWidgetItem* twi = _deviceTable->currentItem();
		if (twi != Q_NULLPTR)
		{
			int row = twi->row();
			if (row != -1)
			{
				auto boardType = _deviceTable->item(twi->row(), kBoardType)->data(Qt::UserRole).toUInt();
				switch (boardType)
				{
				case eFTDI:
					deviceSelectionDialog(eFTDI);
					break;

				case ePSOC:
					deviceSelectionDialog(ePSOC);
					break;

				default:
					break;
				}
			}
		}
	}
}

void DeviceCatalog::on__firmwareUpdateBtn_clicked()
{
	const QString program = fwUpdateProgram();

	if (program.isEmpty())
	{
		QMessageBox::warning(this, "Firmware Update Not Available",
			QString("The FWUpdate utility was not found in:\n%1\n\n"
					"Firmware update is not included in this installation.")
				.arg(QDir::toNativeSeparators(applicationBinPath())));
		return;
	}

	const QDir firmwareDir(QString::fromLatin1(_firmwareDir));
	fwLog(QString("Update firmware requested. Firmware dir: %1, FWUpdate: %2")
		.arg(QDir::toNativeSeparators(firmwareDir.absolutePath()), QDir::toNativeSeparators(program)));

	// Let the user pick the target board, as the legacy Device Catalog did.
	AlpacaDevices psocDevices = enumerateDevices(ePSOC);
	QStringList connectedSerials;
	for (const auto& device : psocDevices)
		connectedSerials << QString::fromLatin1(device->serialNumber());

	fwLog(QString("Connected PSOC debug boards: %1 [%2]").arg(psocDevices.count()).arg(connectedSerials.join(", ")));

	if (psocDevices.isEmpty())
	{
		QMessageBox::warning(this, "Firmware Update", "No PSOC debug board is connected.");
		return;
	}

	DeviceSelectionDialog selectionDialog(this);
	selectionDialog.setDevices(psocDevices);
	if (selectionDialog.exec() != QDialog::Accepted)
	{
		fwLog("Board selection cancelled by user");
		return;
	}

	const QString selectedSerial = selectionDialog.currentSerialNumber();
	fwLog(QString("Selected board: %1").arg(selectedSerial));

	// Pick the firmware variant from the selected board's chipset.
	QString chipset{"None"};
	QString openFailure;
	bool boardFound{false};
	bool boardOpened{false};
	for (auto& device : psocDevices)
	{
		if (QString::fromLatin1(device->serialNumber()) != selectedSerial)
			continue;

		boardFound = true;
		if (device->open())
		{
			boardOpened = true;
			chipset = device->chipVersion();
			device->close();
		}
		else
		{
			openFailure = QString::fromLatin1(device->getLastError()).trimmed();
			// open() keeps the chipset it read before failing, for example when the board
			// answered but has no board ID. It is "None" when the board did not answer.
			chipset = device->chipVersion();
		}
		break;
	}

	// If the selected board is gone or cannot be opened, FWUpdate is very unlikely to
	// succeed either, and guessing a variant would program every connected board blindly.
	if (boardFound == false)
	{
		fwLog(QString("Selected board %1 is no longer connected; aborting").arg(selectedSerial));
		QMessageBox::warning(this, "Board Not Found",
			QString("Board %1 is no longer connected.\n\nReconnect it and try again.").arg(selectedSerial));
		return;
	}

	QStringList variantsToTry;
	QString nextStep;

	if (boardOpened == false)
	{
		const bool boardIdNotSet = openFailure.contains(kBoardIdNotSetText, Qt::CaseInsensitive);
		const bool oldFirmware = openFailure.contains(kOldFirmwareText, Qt::CaseInsensitive);
		const bool chipsetKnown = kVariants.contains(chipset);

		fwLog(QString("Could not open board %1: %2 (chipset %3, board ID not set %4, old firmware %5)")
			.arg(selectedSerial, openFailure.isEmpty() ? QString("no reason given") : openFailure, chipset)
			.arg(boardIdNotSet).arg(oldFirmware));

		if (openFailure.isEmpty())
			openFailure = "The board did not answer.";

		// Without an answer from the board (for example it is open in TAC, or the port
		// could not be opened) FWUpdate cannot reach it either. Stop and say why.
		if (boardIdNotSet == false && oldFirmware == false && chipsetKnown == false)
		{
			QMessageBox::warning(this, "Cannot Update Firmware",
				QString("Board %1 could not be opened, so its firmware was not changed.\n\n"
						"Reason: %2\n\n"
						"If another application (for example TAC) is using the board, close it and try again. "
						"Otherwise unplug the board, plug it back in, and try again.").arg(selectedSerial, openFailure));
			return;
		}

		if (boardIdNotSet)
			nextStep = "\n\nThe board does not have a board ID yet. To set it, right-click the board's platform "
					   "in this window, choose Program Device, then unplug the board and plug it back in.";

		if (chipsetKnown)
		{
			const auto answer = QMessageBox::question(this, "Board Needs Attention",
				QString("Board %1 could not be fully opened:\n%2\n\n"
						"It reported its chipset (%3), so Device Catalog knows which firmware it needs.\n\n"
						"Update its firmware with the %3 firmware?").arg(selectedSerial, openFailure, chipset),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

			if (answer != QMessageBox::Yes)
			{
				fwLog("User cancelled the update of a board that could not be fully opened");
				return;
			}

			variantsToTry << chipset;
			fwLog(QString("Board %1 chipset %2 read despite open failure, using that firmware variant").arg(selectedSerial, chipset));
		}
	}

	if (boardOpened == false && variantsToTry.isEmpty())
	{
		fwLog(QString("Board %1 answered but did not report its chipset; asking user for the variant").arg(selectedSerial));

		QMessageBox chooser(QMessageBox::Warning, "Choose the Firmware",
			QString("Board %1 could not be fully opened:\n%2\n\n"
					"It did not report its chipset, so Device Catalog cannot tell which firmware it needs.\n\n"
					"Choose the firmware for this board. If you are not sure, choose Try all: "
					"Device Catalog tries each firmware in turn until one works.").arg(selectedSerial, openFailure),
			QMessageBox::NoButton, this);

		QList<QPair<QAbstractButton*, QStringList>> choices;
		for (const auto& variant : kVariants)
			choices.append({chooser.addButton(variant, QMessageBox::AcceptRole), QStringList{variant}});
		choices.append({chooser.addButton("Try all", QMessageBox::AcceptRole), kVariants});
		chooser.addButton(QMessageBox::Cancel);

		chooser.exec();

		for (const auto& choice : choices)
		{
			if (chooser.clickedButton() == choice.first)
				variantsToTry = choice.second;
		}

		if (variantsToTry.isEmpty())
		{
			fwLog("User cancelled firmware variant selection");
			return;
		}

		fwLog(QString("User chose firmware variant(s): %1").arg(variantsToTry.join(", ")));
	}
	else if (boardOpened && kVariants.contains(chipset))
	{
		variantsToTry << chipset;
		fwLog(QString("Board %1 chipset %2, using that firmware variant").arg(selectedSerial, chipset));
	}
	else if (boardOpened)
	{
		// The board opened but did not report a known chipset (e.g. old firmware).
		// Trying every variant may be the only way to recover it, but only with consent.
		fwLog(QString("Board %1 chipset unknown (%2); asking user whether to try all variants")
			.arg(selectedSerial, chipset));
		const auto answer = QMessageBox::question(this, "Unknown Chipset",
			QString("Board %1 did not report a known chipset (reported: %2).\n\n"
					"Device Catalog can try each firmware variant in turn (%3) until one succeeds.\n\n"
					"Try all firmware variants?")
				.arg(selectedSerial, chipset, kVariants.join(", ")),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

		if (answer != QMessageBox::Yes)
		{
			fwLog("User declined trying all firmware variants");
			return;
		}

		variantsToTry = kVariants;
		fwLog(QString("User confirmed trying all variants: %1").arg(kVariants.join(", ")));
	}

	// Current FWUpdate takes select=<serial> and updates only that board.
	// Older builds have no per-board argument and update every connected board.
	const bool canSelect = fwUpdateSupportsSelect(program);
	fwLog(QString("FWUpdate supports select=: %1").arg(canSelect ? "yes" : "no"));

	if (canSelect == false && psocDevices.count() > 1)
	{
		fwLog("Multiple PSOC boards connected; asking user to confirm update of all boards");
		const auto answer = QMessageBox::question(this, "Several Debug Boards Connected",
			QString("%1 debug boards are connected:\n%2\n\n"
					"The firmware update tool updates EVERY connected board, not only the one you chose (%3).\n\n"
					"To update only %3, choose No, unplug the other boards, and try again.\n\n"
					"Update all connected boards now?")
				.arg(psocDevices.count()).arg(connectedSerials.join("\n"), selectedSerial),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

		if (answer != QMessageBox::Yes)
		{
			fwLog("User declined multi-board update");
			return;
		}
		fwLog("User confirmed multi-board update");
	}

	QStringList failures;
	QStringList failureDetails;
	bool firmwareFound{false};
	const QString firmwareVersion = firmwareDir.dirName();
	const QString updatedBoards = (canSelect == false && psocDevices.count() > 1)
		? QString("all %1 connected boards").arg(psocDevices.count())
		: QString("board %1").arg(selectedSerial);

	for (const auto& variant : variantsToTry)
	{
		const QString firmwarePath = firmwareDir.filePath(variant + "/MicroEpm.cyacd");

		if (QFile::exists(firmwarePath) == false)
		{
			fwLog(QString("Firmware not found: %1").arg(QDir::toNativeSeparators(firmwarePath)));
			continue;
		}

		firmwareFound = true;

		QStringList arguments{"path=" + QDir::toNativeSeparators(firmwarePath)};
		if (canSelect)
			arguments << "select=" + selectedSerial;
		fwLog(QString("Running FWUpdate %1").arg(arguments.join(' ')));

		const FwUpdateRun run = runFwUpdate(this, program, arguments,
			QString("Updating the firmware of %1 with %2 %3.\n\n"
					"This can take up to two minutes. Do not unplug the board.")
				.arg(updatedBoards, variant, firmwareVersion));

		if (run.started == false)
		{
			// Retrying with another variant cannot help when the utility does not start.
			failures << QString("The firmware update tool could not be started: %1").arg(run.errorString);
			failureDetails << QDir::toNativeSeparators(program);
			break;
		}

		fwLog(QString("FWUpdate %1: finished=%2 stoppedAtPrompt=%3 exitStatus=%4 exitCode=%5 output:\n%6")
			.arg(variant).arg(run.finished).arg(run.stoppedAtPrompt).arg(int(run.exitStatus)).arg(run.exitCode).arg(run.output));

		if (fwUpdateSucceeded(run))
		{
			fwLog(QString("Firmware update succeeded with %1").arg(variant));

			QMessageBox done(QMessageBox::Information, "Firmware Update Complete",
				QString("The firmware of %1 was updated to %2 %3.\n\n"
						"If the board does not show up in TAC, unplug it and plug it back in.%4")
					.arg(updatedBoards, variant, firmwareVersion, nextStep),
				QMessageBox::Ok, this);
			done.setDetailedText(QString("Firmware file: %1\n\n%2").arg(QDir::toNativeSeparators(firmwarePath), run.output));
			done.exec();
			return;
		}

		QString reason;

		if (run.output.contains(kFwUpdateNoMatchText, Qt::CaseInsensitive))
			reason = QString("the update tool could not find board %1; it may have been unplugged").arg(selectedSerial);
		else if (run.output.contains(kFwUpdateNoTargetsText, Qt::CaseInsensitive))
			reason = "the update tool did not find any board to update";
		else if (run.stoppedAtPrompt)
			reason = "the update tool finished without reporting success";
		else if (run.finished == false)
			reason = QString("the update did not finish within %1 seconds").arg(kFwUpdateFinishTimeoutMs / 1000);
		else if (run.exitStatus != QProcess::NormalExit)
			reason = "the update tool stopped unexpectedly";
		else
			reason = QString("the update tool reported error code %1").arg(run.exitCode);

		failures << QString("%1: %2").arg(variant, reason);
		failureDetails << QString("--- %1 (%2)\n%3").arg(variant, QDir::toNativeSeparators(firmwarePath), run.output);

		// The board state is unknown after a timeout, so do not flash it again.
		if (run.finished == false && run.stoppedAtPrompt == false)
			break;
	}

	fwLog(QString("Firmware update failed: %1").arg(failures.isEmpty() ? "no firmware file found" : failures.join(" | ")));

	if (firmwareFound == false && failures.isEmpty())
	{
		QMessageBox::warning(this, "Firmware Update Failed",
			QString("The selected firmware is not installed. No MicroEpm.cyacd file was found under:\n%1")
				.arg(QDir::toNativeSeparators(firmwareDir.absolutePath())));
		return;
	}

	QMessageBox failed(QMessageBox::Warning, "Firmware Update Failed",
		QString("The firmware of %1 was NOT updated.\n\n%2\n\n"
				"Check that the board is plugged in and that no other application (for example TAC) "
				"is using it, then try again. Choose Show Details for the update tool's output.")
			.arg(updatedBoards, failures.join("\n")),
		QMessageBox::Ok, this);
	failed.setDetailedText(failureDetails.join("\n\n"));
	failed.exec();
}

void DeviceCatalog::on__docsBtn_clicked()
{
	startLocalBrowser(docsRoot() + "/getting-started/05-Device-Catalog.html");
}

void DeviceCatalog::onInfoGroupCloseBtnClicked()
{
	if (_infoGroupBox != Q_NULLPTR)
		_infoGroupBox->hide();
}

void DeviceCatalog::onInfoGroupLinkClicked(const QString& link)
{
	// Expand the splitter to reveal the video player pane, which starts
	// collapsed to zero width in the constructor ({1,0}) since no video
	// is playing until a tutorial link is clicked.
	_splitter->setSizes({int(0.6 * width()), int(0.4 * width())});

	_player->playback(QMediaPlayer::PlayingState, link);
}

void DeviceCatalog::on__firmwareSelect_currentTextChanged(const QString &firmwareVersion)
{
	bool ok(false);
	int version = firmwareVersion.toInt(&ok);

	if (ok)
	{
		switch (version)
		{
		case 15:
			_firmwareDir = applicationDataPath().toLatin1() + QDir::separator().toLatin1() + "firmware/1.x.15.0";
			_infoLabelText->setText(kDefaultNotice);
			_infoGroupBox->hide();
			break;
		case 16:
			_firmwareDir = applicationDataPath().toLatin1() + QDir::separator().toLatin1() + "firmware/1.x.16.0";
			_infoLabelText->setText(kv16FirmwareNotice);
			_infoGroupBox->show();
			break;
		case 17:
			_firmwareDir = applicationDataPath().toLatin1() + QDir::separator().toLatin1() + "firmware/1.x.17.0";
			_infoLabelText->setText(kv17FirmwareNotice);
			_infoGroupBox->show();
			break;
		case 18:
			_firmwareDir = applicationDataPath().toLatin1() + QDir::separator().toLatin1() + "firmware/1.x.18.0";
			_infoLabelText->setText(kv18FirmwareNotice);
			_infoGroupBox->show();
			break;
		case 19:
			_firmwareDir = applicationDataPath().toLatin1() + QDir::separator().toLatin1() + "firmware/1.x.19.0";
			_infoLabelText->setText(kv19FirmwareNotice);
			_infoGroupBox->show();
			break;
		}
	}
}
