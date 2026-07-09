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

#include "TACWindow.h"
#include "ui_TACWindow.h"

#include "TACApplication.h"
#include "TACDeviceSelection.h"
#include "TACPinFrame.h"
#include "PreferencesDialog.h"

#include <qt_string_convert.h>

#include <qtac/FTDIDevice.h>
#include <qtac/AlpacaDevice.h>
#include <qtac/TACLiteDriveThread.h>
#include <qtac/TACPSOCDriveThread.h>

#include <QMessageBox>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QUrl>

#include <cstdio>
extern FILE* gCrashLog;
static inline void twCrashLog(const char* msg)
{
    if (gCrashLog) { fputs(msg, gCrashLog); fputc('\n', gCrashLog); fflush(gCrashLog); }
}

static const QString kWindowTitle = QStringLiteral("Test Automation Controller%1");

TACWindow::TACWindow(QWidget* parent)
    : QMainWindow(parent)
    , _ui(new Ui::TACWindowClass)
{
    _ui->setupUi(this);

    _pinFrame = new TACPinFrame(this);

    // Insert TACPinFrame into the scroll area's inner layout.
    if (auto* l = qobject_cast<QVBoxLayout*>(_ui->_pinScrollContents->layout()))
        l->addWidget(_pinFrame);

    connect(_ui->_connectButton,    &QPushButton::clicked, this, &TACWindow::onConnectClicked);
    connect(_ui->_disconnectButton, &QPushButton::clicked, this, &TACWindow::onDisconnectClicked);
    connect(_ui->_actionConnect,    &QAction::triggered,   this, &TACWindow::onConnectClicked);
    connect(_ui->_actionDisconnect, &QAction::triggered,   this, &TACWindow::onDisconnectClicked);
    connect(_ui->_actionQuit,       &QAction::triggered,   this, []{ TACApplication::instance()->quit(); });
    connect(_ui->_actionContents,   &QAction::triggered,   this, &TACWindow::onContentsTriggered);
    connect(_ui->_actionAbout,      &QAction::triggered,   this, &TACWindow::onAboutTriggered);
    connect(_ui->_actionPreferences,&QAction::triggered,   this, &TACWindow::onPreferencesTriggered);
    connect(&_autoShutdownTimer,    &QTimer::timeout,      this, &TACWindow::onAutoShutdownTimeout);

    setupAutoShutdownTimer();

    setWindowTitle(kWindowTitle.arg(""));
}

TACWindow::~TACWindow()
{
    shutDown();
    delete _ui;
}

// ---------------------------------------------------------------------------
// openPort
// ---------------------------------------------------------------------------

void TACWindow::openPort(const QByteArray& portName)
{
    // Ensure device list is current.
    FTDIDevice::updateAlpacaDevices();

    AlpacaDevice dev = _AlpacaDevice::findAlpacaDevice(
        qtac::ByteArray(portName.constData(), portName.size()));

    if (!dev)
    {
        _ui->_statusBar->showMessage("Device not found: " + QString(portName));
        return;
    }

    // Create and inject the concrete drive thread based on board type.
    switch (dev->debugBoardType())
    {
    case ePSOC:
        _driveThread = new qtac::TACPSOCDriveThread(dev->hash());
        break;
    case eFTDI:
    default:
        _driveThread = new qtac::TACLiteDriveThread(dev->hash());
        break;
    }
    dev->setDriveThread(_driveThread);

    // Route drive-thread log lines to our crash/diagnostic log file.
    _driveThread->onLogLine.connect([](const qtac::ByteArray& line) {
        twCrashLog(line.constData());
    });

    twCrashLog(("openPort: " + portName.toStdString()).c_str());
    twCrashLog(("  usbDescriptor: \"" + dev->usbDescriptor().toStdString() + "\"").c_str());
    twCrashLog(("  platformID: " + std::to_string(static_cast<int>(dev->platformID()))).c_str());
    twCrashLog(("  description: " + dev->description().toStdString()).c_str());

    // Create the bridge and connect its Qt signals BEFORE calling open().
    // The drive thread's run() fires onDeviceConnected (via setupConnected) immediately
    // after startRunning() — which can happen before open() even returns. If the bridge
    // is created after open(), those signals fire into the void and "Opening..." persists.
    _bridge = new TACDeviceBridge(dev, _driveThread, this);

    connect(_bridge, &TACDeviceBridge::deviceConnected,
            this,    &TACWindow::onDeviceConnected);
    connect(_bridge, &TACDeviceBridge::deviceDisconnected,
            this,    &TACWindow::onDeviceDisconnected);
    connect(_bridge, &TACDeviceBridge::firmwareVersionUpdated,
            this,    &TACWindow::onFirmwareVersionUpdated);
    connect(_bridge, &TACDeviceBridge::hardwareTypeUpdated,
            this,    &TACWindow::onHardwareTypeUpdated);
    connect(_bridge, &TACDeviceBridge::nameUpdated,
            this,    &TACWindow::onNameUpdated);
    connect(_bridge, &TACDeviceBridge::pinStateChanged,
            this,    &TACWindow::onPinStateChanged);
    connect(_bridge, &TACDeviceBridge::errorEvent,
            this,    &TACWindow::onError);

    if (!dev->open())
    {
        twCrashLog(("open() failed: " + dev->getLastError().toStdString()).c_str());
        _ui->_statusBar->showMessage("Failed to open: " + QString(portName)
            + " — " + QString::fromStdString(dev->getLastError().toStdString()));
        dev->setDriveThread(nullptr);  // clear dangling pointer before deleting
        delete _driveThread;
        _driveThread = nullptr;
        delete _bridge;
        _bridge = nullptr;
        return;
    }

    // Populate the pin panel.
    _pinFrame->setDevice(_bridge);

    // Remember this port for "open last device" feature.
    _prefs.setLastDevice(QString(portName));

    setWindowTitle(kWindowTitle.arg(" — " + QString(portName)));
    _ui->_deviceStatusLabel->setText("Opening…");
    _ui->_connectButton->setEnabled(false);
    _ui->_disconnectButton->setEnabled(true);
}

QByteArray TACWindow::portName() const
{
    if (_bridge)
        return QtAdapter::toQByteArray(_bridge->device()->portName());
    return {};
}

// ---------------------------------------------------------------------------
// shutDown
// ---------------------------------------------------------------------------

void TACWindow::shutDown()
{
    if (!_bridge)
        return;

    _pinFrame->clearDevice();

    _bridge->device()->close();

    // Shut down and delete the drive thread.
    if (_driveThread)
    {
        _driveThread->shutDown();
        delete _driveThread;
        _driveThread = nullptr;
    }

    delete _bridge;
    _bridge = nullptr;

    _ui->_deviceStatusLabel->setText("Disconnected");
    _ui->_connectButton->setEnabled(true);
    _ui->_disconnectButton->setEnabled(false);
    setWindowTitle(kWindowTitle.arg(""));

    TACApplication::disconnectTACWindow(this);
}

// ---------------------------------------------------------------------------
// Slots
// ---------------------------------------------------------------------------

void TACWindow::onConnectClicked()
{
    TACDeviceSelection dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    QByteArray port = dlg.selectedPortName();
    if (port.isEmpty())
        return;

    if (TACApplication::isPortInUse(port))
    {
        QMessageBox::critical(this, "Port In Use",
            "Port " + QString(port) + " is already open in another window.");
        return;
    }

    if (inUse())
        TACApplication::createTACWindow()->openPort(port);
    else
        openPort(port);
}

void TACWindow::onDisconnectClicked()
{
    shutDown();
    deleteLater();
}

void TACWindow::onDeviceConnected()
{
    _ui->_deviceStatusLabel->setText("Connected");
    _ui->_statusBar->showMessage("Device connected.");
    // Rebuild pin panel now that device is fully initialised.
    _pinFrame->setDevice(_bridge);
    _autoShutdownDeadline.restart();
}

void TACWindow::onDeviceDisconnected()
{
    _ui->_deviceStatusLabel->setText("Disconnected");
    _ui->_statusBar->showMessage("Device disconnected.");
}

void TACWindow::onFirmwareVersionUpdated(const QString& version)
{
    _ui->_statusBar->showMessage("FW: " + version);
}

void TACWindow::onHardwareTypeUpdated(const QString& hwType)
{
    _ui->_statusBar->showMessage("HW: " + hwType);
}

void TACWindow::onNameUpdated(const QString& name)
{
    setWindowTitle(kWindowTitle.arg(" — " + name));
}

void TACWindow::onPinStateChanged(quint64 pin, bool state)
{
    _pinFrame->updatePinState(pin, state);
    _autoShutdownDeadline.restart();
}

void TACWindow::onError(const QByteArray& message)
{
    _ui->_statusBar->showMessage("Error: " + QString(message));
}

void TACWindow::onContentsTriggered()
{
    QDesktopServices::openUrl(QUrl("https://confluence.qualcomm.com/confluence/display/QTAC"));
}

void TACWindow::onAboutTriggered()
{
    QMessageBox::about(this,
        "About Test Automation Controller",
        "<b>Test Automation Controller</b><br>"
        "Qt-free refactor build<br><br>"
        "Copyright &copy; Qualcomm Technologies, Inc. and/or its subsidiaries.");
}

void TACWindow::onPreferencesTriggered()
{
    PreferencesDialog dlg(&_prefs, this);
    if (dlg.exec() == QDialog::Accepted)
        setupAutoShutdownTimer();
}

void TACWindow::setupAutoShutdownTimer()
{
    if (_prefs.autoShutdown())
    {
        _autoShutdownTimer.start(60000);   // check every 60s
        _autoShutdownDeadline.restart();
    }
    else
    {
        _autoShutdownTimer.stop();
    }
}

void TACWindow::onAutoShutdownTimeout()
{
    qint64 limitMs = static_cast<qint64>(_prefs.autoShutdownHours() * 3600000.0);
    if (_autoShutdownDeadline.elapsed() > limitMs)
    {
        shutDown();
        deleteLater();
    }
}
