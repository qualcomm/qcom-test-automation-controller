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
// Copyright ©2022 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

// Author: msimpson

#include "UserWidget.h"

UserWidget::UserWidget(QWidget *parent) :
	QWidget(parent)
{
	setupUi(this);

	QGridLayout* gridLayout;

	gridLayout = new QGridLayout(_connectionsGroupBox);
	gridLayout->setObjectName(QString::fromUtf8("ConnectionsLayout"));

	gridLayout = new QGridLayout(_buttonsGroupBox);
	gridLayout->setObjectName(QString::fromUtf8("ButtonsLayout"));

	gridLayout = new QGridLayout(_switchesGroupBox);
	gridLayout->setObjectName(QString::fromUtf8("SwitchesLayout"));

	gridLayout = new QGridLayout(_quickSettingsGroupBox);
	gridLayout->setObjectName(QString::fromUtf8("QuickSettingsLayout"));

	setProperty("usertab", true);
}

UserWidget::~UserWidget()
{
}
