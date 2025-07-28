// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ApplicationEnhancements.h"
#include "DeviceCatalog.h"
#include "ConsoleApplicationEnhancements.h"
#include "DeviceSelectionDialog.h"
#include "USBDescriptors.h"

// Qt
#include <QDir>
#include <QMenu>
#include <QMessageBox>
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


DeviceCatalog::DeviceCatalog(QWidget *parent)
	: QDialog(parent)
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
		case eFTDI:
			invokeLiteProgrammer(_currentSerialNumber, _currentPlatformId);
			break;
		case eUnknownDebugBoard:
			break;
	}
}

void DeviceCatalog::invokeLiteProgrammer(const QString& serialNumber, const PlatformID platformId)
{
#ifdef Q_OS_LINUX
	QString program = "/opt/qcom/QTAC/bin/lite-programmer";
#else
	QString program = "lite-programmer";
#endif
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
#ifdef Q_OS_LINUX
	QString program = "/opt/qcom/QTAC/bin/psoc-programmer";
#else
	QString program = "psoc-programmer";
#endif
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

void DeviceCatalog::onConfigurationLinkClicked
(
	QTableWidgetItem* twi
)
{
#ifdef Q_OS_LINUX
	QString program = "/opt/qcom/QTAC/bin/TACConfigEditor";
#else
	QString program = "TACConfigEditor";
#endif
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

void DeviceCatalog::onCustomContextMenuRequested
(
	const QPoint& pos
)
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
				_firmwareUpdateBtn->setEnabled(true);
			else
				_firmwareUpdateBtn->setEnabled(false);

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

				default:
					break;
				}
			}
		}
	}
}


void DeviceCatalog::on__firmwareUpdateBtn_clicked()
{

	QMessageBox::critical(this, "Missing functionality - EPM", "Firmware update using Device Catalog is not currently available as EPM is not open-sourced.");

}

void DeviceCatalog::on__docsBtn_clicked()
{
	startLocalBrowser(docsRoot() + "/getting-started/08-Device-Catalog.html");
}

