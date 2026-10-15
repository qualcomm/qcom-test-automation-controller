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

#include "PreferencesDialog.h"
#include "TACPreferences.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>

PreferencesDialog::PreferencesDialog(TACPreferences* prefs, QWidget* parent)
    : QDialog(parent)
    , _prefs(prefs)
{
    setWindowTitle("Preferences");
    setModal(true);

    // --- Startup group ---
    auto* startupGroup = new QGroupBox("Startup");
    auto* startupLayout = new QVBoxLayout(startupGroup);
    _openLastDevice = new QCheckBox("Open last device on start");
    startupLayout->addWidget(_openLastDevice);

    // --- Auto-Shutdown group ---
    auto* shutdownGroup = new QGroupBox("Auto Shutdown");
    auto* shutdownForm = new QFormLayout(shutdownGroup);
    _autoShutdown = new QCheckBox("Enable auto shutdown");
    _hours = new QDoubleSpinBox;
    _hours->setRange(0.1, 120.0);
    _hours->setSingleStep(0.5);
    _hours->setSuffix(" hours");
    _hours->setValue(24.0);
    shutdownForm->addRow(_autoShutdown);
    shutdownForm->addRow("Idle time:", _hours);

    connect(_autoShutdown, &QCheckBox::toggled, _hours, &QDoubleSpinBox::setEnabled);

    // --- Buttons ---
    auto* setDefaultsBtn = new QPushButton("Set To Defaults");
    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(setDefaultsBtn, &QPushButton::clicked, this, &PreferencesDialog::onSetToDefaults);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // --- Top-level layout ---
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(startupGroup);
    mainLayout->addWidget(shutdownGroup);
    mainLayout->addWidget(setDefaultsBtn);
    mainLayout->addWidget(buttonBox);

    loadFromPrefs();
}

void PreferencesDialog::loadFromPrefs()
{
    _openLastDevice->setChecked(_prefs->openLastDevice());
    _autoShutdown->setChecked(_prefs->autoShutdown());
    _hours->setValue(_prefs->autoShutdownHours());
    _hours->setEnabled(_prefs->autoShutdown());
}

void PreferencesDialog::onSetToDefaults()
{
    _openLastDevice->setChecked(false);
    _autoShutdown->setChecked(false);
    _hours->setValue(24.0);
    _hours->setEnabled(false);
}

void PreferencesDialog::accept()
{
    _prefs->setOpenLastDevice(_openLastDevice->isChecked());
    _prefs->setAutoShutdown(_autoShutdown->isChecked());
    _prefs->setAutoShutdownHours(_hours->value());
    QDialog::accept();
}
