package com.disk91;

public class TagException extends Exception {

    protected String message;

    public TagException(String mess) {
        super();
        this.message = mess;
    }

    public TagException() {
        super();
    }

    public String getMessage() {
        return this.message;
    }
}
