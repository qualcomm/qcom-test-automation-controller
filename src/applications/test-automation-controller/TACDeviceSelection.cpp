/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
