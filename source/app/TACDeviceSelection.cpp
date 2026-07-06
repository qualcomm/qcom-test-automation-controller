// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "TACDeviceSelection.h"
#include "ui_TACDeviceSelection.h"

#include <qt_string_convert.h>

#include <qtac/FTDIDevice.h>
#include <qtac/AlpacaDevice.h>

#include <QPushButton>
#include <QTableWidgetItem>
#include <QTimer>

TACDeviceSelection::TACDeviceSelection(QWidget* parent)
    : QDialog(parent)
    , _ui(new Ui::TACDeviceSelectionClass)
{
    _ui->setupUi(this);

    _ui->_tacDevices->horizontalHeader()->resizeSection(1, 220);
    _ui->_tacDevices->horizontalHeader()->resizeSection(2, 220);

    setOkEnabled(false);

    connect(_ui->_tacDevices, &QTableWidget::clicked,
            this,             &TACDeviceSelection::onTableClicked);
    connect(_ui->_tacDevices, &QTableWidget::doubleClicked,
            this,             &TACDeviceSelection::onTableDoubleClicked);

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &TACDeviceSelection::refreshDevices);
    _timer->start(50); // fast first tick, then slows to 2500 ms after first refresh
}

TACDeviceSelection::~TACDeviceSelection()
{
    _timer->stop();
    delete _ui;
}

void TACDeviceSelection::onTableClicked(const QModelIndex& index)
{
    QTableWidgetItem* item = _ui->_tacDevices->item(index.row(), 0);
    if (item)
    {
        _selectedPort = item->text().toLatin1();
        setOkEnabled(true);
    }
    else
    {
        _selectedPort.clear();
        setOkEnabled(false);
    }
}

void TACDeviceSelection::onTableDoubleClicked(const QModelIndex& index)
{
    onTableClicked(index);
    if (!_selectedPort.isEmpty())
        accept();
}

void TACDeviceSelection::refreshDevices()
{
    // Remember selection so we can restore it after refresh.
    QString previousPort;
    QList<QTableWidgetItem*> sel = _ui->_tacDevices->selectedItems();
    if (!sel.isEmpty())
        previousPort = sel.first()->text();

    _ui->_tacDevices->blockSignals(true);

    FTDIDevice::updateAlpacaDevices();

    AlpacaDevices devices;
    _AlpacaDevice::getAlpacaDevices(devices);

    _ui->_tacDevices->clearContents();
    _ui->_tacDevices->setRowCount(static_cast<int>(devices.size()));

    for (int i = 0; i < static_cast<int>(devices.size()); ++i)
    {
        const auto& dev = devices[i];

        auto makeItem = [](const QString& text) {
            auto* item = new QTableWidgetItem(text);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            return item;
        };

        QString port = QtAdapter::toQString(dev->portName());
        _ui->_tacDevices->setItem(i, 0, makeItem(port));
        _ui->_tacDevices->setItem(i, 1, makeItem(QtAdapter::toQString(dev->description())));
        _ui->_tacDevices->setItem(i, 2, makeItem(QtAdapter::toQString(dev->serialNumber())));

        if (port == previousPort)
            _ui->_tacDevices->selectRow(i);
    }

    _ui->_tacDevices->blockSignals(false);

    // Slow down after first refresh.
    _timer->setInterval(2500);
}

void TACDeviceSelection::setOkEnabled(bool enabled)
{
    if (auto* btn = _ui->_buttonBox->button(QDialogButtonBox::Ok))
        btn->setEnabled(enabled);
}
