#ifndef TACAUTOMATIONINTERFACE_H
#define TACAUTOMATIONINTERFACE_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

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
