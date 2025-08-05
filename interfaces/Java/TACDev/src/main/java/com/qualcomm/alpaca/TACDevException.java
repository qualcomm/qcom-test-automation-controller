// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause-Clear

// Author: Biswajit Roy <biswroy@qti.qualcomm.com>

package com.qualcomm.alpaca;

/**
 * TACDevException class is an exception wrapper class to throw custom
 * TAC exceptions with relevant error messages
 */
public class TACDevException extends Exception {
    public TACDevException() {
        super("TACDev exception occurred!");
    }
    public TACDevException(String message) {
        super(message);
    }
}
