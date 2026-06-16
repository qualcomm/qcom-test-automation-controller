// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "CommandButton.h"

CommandButton::CommandButton
(
	QWidget* parent
) :
	QPushButton(parent)
{
	connect(this, &QPushButton::released, this, &CommandButton::on_released);
}

void CommandButton::on_released()
{
	emit commandTriggered(_command);
}
