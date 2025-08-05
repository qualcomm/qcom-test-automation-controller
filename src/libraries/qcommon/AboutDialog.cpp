/*
	Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries. 
	 
	Redistribution and use in source and binary forms, with or without
	modification, are permitted (subject to the limitations in the
	disclaimer below) provided that the following conditions are met:
	 
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
	 
		* Redistributions in binary form must reproduce the above
		  copyright notice, this list of conditions and the following
		  disclaimer in the documentation and/or other materials provided
		  with the distribution.
	 
		* Neither the name of Qualcomm Technologies, Inc. nor the names of its
		  contributors may be used to endorse or promote products derived
		  from this software without specific prior written permission.
	 
	NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
	GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
	HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
