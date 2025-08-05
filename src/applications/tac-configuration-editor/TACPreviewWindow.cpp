// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "TACPreviewWindow.h"

#include <QResizeEvent>

TACPreviewWindow::TACPreviewWindow
(
	PlatformConfiguration platformConfiguration,
	QWidget* parent
) :
	  QWidget(parent),
	  _platformConfig(platformConfiguration)
{
    setupUi(this);

    _tacFrame->setPlatformConfiguration(_platformConfig);

	resize(_platformConfig->getFormDimension());
}

TACPreviewWindow::~TACPreviewWindow()
{

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
