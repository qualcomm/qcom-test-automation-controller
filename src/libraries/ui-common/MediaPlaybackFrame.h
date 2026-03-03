#ifndef MEDIAPLAYBACKFRAME_H
#define MEDIAPLAYBACKFRAME_H

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
