// Confidential and Proprietary – Qualcomm Technologies, Inc.

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
// Copyright ©2018-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: Michael Simpson msimpson@qti.qualcomm.com

#include "TACDeviceSelection.h"
#include "AlpacaDevice.h"

// QCommon
#include "Range.h"

// Qt
#include <QPushButton>
#include <QSet>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTimer>

QSet<QByteArray> gExistingDevices;

TACDeviceSelection::TACDeviceSelection
(
	QWidget* parent
) :
	QDialog(parent)
{
	setupUi(this);

	gExistingDevices.clear();

	enableOk(false);

	_tacDevices->horizontalHeader()->resizeSection(1, 220);
	_tacDevices->horizontalHeader()->resizeSection(2, 220);

	QHeaderView* headerView = _tacDevices->horizontalHeader();
	if (headerView != Q_NULLPTR)
	{
		headerView->setFrameStyle(QFrame::Raised);
		headerView->setFrameShape(QFrame::StyledPanel);
	}

	_timer = new QTimer;
	if (_timer != Q_NULLPTR)
	{
		connect(_timer, &QTimer::timeout, this, &TACDeviceSelection::timer_timeout);
		_timer->start(50);
	}
}

TACDeviceSelection::~TACDeviceSelection()
{
	if (_timer != Q_NULLPTR)
	{
		_timer->stop();
		_timer->deleteLater();
		_timer = Q_NULLPTR;
	}
}

QByteArray TACDeviceSelection::getSelectedPortName()
{
	return _selectedPortName;
}

void TACDeviceSelection::on__tacDevices_clicked
(
	const QModelIndex& index
)
{
	QTableWidgetItem* twi;

	twi = _tacDevices->item(index.row(), 0);

	if (twi != Q_NULLPTR)
	{
		_selectedPortName = twi->text().toLatin1();
		enableOk();
	}
	else
	{
		_selectedPortName.clear();
		enableOk(false);
	}
}

void TACDeviceSelection::on__tacDevices_doubleClicked
(
	const QModelIndex& index
)
{
	on__tacDevices_clicked(index);

	accept();
}

void TACDeviceSelection::timer_timeout()
{
	QString selectedPort;

	_tacDevices->blockSignals(true);
	QList<QTableWidgetItem*> selectedItems = _tacDevices->selectedItems();
	if (selectedItems.count())
		selectedPort = selectedItems.at(0)->text();

	_AlpacaDevice::updateAlpacaDevices();

	AlpacaDevices alpacaDevices;

	_AlpacaDevice::getAlpacaDevices(alpacaDevices);

	quint32 deviceCount = alpacaDevices.count();

	_tacDevices->clearContents();

	_tacDevices->setRowCount(deviceCount);
	for (auto deviceIndex : range(deviceCount))
	{
		QTableWidgetItem* twi;

		AlpacaDevice alpacaDevice = alpacaDevices.at(deviceIndex);

		// Portname
		twi = new QTableWidgetItem(QString(alpacaDevice->portName()));
		twi->setData(Qt::UserRole, alpacaDevice->hash());
		_tacDevices->setItem(deviceIndex, 0, twi);

		// Description
		twi = new QTableWidgetItem(QString(alpacaDevice->description()));
		_tacDevices->setItem(deviceIndex, 1, twi);

		// Serial Number
		twi = new QTableWidgetItem(QString(alpacaDevice->serialNumber()));
		_tacDevices->setItem(deviceIndex, 2, twi);

		if (alpacaDevice->portName() == selectedPort)
			_tacDevices->selectRow(deviceIndex);
	}

	_tacDevices->blockSignals(false);

	_timer->setInterval(2500);
}

void TACDeviceSelection::enableOk
(
	bool state
)
{
	QPushButton* okayButton = _buttonBox->button(QDialogButtonBox::Ok);
	if (okayButton != Q_NULLPTR)
	{
		okayButton->setEnabled(state);
	}
}
