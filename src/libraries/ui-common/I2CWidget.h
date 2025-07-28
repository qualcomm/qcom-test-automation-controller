#ifndef I2CWIDGET_H
#define I2CWIDGET_H
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
// Copyright 2022-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

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
