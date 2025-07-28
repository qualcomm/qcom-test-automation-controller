#ifndef FTDIDEVICE_H
#define FTDIDEVICE_H
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
// Copyright 2012-2023 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "QCommonConsoleGlobal.h"

// libTAC
#include "AlpacaDevice.h"

// QCommon
class _FTDIPlatformConfiguration;

class QCOMMONCONSOLE_EXPORT FTDIDevice :
	public _AlpacaDevice
{
public:
	FTDIDevice() = default;
	virtual ~FTDIDevice();

	static bool programDevice(AlpacaDevice alpacaDevice, PlatformID platformID, QByteArray& errorMessage);

	static quint32 updateAlpacaDevices();

	virtual bool open();

	void buildCommandList();
	virtual void buildMapping();

private:
	_FTDIPlatformConfiguration* _ftdiPlatformConfiguration{Q_NULLPTR};
};

#endif // FTDIDEVICE_H
