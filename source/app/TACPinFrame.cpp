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
#include <qtac/PlatformID.h>
#include <qtac/TACDriveThread.h>

#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSpinBox>
#include <QCheckBox>
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
    showNotice();
}

void TACPinFrame::setDevice(TACDeviceBridge* bridge)
{
    clearPins();
    _bridge = bridge;

    connect(_bridge, &TACDeviceBridge::logLine,
            this,    &TACPinFrame::onLogLine);
    connect(_bridge, &TACDeviceBridge::pinStateChanged,
            this,    &TACPinFrame::updatePinState);

    auto* dev = bridge->device().get();
    dev->buildMapping();
    Pins pins = dev->getPins();

    buildPins(pins);
}

void TACPinFrame::clearDevice()
{
    _terminalLog = nullptr;
    _bridge = nullptr;
    clearPins();
    showNotice();
}

void TACPinFrame::clearPins()
{
    _pinButtons.clear();
    _pinInverted.clear();

    QLayout* l = layout();
    while (QLayoutItem* item = l->takeAt(0))
    {
        if (QWidget* w = item->widget())
            w->deleteLater();
        delete item;
    }
}

// ---------------------------------------------------------------------------
// Pre-connection notice
// ---------------------------------------------------------------------------

void TACPinFrame::showNotice()
{
    auto* tabs = new QTabWidget(this);
    layout()->addWidget(tabs);

    // --- General tab ---
    auto* genWidget = new QWidget;
    auto* genLayout = new QVBoxLayout(genWidget);
    genLayout->setAlignment(Qt::AlignTop);

    // Notice message
    auto* noticeBox = new QGroupBox(genWidget);
    auto* noticeLayout = new QVBoxLayout(noticeBox);
    auto* noticeLabel = new QLabel(
        "Open a device to begin. The UI will <span style='color:#ff4444;'><b>not</b></span> "
        "be constructed until a device has been opened. TAC is now configuration driven."
        "<br><br>"
        "Your firmware must be at 15 or greater on a PSOC device. If your firmware is 8 or less, "
        "TAC will just crash. Firmware 9-14 will result in a dialog asking you to update your firmware.",
        noticeBox);
    noticeLabel->setWordWrap(true);
    noticeLabel->setTextFormat(Qt::RichText);
    noticeLayout->addWidget(noticeLabel);
    genLayout->addWidget(noticeBox);

    // Empty placeholder group boxes
    for (const QString& name : { "Connections", "Buttons", "Switches", "Quick Settings", "Variables" })
    {
        auto* box = new QGroupBox(name, genWidget);
        (void)new QVBoxLayout(box);
        genLayout->addWidget(box);
    }

    tabs->addTab(genWidget, "General");

    // --- Device Info tab (placeholder — all dashes until connected) ---
    auto* infoWidget = new QWidget;
    auto* infoOuter  = new QVBoxLayout(infoWidget);
    infoOuter->setAlignment(Qt::AlignTop);

    auto* infoBox  = new QGroupBox("Alpaca Device Information", infoWidget);
    auto* infoForm = new QFormLayout(infoBox);
    infoForm->setLabelAlignment(Qt::AlignRight);

    // Name row gets an extra Rename... button
    static const char* kDash = "-";
    auto addDash = [&](const QString& label) {
        auto* val = new QLabel(kDash, infoBox);
        val->setTextInteractionFlags(Qt::TextSelectableByMouse);
        infoForm->addRow(label + ":", val);
    };

    addDash("Hardware Type");
    addDash("HW Version");
    addDash("Firmware Version");
    addDash("Chipset");

    // Name row with Rename button
    {
        auto* row    = new QWidget(infoBox);
        auto* rowLay = new QHBoxLayout(row);
        rowLay->setContentsMargins(0, 0, 0, 0);
        auto* nameVal = new QLabel(kDash, row);
        nameVal->setTextInteractionFlags(Qt::TextSelectableByMouse);
        auto* renameBtn = new QPushButton("Rename...", row);
        renameBtn->setEnabled(false);
        rowLay->addWidget(nameVal);
        rowLay->addWidget(renameBtn);
        rowLay->addStretch();
        infoForm->addRow("Name:", row);
    }

    addDash("UUID");
    addDash("Serial Number");
    addDash("Platform ID");
    addDash("MAC Address");
    addDash("Configuration File");
    addDash("Configuration Date");
    addDash("Configuration File Version");

    infoOuter->addWidget(infoBox);
    tabs->addTab(infoWidget, "Device Info");
}

// ---------------------------------------------------------------------------
// Device Info tab
// ---------------------------------------------------------------------------

QWidget* TACPinFrame::buildDeviceInfoTab(QWidget* parent)
{
    auto* w      = new QWidget(parent);
    auto* outer  = new QVBoxLayout(w);
    outer->setAlignment(Qt::AlignTop);

    auto* box    = new QGroupBox("Alpaca Device Information", w);
    auto* form   = new QFormLayout(box);
    form->setLabelAlignment(Qt::AlignRight);

    auto addRow = [&](const QString& label, const QString& value) {
        auto* val = new QLabel(value, box);
        val->setTextInteractionFlags(Qt::TextSelectableByMouse);
        form->addRow(label + ":", val);
    };

    qtac::TACDriveThread* dt = _bridge->driveThread();
    if (dt)
    {
        addRow("Hardware Type",     QtAdapter::toQString(dt->debugBoardTypeString()));
        addRow("HW Version",        QtAdapter::toQString(dt->hardwareVersionString()));
        addRow("Firmware Version",  QtAdapter::toQString(dt->firmwareVersion()));
        addRow("Chipset",           QString::number(dt->chipVersion()));

        // Name row with Rename button
        {
            auto* row    = new QWidget(box);
            auto* rowLay = new QHBoxLayout(row);
            rowLay->setContentsMargins(0, 0, 0, 0);
            auto* nameVal = new QLabel(QString(dt->name().constData()), row);
            nameVal->setTextInteractionFlags(Qt::TextSelectableByMouse);
            auto* renameBtn = new QPushButton("Rename...", row);
            renameBtn->setEnabled(_bridge->supportsRename());
            rowLay->addWidget(nameVal);
            rowLay->addWidget(renameBtn);
            rowLay->addStretch();
            form->addRow("Name:", row);

            if (_bridge->supportsRename())
            {
                connect(renameBtn, &QPushButton::clicked, this, [this, nameVal]() {
                    bool ok = false;
                    QString current = nameVal->text();
                    QString newName = QInputDialog::getText(
                        this, "Rename Device", "New name (alphanumeric, max 32 chars):",
                        QLineEdit::Normal, current, &ok);
                    if (ok && !newName.isEmpty() && newName != current)
                    {
                        _bridge->renameDevice(newName);
                        nameVal->setText(newName);
                    }
                });
            }
        }

        addRow("UUID",          QtAdapter::toQString(dt->uuid()));
        addRow("Serial Number", QtAdapter::toQString(dt->serialNumber()));
        addRow("Platform ID",   QString::number(static_cast<int>(dt->platformID())));
        addRow("MAC Address",   QString(dt->macAddress().constData()));
    }

    // Config file path, date, and version from the platform registry + device.
    auto* dev = _bridge->device().get();
    PlatformIDList entries = PlatformContainer::getEntries();
    for (const auto& entry : entries)
    {
        if (dt && entry && entry->_platformID == dt->platformID())
        {
            addRow("Configuration File", QtAdapter::toQString(entry->_path));
            break;
        }
    }

    {
        QString date = QtAdapter::toQString(dev->configModificationDate());
        addRow("Configuration Date",         date.isEmpty() ? "-" : date);
        int ver = dev->configFileVersion();
        addRow("Configuration File Version", ver > 0 ? QString::number(ver) : "-");
    }

    outer->addWidget(box);
    return w;
}

// ---------------------------------------------------------------------------
// Terminal tab
// ---------------------------------------------------------------------------

QWidget* TACPinFrame::buildTerminalTab(QWidget* parent)
{
    auto* w      = new QWidget(parent);
    auto* layout = new QVBoxLayout(w);
    layout->setContentsMargins(4, 4, 4, 4);

    _terminalLog = new QPlainTextEdit(w);
    _terminalLog->setReadOnly(true);
    _terminalLog->setMaximumBlockCount(2000);
    _terminalLog->setFont(QFont("Courier New", 8));
    layout->addWidget(_terminalLog);

    return w;
}

// ---------------------------------------------------------------------------
// Quick Settings / Variables helpers
// ---------------------------------------------------------------------------

void TACPinFrame::appendQuickSettings(QVBoxLayout*                 tabLayout,
                                       QWidget*                    parent,
                                       const QString&              tabName,
                                       const qtac::ButtonEntries&  buttons,
                                       const qtac::VariableEntries& variables)
{
    // --- Quick Settings group box ---
    // Collect buttons for this tab with command_group == 4 (eQuickSettingsGroup)
    std::vector<const qtac::ButtonEntry*> tabButtons;
    for (const auto& btn : buttons)
    {
        QString btnTab = QtAdapter::toQString(btn._tab);
        if (btnTab.isEmpty()) btnTab = "General";
        if (btnTab == tabName && btn._commandGroup == 4)
            tabButtons.push_back(&btn);
    }

    if (!tabButtons.empty())
    {
        auto* box  = new QGroupBox("Quick Settings", parent);
        auto* grid = new QGridLayout(box);

        int autoRow = 0, autoCol = 0;
        for (const auto* btnEntry : tabButtons)
        {
            int row = (btnEntry->_cellY >= 0) ? btnEntry->_cellY : autoRow;
            int col = (btnEntry->_cellX >= 0) ? btnEntry->_cellX : autoCol;

            QString label = QtAdapter::toQString(btnEntry->_name);
            auto* btn = new QPushButton(label, box);
            btn->setCheckable(false);  // momentary, not toggle
            btn->setToolTip(QtAdapter::toQString(btnEntry->_tooltip));

            // Store the script command name in the object name so the slot
            // can retrieve it.
            btn->setObjectName(QtAdapter::toQString(btnEntry->_command));

            connect(btn, &QPushButton::clicked,
                    this, &TACPinFrame::onQuickButtonClicked);

            grid->addWidget(btn, row, col);

            ++autoCol;
            if (autoCol >= 4) { autoCol = 0; ++autoRow; }
        }
        tabLayout->addWidget(box);
    }

    // --- Variables group box ---
    if (!variables.isEmpty())
    {
        auto* box  = new QGroupBox("Variables", parent);
        auto* form = new QFormLayout(box);
        form->setLabelAlignment(Qt::AlignRight);

        for (const auto& kv : variables)
        {
            const qtac::VariableEntry& var = kv.second;
            QString label = QtAdapter::toQString(var._label);
            if (label.isEmpty()) label = QtAdapter::toQString(var._name);
            QString varName = QtAdapter::toQString(var._name);

            if (var._type == qtac::VariableType::Boolean)
            {
                auto* cb = new QCheckBox(box);
                cb->setChecked(var._defaultValue.toBool());
                cb->setToolTip(QtAdapter::toQString(var._tooltip));
                cb->setObjectName(varName);
                connect(cb, &QCheckBox::toggled, this, [this, varName](bool checked) {
                    if (_bridge) _bridge->setVariableValue(varName, checked);
                });
                form->addRow(label + ":", cb);
            }
            else
            {
                // Integer or Float — use a QSpinBox (floats rounded to int for now)
                auto* sb = new QSpinBox(box);
                sb->setRange(0, 30000);
                sb->setValue(static_cast<int>(var._defaultValue.toUInt()));
                sb->setToolTip(QtAdapter::toQString(var._tooltip));
                sb->setObjectName(varName);
                connect(sb, QOverload<int>::of(&QSpinBox::valueChanged),
                        this, [this, varName](int val) {
                    if (_bridge) _bridge->setVariableValue(varName, val);
                });
                form->addRow(label + ":", sb);
            }
        }
        tabLayout->addWidget(box);
    }
}

// ---------------------------------------------------------------------------
// Build pin grid
// ---------------------------------------------------------------------------

void TACPinFrame::buildPins(const Pins& pins)
{
    // Collect buttons and variables from the device's platform config.
    const qtac::ButtonEntries&   allButtons   = _bridge->device()->getButtons();
    const qtac::VariableEntries& allVariables = _bridge->device()->getVariables();

    if (pins.isEmpty() && allButtons.empty() && allVariables.isEmpty())
    {
        layout()->addWidget(new QLabel("No active pins for this device.", this));
        return;
    }

    // Collect unique tab names from pins, then build the canonical tab order:
    // "General" always first, then other tcnf-defined tabs in sorted order,
    // then fixed app tabs "Device Info" and "Terminal" at the end.
    QList<QString> dynamicTabs;
    for (const auto& pin : pins)
    {
        QString tab = QtAdapter::toQString(pin._tabName);
        if (tab.isEmpty() || tab.startsWith('<')) tab = "General";
        if (tab != "General" && tab != "Device Info" && tab != "Terminal"
            && !dynamicTabs.contains(tab))
            dynamicTabs.append(tab);
    }
    // Also collect tabs from buttons
    for (const auto& btn : allButtons)
    {
        QString tab = QtAdapter::toQString(btn._tab);
        if (tab.isEmpty() || tab.startsWith('<')) tab = "General";
        if (tab != "General" && tab != "Device Info" && tab != "Terminal"
            && !dynamicTabs.contains(tab))
            dynamicTabs.append(tab);
    }
    std::sort(dynamicTabs.begin(), dynamicTabs.end());

    QList<QString> tabOrder;
    tabOrder.append("General");
    tabOrder.append("Device Info");
    tabOrder += dynamicTabs;
    tabOrder.append("Terminal");

    auto* tabs = new QTabWidget(this);
    layout()->addWidget(tabs);

    static const CommandGroups kGroupOrder[] = {
        eConnectionGroup, eButtonGroup, eSwitchGroup, eUnknownCommandGroup
    };

    for (const QString& tabName : tabOrder)
    {
        if (tabName == "Device Info")
        {
            tabs->addTab(buildDeviceInfoTab(tabs), tabName);
            continue;
        }

        if (tabName == "Terminal")
        {
            tabs->addTab(buildTerminalTab(tabs), tabName);
            continue;
        }

        auto* tabWidget = new QWidget;
        auto* tabLayout = new QVBoxLayout(tabWidget);
        tabLayout->setAlignment(Qt::AlignTop);

        // Group pins by command group within this tab.
        QMap<CommandGroups, QList<PinEntry>> grouped;
        for (const auto& pin : pins)
        {
            QString t = QtAdapter::toQString(pin._tabName);
            if (t.isEmpty() || t.startsWith('<')) t = "General";
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

                // Logical display state accounts for inverted pins:
                // inverted pin with initial_value:false is logically "on" (e.g. battery connected).
                bool displayState = pe._inverted ? !pe._initialValue : pe._initialValue;

                auto* btn = new QPushButton(label, box);
                btn->setCheckable(true);
                btn->setChecked(displayState);
                btn->setEnabled(pe._enabled);
                btn->setToolTip(QtAdapter::toQString(pe._pinTooltip));

                // Store the hash so the slot can look it up.
                btn->setObjectName(QString::number(static_cast<quint64>(pe._hash)));

                connect(btn, &QPushButton::toggled,
                        this, &TACPinFrame::onPinButtonToggled);

                grid->addWidget(btn, row, col);
                _pinButtons.insert(static_cast<quint64>(pe._hash), btn);
                _pinInverted.insert(static_cast<quint64>(pe._hash), pe._inverted);

                if (displayState)
                    btn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; }");
            }

            tabLayout->addWidget(box);
        }

        // Append Quick Settings buttons and Variables for this tab
        appendQuickSettings(tabLayout, tabWidget, tabName, allButtons, allVariables);

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

    bool inverted = _pinInverted.value(hash, false);
    bool hwState  = inverted ? !checked : checked;
    _bridge->device()->setPinState(static_cast<PinID>(hash), hwState);
}

void TACPinFrame::onQuickButtonClicked()
{
    if (!_bridge) return;

    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QByteArray cmd = btn->objectName().toLatin1();
    _bridge->quickCommand(cmd);
}

void TACPinFrame::onLogLine(const QByteArray& line)
{
    if (!_terminalLog) return;
    if (line.trimmed().isEmpty()) return;

    _terminalLog->appendPlainText(QString(line));
    _terminalLog->verticalScrollBar()->setValue(
        _terminalLog->verticalScrollBar()->maximum());
}

void TACPinFrame::updatePinState(quint64 pin, bool state)
{
    auto it = _pinButtons.find(pin);
    if (it == _pinButtons.end()) return;

    bool inverted    = _pinInverted.value(pin, false);
    bool displayState = inverted ? !state : state;

    QPushButton* btn = it.value();
    btn->blockSignals(true);
    btn->setChecked(displayState);
    btn->setStyleSheet(displayState
        ? "QPushButton { background-color: #4CAF50; color: white; }"
        : "");
    btn->blockSignals(false);
}
