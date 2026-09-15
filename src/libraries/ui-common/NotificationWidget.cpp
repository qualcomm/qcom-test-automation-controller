// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "ui_NotificationWidget.h"
#include "NotificationWidget.h"
#include "ColorConversion.h"

// Qt
#include <QLabel>
#include <QStatusBar>

const QSize kNotificationIconSize(10,10);
const quint32 kProgressClearDelay(100);
const quint32 kToastDisplayTime(3000);
const QString kProgressStyle("QProgressBar::chunk {background-color: %1; width: 1px;}");
const QByteArray kOperationMsg("Operation in progress...");

NotificationWidget::NotificationWidget(QWidget *parent)
	: QWidget{parent},
	_ui(new Ui::NotificationWidgetClass)
{
	_ui->setupUi(this);

	_notificationWindow = new HoverAwareQWindow(this);
	connect(_notificationWindow, &HoverAwareQWindow::clearAll, this, &NotificationWidget::onNotificationCleared);

	updateBellIcon();

	connect(this, &NotificationWidget::notificationAdded, this, &NotificationWidget::onNotificationAdded);
	connect(this, &NotificationWidget::progress, this, &NotificationWidget::onProgressUpdated);
	connect(_ui->_notificationBtn, &QPushButton::clicked, this, &NotificationWidget::onNotificationButtonClicked);
	connect(&_timer, &QTimer::timeout, this, &NotificationWidget::onTimerTimeout);

	_timer.setTimerType(Qt::PreciseTimer);
	_timer.setInterval(kProgressClearDelay);
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

	_hasUnread = true;
	updateBellIcon();

	emit notificationAdded(message, notificationLevel);
}

void NotificationWidget::onNotificationAdded(const QString &message, const NotificationLevel notificationLevel)
{
	if (_silent == false)
		makeNotificationLabel(message, notificationLevel);

	_ui->_notificationBtn->setToolTip("Click to see notifications");
	_ui->_notificationBtn->setEnabled(true);
}

void NotificationWidget::onNotificationCleared()
{
	_hasUnread = false;
	updateBellIcon();

	_ui->_notificationBtn->setToolTip("You do not have new notifications");
	_ui->_notificationBtn->setEnabled(false);
}

void NotificationWidget::onNotificationButtonClicked()
{
	QWidget* topLevelWindow = this->window();

	if (topLevelWindow != Q_NULLPTR)
		_notificationWindow->setWindowLocation(topLevelWindow->size(), topLevelWindow->pos());
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
	QLabel* popup = new QLabel(this->window());
	popup->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
	popup->setAttribute(Qt::WA_ShowWithoutActivating);
	popup->setAttribute(Qt::WA_TransparentForMouseEvents);
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

	notificationWgtPos.setX(windowSize.width() + windowLoc.x() - kNotificationLabelWidth - 10);
	notificationWgtPos.setY(windowSize.height() + windowLoc.y() - kNotificationLabelHeight - 40);

	popup->move(notificationWgtPos);
	popup->show();

	QTimer::singleShot(kToastDisplayTime, popup, &QLabel::deleteLater);
}

void NotificationWidget::updateBellIcon()
{
	QString iconPath = _hasUnread
		? QStringLiteral(":/NotificationBellRing.png")
		: QStringLiteral(":/NotificationBellSilent.png");

	QIcon notificationIcon;
	notificationIcon.addFile(iconPath, kNotificationIconSize, QIcon::Normal, QIcon::Off);

	_ui->_notificationBtn->setIcon(notificationIcon);
}
