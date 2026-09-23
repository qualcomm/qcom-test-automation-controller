// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ApplicationEnhancements.h"
#include "ConsoleApplicationEnhancements.h"
#include "DeviceCatalog.h"
#include "DeviceSelectionDialog.h"
#include "USBDescriptors.h"

// Qt

#include <QDir>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QProcess>
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
const QByteArray kDefaultNotice(QByteArrayLiteral("This space is used to share notification to user"));
const QStringList kVariants{"LP030", "LP038"};


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

	// applicationDataPath() returns the "configurations" subfolder (e.g.
	// .../Alpaca/configurations/), not the app's ProgramData root - the real
	// firmware/ directory is a sibling of configurations/, not nested inside
	// it. Resolve via ".." rather than assuming applicationDataPath()'s
	// return value directly is the app root, so this keeps working correctly
	// regardless of how that shared helper's return path is formatted.
	_firmwareDir = QDir::cleanPath(applicationDataPath() + "/../firmware/1.x.15.0").toLatin1();

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
	QString program = applicationBinPath() + "LiteProgrammer";
	QStringList arguments; //-p platformid=18 serial=FT6G3Z6Y
	arguments << "-p";
	arguments << "serial=" + serialNumber;
	arguments << "platformid=" + QString::number(platformId);

	QProcess* process = new QProcess(Q_NULLPTR);

	process->setProgram(program);
	process->setArguments(arguments);
	process->startDetached();

	QThread::sleep(1);

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

	QProcess* process = new QProcess(Q_NULLPTR);

	process->setProgram(program);
	process->setArguments(arguments);
	process->startDetached();

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
				_firmwareUpdateBtn->setEnabled(true);
				_firmwareLabel->setEnabled(true);
				_firmwareSelect->setEnabled(true);
			}
			else
			{
				_firmwareUpdateBtn->setEnabled(false);
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
	if (QDir(_firmwareDir).exists() == false)
	{
		QMessageBox::critical(this, "Missing Firmware Directory",
			QString("The firmware directory is not available at the intended install location: %1").arg(_firmwareDir));
		return;
	}

	AlpacaDevices alpacaDevices = enumerateDevices(ePSOC);
	DeviceSelectionDialog* selectionDialog = new DeviceSelectionDialog(this);
	selectionDialog->setDevices(alpacaDevices);

	if (selectionDialog->exec() == QDialog::Accepted)
	{
		AlpacaDevice alpacaDevice = selectionDialog->currentDevice();

		if (alpacaDevice.isNull() == false)
		{
			// Query the actual connected chip variant rather than guessing:
			// a firmware image built for the wrong PSOC silicon is not
			// guaranteed to be safely rejected, so this must match the
			// real, physically connected device.
			// The variant is only read off the hardware inside
			// PSOCDevice::open() (which populates _chipVersion). On a device
			// that was enumerated but never opened, _chipVersion is still its
			// default 0, so chipVersion() returns "None" and every good board
			// is wrongly rejected. Open first, read the variant, then close
			// again so FWUpdate can take exclusive access to the port.
			const bool openedForVariantQuery = alpacaDevice->open();
			QString chipVariant = alpacaDevice->chipVersion();
			if (openedForVariantQuery)
				alpacaDevice->close();

			if (kVariants.contains(chipVariant) == false)
			{
				QMessageBox::critical(this, "Unknown Debug Board Chip Variant",
					QString("Could not determine a known PSOC chip variant (%1) for the selected device %2. "
						"This debug board may not have been programmed by the factory.")
						.arg(chipVariant, alpacaDevice->serialNumber()));
			}
			else
			{
				QString firmwarePath = QString(_firmwareDir) + QDir::separator() + chipVariant + QDir::separator() + "MicroEpm.cyacd";

				if (QFile::exists(firmwarePath) == false)
				{
					QMessageBox::critical(this, "Missing Firmware File",
						QString("No firmware image was found for chip variant %1 at: %2").arg(chipVariant, firmwarePath));
				}
				else
				{
					QString program = applicationBinPath() + "FWUpdate";
					QStringList arguments;
					arguments << "path=" + firmwarePath;

					QProcess* process = new QProcess(Q_NULLPTR);

					process->setProgram(program);
					process->setArguments(arguments);
					process->start();
					process->waitForFinished(30000);

					const QByteArray processOutput = process->readAllStandardOutput() + process->readAllStandardError();

					if (process->exitCode() == 0)
					{
						QMessageBox::information(this, "Firmware Update Complete",
							QString("The device has been programmed with firmware from %1.").arg(firmwarePath));
					}
					else
					{
						QMessageBox::warning(this, "Firmware Update Failed",
							QString("Unable to program the connected device with firmware from %1. Exit code: %2.\n\n%3")
								.arg(firmwarePath)
								.arg(process->exitCode())
								.arg(QString::fromLatin1(processOutput)));
					}

					process->deleteLater();
				}
			}
		}
	}

	selectionDialog->deleteLater();
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
	_player.playback(QMediaPlayer::PlayingState, link);
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
			_firmwareDir = QDir::cleanPath(applicationDataPath() + "/../firmware/1.x.15.0").toLatin1();
			_infoLabelText->setText(kDefaultNotice);
			_infoGroupBox->hide();
			break;
		case 16:
			_firmwareDir = QDir::cleanPath(applicationDataPath() + "/../firmware/1.x.16.0").toLatin1();
			_infoLabelText->setText(kv16FirmwareNotice);
			_infoGroupBox->show();
			break;
		case 17:
			_firmwareDir = QDir::cleanPath(applicationDataPath() + "/../firmware/1.x.17.0").toLatin1();
			_infoLabelText->setText(kv17FirmwareNotice);
			_infoGroupBox->show();
			break;
		case 18:
			_firmwareDir = QDir::cleanPath(applicationDataPath() + "/../firmware/1.x.18.0").toLatin1();
			_infoLabelText->setText(kv17FirmwareNotice);
			_infoGroupBox->show();
			break;
		case 19:
			_firmwareDir = QDir::cleanPath(applicationDataPath() + "/../firmware/1.x.19.0").toLatin1();
			_infoLabelText->setText(kv17FirmwareNotice);
			_infoGroupBox->show();
		}
	}
}
