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

// Author: Biswajit Roy <biswroy@qti.qualcomm.com>

package com.qualcomm.alpaca;

import java.nio.charset.StandardCharsets;

import com.sun.jna.ptr.IntByReference;

/**
 * TACDev class provides access to basic QTAC methods and acts
 * as an entrypoint to all TACDevice instances
 */
final public class TACDev {

    private final int bufferSize = 1024;

    private final long NO_TAC_ERROR = 0;
    private final long TACDEV_BAD_TAC_HANDLE = 2;
    private final long TACDEV_COMMAND_NOT_FOUND = 3;
    private final long TACDEV_BAD_INDEX = 4;
    private final long TACDEV_INIT_FAILED = 5;

    public TACDev() throws TACDevException {
        long initResult = BaseTACDev.INSTANCE.InitializeTACDev();
        if (initResult == TACDEV_INIT_FAILED) {
            throw new TACDevException("TACDev failed to initialize.");
        }
    }

    /**
     * Returns the version of QTAC in use
     * @return The string containing the QTAC version
     * @throws TACDevException
     */
    public String AlpacaVersion() throws TACDevException {
        byte[] buffer = new byte[bufferSize];
        try {
            long result = BaseTACDev.INSTANCE.GetAlpacaVersion(buffer, bufferSize);
            if (result != NO_TAC_ERROR)
                throw new TACDevException("AlpacaVersion() failed with error code: " + result);
        } catch(UnsatisfiedLinkError error) {
            System.out.println("Error occurred while loading TACDev library: " + error);
            System.exit(1);
        }
        return new String(buffer, StandardCharsets.US_ASCII).trim();
    }

    /**
     * Returns the version of TAC in use
     * @return The string containing the TAC version
     * @throws TACDevException
     */
    public String TACVersion() throws TACDevException {
        byte[] buffer = new byte[bufferSize];
        try {
            long result = BaseTACDev.INSTANCE.GetTACVersion(buffer, bufferSize);
            if (result != NO_TAC_ERROR)
                throw new TACDevException("AlpacaVersion() failed with error code: " + result);

        } catch (UnsatisfiedLinkError error) {
            System.out.println("Error occurred while loading TACDev library: " + error);
            System.exit(1);
        }
        return new String(buffer, StandardCharsets.US_ASCII).trim();
    }

    /**
     * Returns the count of connected TAC devices
     * @return The integer representing the number of connected TAC devices
     * @throws TACDevException
     */
    public long GetDeviceCount() throws TACDevException {
        try {
            IntByReference deviceCount = new IntByReference(0);

            long result = BaseTACDev.INSTANCE.GetDeviceCount(deviceCount);
            if (result != NO_TAC_ERROR)
                throw new TACDevException("AlpacaVersion() failed with error code: " + result);
            return deviceCount.getValue();

        } catch(UnsatisfiedLinkError error) {
            System.out.println("Error occurred while loading TACDev library: " + error);
            System.exit(1);
            return -1;
        }
    }

    /**
     * Returns a TACDevice object for the `deviceIndex` parameter
     * @param deviceIndex The index of the TACDevice. This value should always be less than the value returned by `GetDeviceCount()`
     * @return A TACDevice object for the TAC device
     * @throws TACDevException
     */
    public TACDevice GetDevice(int deviceIndex) throws TACDevException {
        TACDevice tacDevice = null;
        byte[] buffer = new byte[bufferSize];

        try {
            long portDataByteCount = BaseTACDev.INSTANCE.GetPortData(deviceIndex, buffer, bufferSize);

            if (portDataByteCount <= 0)
                throw new TACDevException("No bytes in port data. Function returned: " + portDataByteCount);

        } catch(UnsatisfiedLinkError error) {
            System.out.println("Error occurred while loading TACDev library: " + error);
            System.exit(1);
        } finally {
            String portData = new String(buffer, StandardCharsets.US_ASCII).trim();
            String[] deviceAttributes = portData.split(";");

            if(deviceAttributes.length >= 3) {
                tacDevice = new TACDevice(deviceAttributes[0], deviceAttributes[1], deviceAttributes[2]);
            }
        }
        return tacDevice;
    }
}