// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "I2CWidget.h"

#include "ui_I2CWidget.h"

I2CWidget::I2CWidget(QWidget *parent) :
	QWidget(parent),
	_ui(new Ui::I2CWidget)
{
	_ui->setupUi(this);
}

I2CWidget::~I2CWidget()
{
	delete _ui;
}

void I2CWidget::SetDevice(AlpacaDevice alpacaDevice)
{

}
