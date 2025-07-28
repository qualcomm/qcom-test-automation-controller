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
// Copyright 2020 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

#include "LicenseManager.h"

//
#include <QCoreApplication>
#include <QDir>

void errorCallback(char* errStr);
void warningCallback(char* warningStr);
void infoCallback(char* infoStr);

LicenseManager* LicenseManager::_instance = Q_NULLPTR;

LicenseManager::LicenseManager
(
	AppCore& appCore
) :
	_appCore(appCore)
{
	QString fileName;

	QCoreApplication* app = QCoreApplication::instance();
	if (app)
	{
		fileName = QDir::cleanPath(app->applicationDirPath() + QDir::separator() + "LimeClient");
	}
	else
	{
		fileName = "LimeClient";
	}

	_limeLibrary.setFileName(fileName);

	_initializeProc = (LimeInitialize) _limeLibrary.resolve("Lime_Initialize");
	if (_initializeProc != Q_NULLPTR)
	{
		_lastLimeCode = _initializeProc("", "");
		if (_lastLimeCode == LIME_CLIENT_SUCCESS)
		{
			_getVersion = (GetVersion) _limeLibrary.resolve("Lime_GetVersion");
			if (_getVersion)
			{
				char* lookAtMe = _getVersion();

				_limeVersion = QByteArray(lookAtMe);
			}
			else
			{
				_appCore.writeToAppLog("Lime_GetVersion Failed\n");
			}

			_setWarningCallback = (SetWarningCallback) _limeLibrary.resolve("Lime_SetWarningCallback");
			if (_setWarningCallback)
				_setWarningCallback(warningCallback);

			_setErrorCallback = (SetErrorCallback) _limeLibrary.resolve("Lime_SetErrorCallback");
			if (_setErrorCallback)
				_setErrorCallback(errorCallback);

			_setInfoCallback = (SetInfoCallback) _limeLibrary.resolve("Lime_SetInfoCallback");
			if (_setInfoCallback)
				_setInfoCallback(infoCallback);

			_checkLicense = (CheckLicense) _limeLibrary.resolve("Lime_CheckLicense");
			if (_checkLicense != Q_NULLPTR)
			{
				_valid = true;
			}
			else
			{
				_appCore.writeToAppLog("Lime_CheckLicense Failed to resolve\n");
			}
		}
		else
		{
			_appCore.writeToAppLog("_initializeProc Failed\n");
		}
	}
	else
	{
		_appCore.writeToAppLog("Lime_Initialize Failed\n");
	}

//	1    0 0003E490 Lime_ActivateLicenseRequest
//	2    1 0003E620 Lime_ActivateLicenseResponse
//	3    2 0003E7A0 Lime_CheckLicense
//	4    3 0003E840 Lime_DeactivateLicenseRequest
//	5    4 0003E970 Lime_DeactivateLicenseResponse
//	6    5 0003EA70 Lime_DeleteLicenseFiles
//	7    6 0003EA80 Lime_GetActiveLicenses
//	8    7 0003EB50 Lime_GetActiveLicensesByFeature
//	9    8 0003EC90 Lime_GetErrorString
//   10    9 0003ED30 Lime_GetLicenseInfo
//   11    A 0003EE30 Lime_GetLicenses
//   12    B 0003EF00 Lime_GetLockState
//   13    C 0003EFD0 Lime_GetVersion
//   14    D 0003F060 Lime_Initialize
//   15    E 0003F110 Lime_RecoverLicenseRequest
//   16    F 0003F210 Lime_RecoverLicenseResponse
//   17   10 0003F310 Lime_SetErrorCallback
//   18   11 0003F320 Lime_SetInfoCallback
//   19   12 0003F330 Lime_SetLockState
//   20   13 0001ED40 Lime_SetWarningCallback
//   21   14 0003F390 Lime_VerifyLicenseRequest
//   22   15 0003F490 Lime_VerifyLicenseResponse

}

LicenseManager::~LicenseManager()
{
	_limeLibrary.unload();
}

LicenseManager *LicenseManager::getInstance
(
	AppCore& appCore
)
{
	if (_instance == Q_NULLPTR)
	{
		_instance = new LicenseManager(appCore);
	}

	return _instance;
}

void LicenseManager::release()
{
	if (_instance != Q_NULLPTR)
	{
		delete _instance;
		_instance = Q_NULLPTR;
	}
}

bool LicenseManager::checkLicence
(
	const QByteArray& productId,
	const QByteArray& featureId
)
{
	bool result{false};

	if (_valid)
	{
		_lastLimeCode = _checkLicense(productId.data(), featureId.data());
		result = (_lastLimeCode == LIME_CLIENT_SUCCESS);
	}
	else
	{
		_appCore.writeToAppLog("LicenseManager::checkLicence isn't valid\n");
	}

	return result;
}

void LicenseManager::licenseDeactivated
(
	std::string licenseId,
	std::string activationId
)
{
	Q_UNUSED(licenseId)
	Q_UNUSED(activationId)

	emit licenseExpired();
}

void errorCallback(char* errStr)
{
	QByteArray message = "LicenseManager::" + QByteArray(errStr);
	message = message.left(80);
	message += "\n";
//	_appCore.writeToLog(message);
}

void warningCallback(char* warningStr)
{
	QByteArray message = "LicenseManager::" + QByteArray(warningStr);
	message = message.left(80);
	message += "\n";
//	_appCore.writeToLog(message);
}

void infoCallback(char* infoStr)
{
	QByteArray message = "LicenseManager::" + QByteArray(infoStr);
	message = message.left(80);
	message += "\n";
//	_appCore.writeToLog(message);
}
