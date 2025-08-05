// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
