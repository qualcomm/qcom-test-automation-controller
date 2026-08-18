// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef PSOCI2CDIALOG_H
#define PSOCI2CDIALOG_H

#include <QDialog>

namespace Ui { class PSOCI2CDialog; }

class PSOCI2CDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PSOCI2CDialog(QWidget *parent = nullptr);
	~PSOCI2CDialog();

private:
	Ui::PSOCI2CDialog *ui;
};

#endif // PSOCI2CDIALOG_H
