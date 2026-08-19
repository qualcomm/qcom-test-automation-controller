// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PSOCI2CDIALOG_H
#define PSOCI2CDIALOG_H

#include "ui_PSOCI2CDialog.h"
#include "PSOCPlatformConfiguration.h"

// Qt
#include <QDialog>

class PSOCI2CDialog : public QDialog, Ui::PSOCI2CDialog
{
	Q_OBJECT

public:
	explicit PSOCI2CDialog(_PSOCPlatformConfiguration* psocConfig, QWidget *parent = nullptr);
	~PSOCI2CDialog();

private slots:
	void on__addSlaveBtn_clicked();
	void on__removeSlaveBtn_clicked();

private:
	void updateButtonStates();
	void updateSlaveFromRow(int row);
	void validateAddresses();
	QWidget* addSlaveRow(int row, const PSOCI2CSlave &slave);

	_PSOCPlatformConfiguration*	_psocConfig{Q_NULLPTR};
	QList<QWidget*>				_slaveRows;
};

#endif // PSOCI2CDIALOG_H
