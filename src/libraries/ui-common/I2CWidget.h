#ifndef I2CWIDGET_H
#define I2CWIDGET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "UIGlobalLib.h"

namespace Ui {
	class I2CWidget;
}

// qcommon-console
#include "AlpacaDevice.h"

// Qt
#include <QWidget>

class UILIB_EXPORT I2CWidget :
	public QWidget
{
	Q_OBJECT

public:
	I2CWidget(QWidget *parent = Q_NULLPTR);
	~I2CWidget();

	void SetDevice(AlpacaDevice alpacaDevice);

private:
	Ui::I2CWidget*				_ui{Q_NULLPTR};
	AlpacaDevice				_alpacaDevice;
};

#endif // I2CWIDGET_H
