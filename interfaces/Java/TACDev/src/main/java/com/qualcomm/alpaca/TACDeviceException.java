// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: Biswajit Roy <biswroy@qti.qualcomm.com>

package com.qualcomm.alpaca;

/**
 * TACDeviceException class is an exception wrapper class to throw custom
 * TAC exceptions with relevant error messages
 */
public class TACDeviceException extends Exception {
    public TACDeviceException() {
        super("TACDevice exception occurred!");
    }
    public TACDeviceException(String message) {
        super(message);
    }
}
