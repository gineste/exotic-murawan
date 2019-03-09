package com.disk91;

public class Tools {

    public static boolean log_on = true;

    private final static char[] hexArray = "0123456789ABCDEF".toCharArray();
    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }

    public static byte[] reverseByteArray(byte[] array) {
        byte[] ret = new byte[array.length];

        for(int i = 0; i < array.length; ++i) {
            ret[i] = array[array.length - 1 - i];
        }

        return ret;
    }

    public static int ubyte(byte b) {
        if ( b < 0 ) {
            return 256 + b;
        } else {
            return b;
        }
    }

    public static byte toByte(int v) {
        if ( v > 127 ) {
            return (byte)(v-256);
        } else {
            return (byte)v;
        }
    }

    public static String byteArrayToString(byte [] d) {
        String r = "";
        for ( byte b : d ) {
            if ( b == 0 ) break;
            char c = (char)b;
            r += c;
        }
        return r;
    }

}
