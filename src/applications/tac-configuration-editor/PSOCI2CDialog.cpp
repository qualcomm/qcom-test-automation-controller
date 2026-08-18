#include "PSOCI2CDialog.h"
#include "ui_PSOCI2CDialog.h"
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

PSOCI2CDialog::PSOCI2CDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::PSOCI2CDialog)
{
	ui->setupUi(this);
}

PSOCI2CDialog::~PSOCI2CDialog()
{
	delete ui;
}
