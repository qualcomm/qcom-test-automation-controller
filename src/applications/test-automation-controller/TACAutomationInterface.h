#ifndef TACAUTOMATIONINTERFACE_H
#define TACAUTOMATIONINTERFACE_H
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
// Copyright 2019 Qualcomm Technologies, Inc.
// All rights reserved.
// Qualcomm Technologies Confidential and Proprietary

/*
	Author: Michael Simpson (msimpson@qti.qualcomm.com)
*/

// Qt
#include <QString>

class TACAutomationInterface
{
public:
	TACAutomationInterface() {}
	virtual ~TACAutomationInterface(){}

	virtual void transactionComplete() = 0;
	virtual void openComplete() = 0;
	virtual void openError(const QString& errorString) = 0;
	virtual void deviceDisconnected() = 0;

	virtual void batteryStateChange(bool newState) = 0;
	virtual void usb0StateChange(bool newState) = 0;
	virtual void usb1StateChange(bool newState) = 0;
	virtual void powerKeyStateChange(bool newState) = 0;
	virtual void volumeUpStateChange(bool newState) = 0;
	virtual void volumeDownStateChange(bool newState) = 0;

	virtual void uim1StateChange(bool newState) = 0;
	virtual void uim2StateChange(bool newState) = 0;
	virtual void disconnectSDCardStateChange(bool newState) = 0;
	virtual void primaryEDLStateChange(bool newState) = 0;
	virtual void forcePSHoldHighStateChange(bool newState) = 0;
    virtual void secondaryEDLStateChange(bool newState) = 0;
    virtual void secondaryPM_RESIN_N_StateChange(bool newState) = 0;
    virtual void eud_StateChange(bool newState) = 0;
	virtual void headsetDisconnectStateChange(bool newState) = 0;

	virtual void resetCountUpdated(quint32 resetCount) = 0;
	virtual void clearResetCountUpdate() = 0;
};

#endif // TACAUTOMATIONINTERFACE_H
