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
