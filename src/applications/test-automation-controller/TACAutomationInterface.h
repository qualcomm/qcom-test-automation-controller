#ifndef TACAUTOMATIONINTERFACE_H
#define TACAUTOMATIONINTERFACE_H
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
