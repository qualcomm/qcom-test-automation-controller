// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "TACPreviewWindow.h"

#include <QResizeEvent>

TACPreviewWindow::TACPreviewWindow(QWidget* parent) : QWidget(parent)
{
    setupUi(this);

	connect(_tacFrame, &TACFrame::startNotification, this, &TACPreviewWindow::onNotificationStarted);
}

TACPreviewWindow::~TACPreviewWindow()
{
}

void TACPreviewWindow::setPlatformConfiguration(PlatformConfiguration platformConfiguration)
{
	_platformConfig = platformConfiguration;

	_tacFrame->setPlatformConfiguration(_platformConfig);
	resize(_platformConfig->getFormDimension());
}

void TACPreviewWindow::resizeEvent
(
	QResizeEvent* event
)
{
	QSize newSize = event->size();
	if (newSize.width() > kClassicDimension.width() || newSize.height() > kClassicDimension.height())
	{
		// we are going to pin this to larger than the classic dimension
		_platformConfig->setFormDimension(size());
	}
}

void TACPreviewWindow::onNotificationStarted(const QString &message, NotificationLevel level)
{
	emit startNotification(message, level);
}
