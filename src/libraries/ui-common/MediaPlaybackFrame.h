#ifndef MEDIAPLAYBACKFRAME_H
#define MEDIAPLAYBACKFRAME_H

// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

#include "UIGlobalLib.h"

// Qt
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QFrame>
#include <QVideoWidget>

namespace Ui { class MediaPlaybackFrame; }

class UILIB_EXPORT MediaPlaybackFrame : public QFrame
{
	Q_OBJECT

public:
	explicit MediaPlaybackFrame(QWidget *parent = nullptr);
	~MediaPlaybackFrame();

	void playback(const QMediaPlayer::PlaybackState action, const QString& mediaPath = QString());

public slots:
	void onMediaStatusChanged(const QMediaPlayer::MediaStatus status);

	void on__playBtn_clicked();
	void on__stopBtn_clicked();
	void on__rewindBtn_clicked();

private:
	Ui::MediaPlaybackFrame*		_ui;
	QMediaPlayer				_player;
	QVideoWidget				_vw;
	QAudioOutput				_ao;
};

#endif // MEDIAPLAYBACKFRAME_H
