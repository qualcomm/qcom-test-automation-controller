#ifndef LICENSEMANAGER_H
#define LICENSEMANAGER_H
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
// Copyright 2020-2021 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "LimeWrapper.h"

// QCommon
#include "QCommonConsoleGlobal.h"
#include "AppCore.h"

// Qt
#include <QByteArray>
#include <QLibrary>
#include <QObject>


class LicenseManager;

typedef eLimeReturnCode (*LimeInitialize)(const char* userId, const char* storageLocation);
typedef char* (*GetVersion)();
typedef eLimeReturnCode (*CheckLicense)(const char* productId, const char* featureId);
typedef void (*SetWarningCallback)(WarningCallback pCallback);
typedef void (*SetErrorCallback)(ErrorCallback pErrorCB);
typedef void (*SetInfoCallback)(InfoCallback pCallback);

class QCOMMONCONSOLE_EXPORT LicenseManager :
	public QObject
{
Q_OBJECT

public:

	virtual ~LicenseManager();

	eLimeReturnCode lastLimeError()
	{
		eLimeReturnCode lastCode = _lastLimeCode;
		_lastLimeCode = LIME_CLIENT_SUCCESS;
		return lastCode;
	}

	static LicenseManager* getInstance(AppCore& appCore);
	static void release();

	bool valid()
	{
		return _valid;
	}
	bool checkLicence(const QByteArray& productId, const QByteArray& featureId);

	// Lime Callbacks
	virtual void licenseDeactivated(std::string licenseId, std::string activationId);

signals:
	void licenseExpired();

private:
	LicenseManager(AppCore& appCore);

	QLibrary							_limeLibrary;
	LimeInitialize						_initializeProc{Q_NULLPTR};
	GetVersion							_getVersion{Q_NULLPTR};
	CheckLicense						_checkLicense{Q_NULLPTR};
	SetWarningCallback					_setWarningCallback{Q_NULLPTR};
	SetErrorCallback					_setErrorCallback{Q_NULLPTR};
	SetInfoCallback						_setInfoCallback{Q_NULLPTR};
	AppCore&							_appCore;

	static LicenseManager*				_instance;
	bool								_valid{false};
	QByteArray							_limeVersion;
	eLimeReturnCode						_lastLimeCode;
};

#endif // LICENSEMANAGER_H
