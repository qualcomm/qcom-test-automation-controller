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
