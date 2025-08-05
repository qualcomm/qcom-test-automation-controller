// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// UI
#include "ui_AboutDialog.h"
#include "AboutDialog.h"

// qcommon-console
#include "AlpacaDefines.h"
#include "AppCore.h"
#include "StringProof.h"

// QT
#include <QDateTime>
#include <QKeyEvent>
#include <QMessageBox>

AboutDialog::AboutDialog
(
	QWidget* parent
) :
	QDialog(parent),
	_ui(new Ui::AboutDialogClass)
{
	setWindowFlag(Qt::WindowContextHelpButtonHint, false);

	_ui->setupUi(this);
}

void AboutDialog::setTitle
(
	const QByteArray& title
)
{
	QByteArray workingCopy{title};

	AppCore* appCore = AppCore::getAppCore();

	_ui->_title->setText(workingCopy);
}

void AboutDialog::setAppName
(
	const QString& appName
)
{
	QString appTitle = windowTitle().arg(appName);

	AppCore* appCore = AppCore::getAppCore();

	setWindowTitle(appTitle);
}

void AboutDialog::setAppVersion
(
	const QByteArray& appVersion
)
{
	_ui->_versionLabel->setText("Product Version - " + kProductVersion + "\nApplication Version - " + appVersion + "\nBuild Time: - " + kBuildTime);
}

void AboutDialog::setAboutText
(
	const QByteArray& aboutText
)
{
	QByteArray workingCopy{aboutText};

	AppCore* appCore = AppCore::getAppCore();

	_ui->_aboutText->setHtml(workingCopy);
}

void AboutDialog::setBackSplash
(
	const QPixmap& backSplash
)
{
	_ui->_backsplash->setPixmap(backSplash);
}

void AboutDialog::on__okayButton_clicked()
{
	accept();
}

void AboutDialog::on__aboutText_anchorClicked
(
	const QUrl& link
)
{
	Q_UNUSED(link)
}

void AboutDialog::changeEvent
(
	QEvent* e
)
{
	QDialog::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		_ui->retranslateUi(this);
		break;

	default:
		break;
	}
}

void AboutDialog::mouseReleaseEvent
(
	QMouseEvent* mouseReleaseEvent
)
{
	Q_UNUSED(mouseReleaseEvent);

	accept();
}


void AboutDialog::on__checkDateButton_clicked()
{
	QString message;

	message = "Build Date: " + kBuildTime;
	message += "\nCurrent Date: " + QDateTime::currentDateTime().toString();
	message += "\nApp Stale Date: " + QDateTime::currentDateTime().addDays(90).toString();

	 QMessageBox::information(this, "Check Date Staleness", message);
}
