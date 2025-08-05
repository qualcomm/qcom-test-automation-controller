#ifndef USERWIDGET_H
#define USERWIDGET_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: msimpson

#include "UIGlobalLib.h"

#include "ui_UserWidget.h"

// QWidget
#include <QWidget>

class UILIB_EXPORT UserWidget :
	public QWidget,
	public Ui::UserWidget
{
	Q_OBJECT

public:
	explicit UserWidget(QWidget* parent = Q_NULLPTR);
	~UserWidget();
};

#endif // GENERALWIDGET_H
