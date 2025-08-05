#ifndef DEVICECATALOG_H
#define DEVICECATALOG_H
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
			Biswajit Roy (biswroy@qti.qualcomm.com)
*/

#include "ui_DeviceCatalog.h"
#include "AlpacaDevice.h"

// Qt
#include <QDialog>


class DeviceCatalog :
	public QDialog,
	public Ui::DeviceCatalog
{
	Q_OBJECT

public:
	DeviceCatalog(QWidget *parent = nullptr);
	~DeviceCatalog();

private slots:
	void onConfigurationLinkClicked(QTableWidgetItem* twi);
	void onCustomContextMenuRequested(const QPoint& pos);
	void on__deviceTable_itemClicked(QTableWidgetItem *item);
	void on__programBtn_clicked();

	void on__firmwareUpdateBtn_clicked();
	void on__docsBtn_clicked();

private:
	AlpacaDevices enumerateDevices(const DebugBoardType type);

	void deviceSelectionDialog(const DebugBoardType type);

	void processProgramming(const DebugBoardType &type, const QPoint &pos, int row);

	void invokeProgrammer(const DebugBoardType type);

	void invokeLiteProgrammer(const QString &serialNumber, const PlatformID platformId);
	void invokePSOCProgrammer(const QString &serialNumber, const PlatformID platformId);

	QString                     _currentSerialNumber;
	PlatformID                  _currentPlatformId;
};
#endif // DEVICECATALOG_H
