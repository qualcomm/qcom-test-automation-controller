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
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QProcess>
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
			QAction* programAction = menu.addAction(QString("Program Device %1 with Platform ID: %2").arg(device->name().data()).arg(_currentPlatformId));
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
	QStringList arguments; //-p platformid=18 serial=FT6G3Z6Y
	arguments << "-p";
	arguments << "serial=" + serialNumber;
	arguments << "platformid=" + QString::number(platformId);

	if (QProcess::startDetached(program, arguments) == false)
	{
		QMessageBox::warning(this, "PSOC Device Not Programmed", QString("The PSOC programmer is not installed or could not be started:\n%1\n\n"
																		 "The device with serial number %2 was not changed.")
																	 .arg(QDir::toNativeSeparators(program), serialNumber));
		return;
	}

	QThread::sleep(1);

	QMessageBox::information(this, "PSOC Device Programmed", QString("The device with serial number %1 needs to be"
																	 " unplugged and plugged back in.").arg(serialNumber));
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
	for (auto& device : psocDevices)
	{
		if (QString::fromLatin1(device->serialNumber()) != selectedSerial)
			continue;

		if (device->open())
		{
			chipset = device->chipVersion();
			device->close();
		}
		else
		{
			fwLog(QString("Could not open board %1 to read chipset").arg(selectedSerial));
		}
		break;
	}

	QStringList variantsToTry;
	if (kVariants.contains(chipset))
	{
		variantsToTry << chipset;
		fwLog(QString("Board %1 chipset %2, using that firmware variant").arg(selectedSerial, chipset));
	}
	else
	{
		variantsToTry = kVariants;
		fwLog(QString("Board %1 chipset unknown (%2), falling back to trying all variants: %3")
			.arg(selectedSerial, chipset, kVariants.join(", ")));
	}

	// FWUpdate has no per-board argument and updates every connected PSOC board.
	if (psocDevices.count() > 1)
	{
		fwLog("Multiple PSOC boards connected; asking user to confirm update of all boards");
		const auto answer = QMessageBox::question(this, "Multiple Debug Boards Connected",
			QString("%1 PSOC debug boards are connected:\n%2\n\n"
					"FWUpdate programs ALL connected PSOC boards, not only the selected one (%3).\n"
					"Disconnect the other boards to update only the selected one.\n\n"
					"Continue and update all connected boards?")
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
	bool firmwareFound{false};

	for (const auto& variant : variantsToTry)
	{
		const QString firmwarePath = firmwareDir.filePath(variant + "/MicroEpm.cyacd");

		if (QFile::exists(firmwarePath) == false)
		{
			fwLog(QString("Firmware not found: %1").arg(QDir::toNativeSeparators(firmwarePath)));
			continue;
		}

		firmwareFound = true;
		fwLog(QString("Running FWUpdate path=%1").arg(QDir::toNativeSeparators(firmwarePath)));

		QProcess process;

		process.setProgram(program);
		process.setArguments({"path=" + QDir::toNativeSeparators(firmwarePath)});
		process.setProcessChannelMode(QProcess::MergedChannels);
		process.start();

		if (process.waitForStarted(kFwUpdateStartTimeoutMs) == false)
		{
			// Retrying with another variant cannot help when the utility does not start.
			failures << QString("Could not start %1: %2")
							.arg(QDir::toNativeSeparators(program), process.errorString());
			break;
		}

		QApplication::setOverrideCursor(Qt::WaitCursor);
		const bool finished = process.waitForFinished(kFwUpdateFinishTimeoutMs);
		QApplication::restoreOverrideCursor();

		if (finished == false)
		{
			process.kill();
			process.waitForFinished(5000);
		}

		const QString output = QString::fromLocal8Bit(process.readAll()).trimmed();

		fwLog(QString("FWUpdate %1: finished=%2 exitStatus=%3 exitCode=%4 output:\n%5")
			.arg(variant).arg(finished).arg(int(process.exitStatus())).arg(process.exitCode()).arg(output));

		if (finished && process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0)
		{
			fwLog(QString("Firmware update succeeded with %1").arg(variant));
			QMessageBox::information(this, "Firmware Update Complete",
				QString("The device has been programmed with firmware from %1.")
					.arg(QDir::toNativeSeparators(firmwarePath)));
			return;
		}

		QString reason;

		if (finished == false)
			reason = QString("did not finish within %1 seconds").arg(kFwUpdateFinishTimeoutMs / 1000);
		else if (process.exitStatus() != QProcess::NormalExit)
			reason = "FWUpdate stopped unexpectedly";
		else
			reason = QString("exit code %1").arg(process.exitCode());

		failures << QString("%1 (%2)%3").arg(variant, reason, output.isEmpty() ? QString() : ":\n" + output);

		// The board state is unknown after a timeout, so do not flash it again.
		if (finished == false)
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

	QMessageBox::warning(this, "Firmware Update Failed",
		QString("Unable to program the connected device with the selected firmware version.\n\n%1\n\n"
				"Confirm that a PSOC debug board is connected and that no other application "
				"(such as TAC) has it open, then try again.")
			.arg(failures.join("\n\n")));
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
