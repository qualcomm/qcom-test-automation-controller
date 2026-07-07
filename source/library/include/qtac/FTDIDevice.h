// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//         notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//         copyright notice, this list of conditions and the following
//         disclaimer in the documentation and/or other materials provided
//         with the distribution.
//
//     * Neither the name of Qualcomm Technologies, Inc. nor the names of its
//         contributors may be used to endorse or promote products derived
//         from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
// HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
// IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
// IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: Michael Simpson

#ifndef QTAC_FTDIDEVICE_H
#define QTAC_FTDIDEVICE_H

#include <qtac/AlpacaDevice.h>
#include <qtac/FTDIPlatformConfiguration.h>

class FTDIDevice : public _AlpacaDevice
{
public:
	FTDIDevice() = default;
	virtual ~FTDIDevice() { delete _ftdiPlatformConfiguration; }

	static bool programDevice(AlpacaDevice alpacaDevice,
	                          PlatformID platformID,
	                          qtac::ByteArray& errorMessage);

	static uint32_t updateAlpacaDevices();

	virtual bool open() override;

	void buildCommandList();
	virtual void buildMapping() override;
	virtual Pins getPins() override;
	virtual void quickCommand(const qtac::ByteArray& command) override;
	virtual void setVariableValue(const qtac::String& name, const qtac::Variant& value) override;
	virtual const qtac::ButtonEntries&   getButtons()   const override;
	virtual const qtac::VariableEntries& getVariables() const override;

private:
	_FTDIPlatformConfiguration* _ftdiPlatformConfiguration{nullptr};
};

#endif // QTAC_FTDIDEVICE_H
