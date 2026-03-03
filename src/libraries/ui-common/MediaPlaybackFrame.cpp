// Confidential and Proprietary Qualcomm Technologies, Inc.

// NO PUBLIC DISCLOSURE PERMITTED:  Please report postings of this software on public servers or websites
// to: DocCtrlAgent@qualcomm.com.

// RESTRICTED USE AND DISCLOSURE:
// This software contains confidential and proprietary information and is not to be used, copied, reproduced, modified
// or distributed, in whole or in part, nor its contents revealed in any manner, without the express written permission
// of Qualcomm Technologies, Inc.

// Qualcomm is a trademark of QUALCOMM Incorporated, registered in the United States and other countries. All
// QUALCOMM Incorporated trademarks are used with permission.

// This software may be subject to U.S. and international export, re-export, or transfer laws.  Diversion contrary to U.S.
// and international law is strictly prohibited.

// Qualcomm Technologies, Inc.
// 5775 Morehouse Drive
// San Diego, CA 92121 U.S.A.
// Copyright 2026 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "MediaPlaybackFrame.h"
#include "ui_MediaPlaybackFrame.h"


MediaPlaybackFrame::MediaPlaybackFrame(QWidget *parent) : QFrame(parent)
	, _ui(new Ui::MediaPlaybackFrame)
{
	_ui->setupUi(this);

	connect(&_player, &QMediaPlayer::mediaStatusChanged, this, &MediaPlaybackFrame::onMediaStatusChanged);
}

MediaPlaybackFrame::~MediaPlaybackFrame()
{
	delete _ui;
}

void MediaPlaybackFrame::playback(const QMediaPlayer::PlaybackState action, const QString &mediaPath)
{
	if (mediaPath.isEmpty() == false)
	{
		_player.setSource(QUrl(mediaPath));
		_player.setVideoOutput(&_vw);
		_player.setAudioOutput(&_ao);

		// _ui->_splitter->setSizes({int(0.6*width()), int(0.4*width())});
		_ui->_mediaHLayout->addWidget(&_vw);
		_ao.setVolume(100);
	}

	switch (action)
	{
	case QMediaPlayer::PlayingState:
		_player.play();
		break;
	case QMediaPlayer::PausedState:
		_player.pause();
		break;
	case QMediaPlayer::StoppedState:
		_player.stop();
		break;
	}

	QString playError = _player.errorString();
	if (playError.isEmpty() == false)
	{
		// _ui->_infoLabelText->setText("Error occured during playback: " + playError);
		// _ui->_infoLabelText->show();

		qDebug() << playError;

		_player.stop();
		_vw.hide();
	}
}

void MediaPlaybackFrame::onMediaStatusChanged(const QMediaPlayer::MediaStatus status)
{
	switch (status)
	{
	case QMediaPlayer::NoMedia:
	case QMediaPlayer::LoadingMedia:
	case QMediaPlayer::EndOfMedia:
	case QMediaPlayer::InvalidMedia:
		_vw.hide();

		// TODO: Fix ugly implementation
		_ui->_playBtn->setIcon(QIcon(":/Play.png"));
		_ui->_playBtn->setToolTip("Play the media");
		break;
	case QMediaPlayer::LoadedMedia:
	case QMediaPlayer::StalledMedia:
	case QMediaPlayer::BufferingMedia:
	case QMediaPlayer::BufferedMedia:
		_vw.show();

		// TODO: Fix ugly implementation
		_ui->_playBtn->setIcon(QIcon(":/Pause.png"));
		_ui->_playBtn->setToolTip("Pause the media");
		break;
	}
}

void MediaPlaybackFrame::on__playBtn_clicked()
{
	if (_player.playbackState() == QMediaPlayer::PlayingState)
	{
		playback(QMediaPlayer::PausedState);
		_ui->_playBtn->setIcon(QIcon(":/Play.png"));
		_ui->_playBtn->setToolTip("Play the media");
	}
	else
	{
		playback(QMediaPlayer::PlayingState);
		_ui->_playBtn->setIcon(QIcon(":/Pause.png"));
		_ui->_playBtn->setToolTip("Pause the media");
	}
}

void MediaPlaybackFrame::on__stopBtn_clicked()
{
	playback(QMediaPlayer::StoppedState);

	_vw.hide();
	_ui->_playBtn->setIcon(QIcon(":/Play.png"));
	_ui->_playBtn->setToolTip("Play the media");
}


void MediaPlaybackFrame::on__rewindBtn_clicked()
{
	// TODO: Fix ugly implementation
	_player.setPosition(0);
	playback(QMediaPlayer::PlayingState);
}
