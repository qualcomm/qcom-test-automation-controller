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

#include "TACPinFrame.h"

#include <qt_string_convert.h>

#include <qtac/AlpacaDevice.h>
#include <qtac/CommandGroup.h>

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMap>
#include <QTabWidget>
#include <QVBoxLayout>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static QString groupBoxTitle(CommandGroups g)
{
    switch (g)
    {
    case eConnectionGroup: return "Connections";
    case eButtonGroup:     return "Buttons";
    case eSwitchGroup:     return "Switches";
    default:               return "Other";
    }
}

// ---------------------------------------------------------------------------
// TACPinFrame
// ---------------------------------------------------------------------------

TACPinFrame::TACPinFrame(QWidget* parent)
    : QWidget(parent)
{
    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
}

void TACPinFrame::setDevice(TACDeviceBridge* bridge)
{
    clearPins();
    _bridge = bridge;

    // open() already called by TACWindow before we get here.
    // buildMapping() populates the command list; getPins() returns PinEntry
    // data directly from the platform config (full label, group, cell, etc.)
    auto* dev = bridge->device().get();
    dev->buildMapping();
    Pins pins = dev->getPins();

    buildPins(pins);
}

void TACPinFrame::clearDevice()
{
    _bridge = nullptr;
    clearPins();
}

void TACPinFrame::clearPins()
{
    _pinButtons.clear();

    QLayout* l = layout();
    while (QLayoutItem* item = l->takeAt(0))
    {
        if (QWidget* w = item->widget())
            w->deleteLater();
        delete item;
    }
}

// ---------------------------------------------------------------------------
// Build pin grid
// ---------------------------------------------------------------------------

void TACPinFrame::buildPins(const Pins& pins)
{
    if (pins.isEmpty())
    {
        layout()->addWidget(new QLabel("No active pins for this device.", this));
        return;
    }

    // Collect unique tab names in order of first appearance.
    QList<QString> tabOrder;
    for (const auto& pin : pins)
    {
        QString tab = QtAdapter::toQString(pin._tabName);
        if (tab.isEmpty()) tab = "General";
        if (!tabOrder.contains(tab))
            tabOrder.append(tab);
    }

    auto* tabs = new QTabWidget(this);
    layout()->addWidget(tabs);

    static const CommandGroups kGroupOrder[] = {
        eConnectionGroup, eButtonGroup, eSwitchGroup, eUnknownCommandGroup
    };

    for (const QString& tabName : tabOrder)
    {
        auto* tabWidget = new QWidget;
        auto* tabLayout = new QVBoxLayout(tabWidget);
        tabLayout->setAlignment(Qt::AlignTop);

        // Group pins by command group within this tab.
        QMap<CommandGroups, QList<PinEntry>> grouped;
        for (const auto& pin : pins)
        {
            QString t = QtAdapter::toQString(pin._tabName);
            if (t.isEmpty()) t = "General";
            if (t == tabName)
                grouped[pin._commandGroup].append(pin);
        }

        for (CommandGroups grp : kGroupOrder)
        {
            if (!grouped.contains(grp)) continue;

            const auto& groupPins = grouped[grp];
            auto* box  = new QGroupBox(groupBoxTitle(grp), tabWidget);
            auto* grid = new QGridLayout(box);

            for (int i = 0; i < groupPins.size(); ++i)
            {
                const PinEntry& pe = groupPins[i];

                int row = (pe._cellLocation.y() >= 0) ? pe._cellLocation.y() : i / 4;
                int col = (pe._cellLocation.x() >= 0) ? pe._cellLocation.x() : i % 4;

                QString label = QtAdapter::toQString(pe._pinLabel);
                if (label.isEmpty())
                    label = QtAdapter::toQString(pe._pinCommand);
                if (label.isEmpty())
                    label = QString("Pin %1").arg(pe._pin);

                auto* btn = new QPushButton(label, box);
                btn->setCheckable(true);
                btn->setChecked(pe._initialValue);
                btn->setEnabled(pe._enabled);
                btn->setToolTip(QtAdapter::toQString(pe._pinTooltip));

                // Store the hash so the slot can look it up.
                btn->setObjectName(QString::number(static_cast<quint64>(pe._hash)));

                connect(btn, &QPushButton::toggled,
                        this, &TACPinFrame::onPinButtonToggled);

                grid->addWidget(btn, row, col);
                _pinButtons.insert(static_cast<quint64>(pe._hash), btn);

                if (pe._initialValue)
                    btn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
            }

            tabLayout->addWidget(box);
        }

        tabs->addTab(tabWidget, tabName);
    }
}

// ---------------------------------------------------------------------------

void TACPinFrame::onPinButtonToggled(bool checked)
{
    if (!_bridge) return;

    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    quint64 hash = btn->objectName().toULongLong();

    btn->setStyleSheet(checked
        ? "QPushButton { background-color: #4CAF50; color: white; }"
        : "");

    _bridge->device()->setPinState(static_cast<PinID>(hash), checked);
}

void TACPinFrame::updatePinState(quint64 pin, bool state)
{
    auto it = _pinButtons.find(pin);
    if (it == _pinButtons.end()) return;

    QPushButton* btn = it.value();
    btn->blockSignals(true);
    btn->setChecked(state);
    btn->setStyleSheet(state
        ? "QPushButton { background-color: #4CAF50; color: white; }"
        : "");
    btn->blockSignals(false);
}
