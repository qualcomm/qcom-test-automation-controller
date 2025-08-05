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
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_NotificationWidget.h"
#include "NotificationWidget.h"
#include "ColorConversion.h"

// Qt
#include <QLabel>
#include <QStatusBar>

const QSize kNotificationIconSize(10,10);
const quint32 kNoticeTime(100);
const QString kProgressStyle("QProgressBar::chunk {background-color: %1; width: 1px;}");
const QByteArray kOperationMsg("Operation in progress...");

NotificationWidget::NotificationWidget(QWidget *parent)
	: QWidget{parent},
	_ui(new Ui::NotificationWidgetClass)
{
	_ui->setupUi(this);

	_notificationWindow = new HoverAwareQWindow(this);

	QIcon notificationIcon;
	notificationIcon.addFile(QString::fromUtf8(":/NotificationBellSilent.png"), kNotificationIconSize, QIcon::Normal, QIcon::Off);
	_ui->_notificationBtn->setIcon(notificationIcon);

	_ui->_progressBar->hide();

	connect(this, &NotificationWidget::notificationAdded, this, &NotificationWidget::onNotificationAdded);
	connect(this, &NotificationWidget::progress, this, &NotificationWidget::onProgressUpdated);
	connect(_ui->_notificationBtn, &QPushButton::clicked, this, &NotificationWidget::onNotificationButtonClicked);
	connect(&_timer, &QTimer::timeout, this, &NotificationWidget::onTimerTimeout);

	_timer.setTimerType(Qt::PreciseTimer);
	_timer.setInterval(kNoticeTime);
	_timer.setSingleShot(true);
}

NotificationWidget::~NotificationWidget()
{
	if (_notificationWindow != Q_NULLPTR)
		delete _notificationWindow;

	if (_ui != Q_NULLPTR)
		delete _ui;
}

void NotificationWidget::setSilent(bool status)
{
	_silent = status;
}

bool NotificationWidget::isSilent()
{
	return _silent;
}

void NotificationWidget::insertNotification(const QString &message, const NotificationLevel notificationLevel)
{
	if (_notificationWindow != Q_NULLPTR)
		_notificationWindow->insertNotification(message, notificationLevel);

	QIcon notificationIcon;
	notificationIcon.addFile(QString::fromUtf8(":/NotificationBellRing.png"), kNotificationIconSize, QIcon::Normal, QIcon::Off);
	_ui->_notificationBtn->setIcon(notificationIcon);

	emit notificationAdded(message, notificationLevel);
}

void NotificationWidget::onNotificationAdded(const QString &message, const NotificationLevel notificationLevel)
{
	makeNotificationLabel(message, notificationLevel);
	_ui->_notificationBtn->setToolTip("Click to see notifications");
	_ui->_notificationBtn->setEnabled(true);
}

void NotificationWidget::onNotificationCleared()
{
	QIcon notificationIcon;
	notificationIcon.addFile(QString::fromUtf8(":/NotificationBellSilent.png"), kNotificationIconSize, QIcon::Normal, QIcon::Off);

	_ui->_notificationBtn->setIcon(notificationIcon);
	_ui->_notificationBtn->setToolTip("You do not have new notifications");
	_ui->_notificationBtn->setEnabled(false);
}

void NotificationWidget::onNotificationButtonClicked()
{
	QWidget* wgt = qobject_cast<QWidget*>(sender()->parent()->parent()->parent());

	if (wgt != Q_NULLPTR)
	{
		_notificationWindow->setWindowLocation(wgt->size(), wgt->pos());
		connect(_notificationWindow, &HoverAwareQWindow::clearAll, this, &NotificationWidget::onNotificationCleared);
	}
}

void NotificationWidget::onProgressUpdated(const quint8 newValue, NotificationLevel level)
{
	QColor barColor =  QColor(53, 161, 84);

	QString style = kProgressStyle.arg(barColor.name());
	_ui->_progressBar->setStyleSheet(style);

	if (newValue != kProgressActive)
	{
		_ui->_progressBar->setRange(kProgressActive, kProgressMax);

		switch (level)
		{
		case eInfoNotification:
		{
			quint8 originalValue = _ui->_progressBar->value();

			_ui->_progressBar->setValue(originalValue + newValue);
			break;
		}
		case eErrorNotification:
		{
			barColor = QColor(201, 60, 65);

			QString style = kProgressStyle.arg(barColor.name());
			_ui->_progressBar->setStyleSheet(style);

			_ui->_progressBar->setValue(kProgressMax);
			break;
		}
		case eDebugNotification:
		case eWarnNotification:
			break;
		}

		_timer.start();
	}
	else
	{
		// In case of indeterminate progress...
		_ui->_progressBar->setRange(kProgressActive, kProgressActive);
		_ui->_progressBar->setValue(kProgressActive);
	}	

	QStatusBar* statusbar = qobject_cast<QStatusBar*>(parent());
	if (statusbar != Q_NULLPTR)
		statusbar->showMessage(kOperationMsg);

	_ui->_progressBar->show();
}

void NotificationWidget::onTimerTimeout()
{
	_ui->_progressBar->hide();
	_ui->_progressBar->setValue(0);

	QStatusBar* statusbar = qobject_cast<QStatusBar*>(parent());

	if (statusbar != Q_NULLPTR)
		statusbar->clearMessage();
}

void NotificationWidget::makeNotificationLabel(const QString& message, const NotificationLevel notificationLevel)
{
	QLabel* popup = new QLabel(this);
	popup->setWindowFlags(Qt::Popup);
	popup->setText(message);

	QPalette qPalette = popup->palette();
	QColor labelColor = ColorConversion::getLabelColor(notificationLevel);
	qPalette.setBrush(QPalette::Window, labelColor);
	popup->setPalette(qPalette);

	popup->setFrameStyle(QLabel::Raised | QLabel::Panel);
	popup->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	popup->setWordWrap(true);
	popup->setFixedSize(kNotificationLabelWidth, kNotificationLabelHeight);
	popup->setFocusPolicy(Qt::NoFocus);

	QPoint notificationWgtPos{0,0};

	QSize windowSize = this->window()->size();
	QPoint windowLoc = this->window()->pos();

	notificationWgtPos.setX(windowSize.width() + windowLoc.x() - kNotificationLabelWidth);
	notificationWgtPos.setY(windowSize.height() + windowLoc.y() - kNotificationLabelHeight - 10);

	popup->move(notificationWgtPos);
	popup->show();

	QTimer::singleShot(kNoticeTime, popup, &QLabel::hide);
}
