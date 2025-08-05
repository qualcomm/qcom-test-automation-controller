#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H
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

// QCommon
#include "QCommonGlobal.h"

// QT
#include <QDialog>
#include <QPixmap>

namespace Ui
{
	class AboutDialogClass;
}

class QCOMMON_EXPORT AboutDialog :
	public QDialog
{
Q_OBJECT

public:
	AboutDialog(QWidget* parent = Q_NULLPTR);

	void setTitle(const QByteArray& title);
	void setAppName(const QString& appName);
	void setAppVersion(const QByteArray& appVersion);
	void setAboutText(const QByteArray& aboutText);
	void setBackSplash(const QPixmap& backSplash);

private slots:
	void on__aboutText_anchorClicked(const QUrl& link);
	void on__checkDateButton_clicked();
	void on__okayButton_clicked();

protected:
	virtual void changeEvent(QEvent* e);
	virtual void mouseReleaseEvent(QMouseEvent* mouseReleaseEvent);

private:
	Ui::AboutDialogClass*		_ui{Q_NULLPTR};
};

#endif // ABOUTDIALOG_H
