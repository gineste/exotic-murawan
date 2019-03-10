package com.disk91;

import com.st.st25pc.model.readers.RFGenericReader;
import com.st.st25sdk.STException;
import com.st.st25sdk.type5.ReadBlockResult;
import com.st.st25sdk.type5.Type5Tag;

import java.util.Scanner;

public class UzConsole {

    private byte [] _uid;
    private RFGenericReader _reader;
    private Type5Tag _tag;
    private int _offset;
    private NfcStore.Header _h;

    protected class Header {
        int magic;
        int hostIsWritting;
        int hostWritedone;
        int hostReadDone;
        int mcuIsWritting;
        int mcuWriteDone;
        int mcuReadDone;

        public void initFromByteStream(byte [] b) {
            if ( b.length != 4 ) return;
            magic = Tools.ubyte(b[0]);
            magic += Tools.ubyte(b[1]) << 8;

            hostIsWritting = (Tools.ubyte(b[2]) & 0x10) >> 4;
            hostWritedone = (Tools.ubyte(b[2]) & 0x20) >> 5;
            hostReadDone = (Tools.ubyte(b[2]) & 0x40) >> 6;
            mcuIsWritting = (Tools.ubyte(b[3]) & 0x10) >> 4;
            mcuWriteDone = (Tools.ubyte(b[3]) & 0x20) >> 5;
            mcuReadDone = (Tools.ubyte(b[3]) & 0x40) >> 6;
        }

        public byte[] getByteStream() {
            byte[] b = new byte[4];
            b[0] = Tools.toByte((magic >>  0) & 0xFF);
            b[1] = Tools.toByte((magic >>  8) & 0xFF);
            b[2] = Tools.toByte((hostIsWritting << 4) | (hostWritedone << 5) | (hostReadDone << 6));
            b[3] = Tools.toByte((mcuIsWritting << 4) | ( mcuWriteDone << 5) | (mcuReadDone << 6));
            return b;
        }

        public void loadHeader(int offset, Type5Tag tag ) throws  STException {
            Tools.log_on=false;
            ReadBlockResult r = readBlockWithRetry(offset, 1);
            Tools.log_on=true;
            this.initFromByteStream(r.data);
        }

        public void writeHeader(int offset, Type5Tag tag ) throws  STException {
            Tools.log_on=false;
            writeBlockWithRetry(offset,this.getByteStream());
            Tools.log_on=true;
        }
    }

    protected ReadBlockResult readBlockWithRetry(int offset, int blocks) throws STException {
        int tries = 0;
        while ( tries < 5 ) {
            try {
                ReadBlockResult r = _tag.readBlocks(offset, blocks);
                return r;
            } catch (STException e) {
                tries++;
                try {
                    Thread.sleep(50);
                } catch (InterruptedException f) {
                }
            }
        }
        throw new STException("too many retries");
    }

    protected void writeBlockWithRetry(int offset, byte [] blocks) throws STException {
        int tries = 0;
        while ( tries < 5 ) {
            try {
                _tag.writeBlocks(offset, blocks);
                return;
            } catch (STException e) {
                tries++;
                try {
                    Thread.sleep(50);
                } catch (InterruptedException f) {
                }
            }
        }
        throw new STException("too many retries");
    }


    public UzConsole (
            byte [] uid,
            RFGenericReader reader,
            Type5Tag tag,
            int offset
    ) throws TagException {
        this._uid = uid;
        this._reader = reader;
        this._tag = tag;
        this._offset = offset;
    }



    public void interractiveCommand() throws TagException {
        Header _h = new Header();

        // get header bloc
        try {
            while ( true ) {
                _h.loadHeader(_offset,_tag);
                if ( _h.magic == 0xCAFE ) {
                    /*
                    System.out.println("Found uzSerial : " +
                            "magic("+String.format("%04X",_h.magic)+") " +
                            "hostIsWritting("+_h.hostIsWritting+") " +
                            "hostWritedone("+_h.hostWritedone+") " +
                            "hostReadDone("+_h.hostReadDone+") " +
                            "mcuIsWritting("+_h.mcuIsWritting+")" +
                            "mcuWriteDone("+_h.mcuWriteDone+")" +
                            "mcuReadDone("+_h.mcuReadDone+")");
                     */
                    System.out.print("uzSerial >");
                    if ( _h.mcuIsWritting == 0 ) {
                        System.out.print("> ");
                        Scanner scanner = new Scanner(System.in);
                        String cmd = scanner.nextLine();
                        try {
                            executeCommand(cmd);
                        } catch (TagException e) {
                            // normal exit of the command
                            break;
                        }
                    }
                } else {
                    System.err.println("uzSerial header not found");
                }
            }

        } catch (STException e) {
            System.err.println("crash");
            e.printStackTrace();
            throw new TagException();
        }

    }


    /**
     * Execute a command and return true if it has respond OK
     * @param cmd
     * @return
     * @throws STException
     */
    public boolean executeCommand(String cmd) throws TagException {
        Header _h = new Header();
        try {
            _h.loadHeader(_offset,_tag);
            if ( _h.magic == 0xCAFE && _h.mcuIsWritting == 0 ) {
                if (cmd.length() > 0) {
                    // transform command line into a byte array of char
                    // add the line ending
                    char[] ca = cmd.toCharArray();
                    int len = ca.length + 3;
                    int lblock = ((len & 3) == 0) ? len >> 2 : 1 + (len >> 2);
                    byte[] ba = new byte[4 * lblock];
                    for (int i = 0; i < ca.length; i++) {
                        ba[i] = (byte) ca[i];
                    }
                    ba[ca.length] = (byte) '\r';
                    ba[ca.length + 1] = (byte) '\n';
                    ba[ca.length + 2] = (byte) '\0';
                    for (int i = (ca.length + 3); i < 4 * lblock; i++) {
                        ba[i] = 0;
                    }

                    // Ready to write buffer
                    _h.hostIsWritting = 1;
                    _h.hostWritedone = 0;
                    _h.mcuReadDone = 0;
                    _h.writeHeader(_offset, _tag);
                    try {
                        Thread.sleep(50);
                    } catch (InterruptedException e) {
                    }
                    Tools.log_on = false;
                    this.writeBlockWithRetry(_offset + 1, ba);
                    Tools.log_on = true;
                    _h.hostIsWritting = 0;
                    _h.hostWritedone = 1;
                    _h.mcuWriteDone = 0;
                    _h.writeHeader(_offset, _tag);
                    // wait for response
                    // do {
                    //    _h.loadHeader(_offset,_tag);
                    //    try { Thread.sleep(50); } catch ( InterruptedException e ) {};
                    //} while ( _h.mcuReadDone == 0 );
                    while (true) {
                        int tries = 0;
                        do {
                            _h.loadHeader(_offset, _tag);
                            try {
                                Thread.sleep(50);
                            } catch (InterruptedException e) {
                            }
                            tries++;
                        } while (_h.mcuWriteDone == 0 && tries < 30);
                        if (tries == 30) {
                            return false;
                        } else {
                            // get response
                            Tools.log_on = false;
                            byte dresp[] = new byte[84];
                            ReadBlockResult resp = this.readBlockWithRetry(_offset + 1, 10);
                            boolean done = false;
                            for (int i = 0; i < 10 * 4; i++) {
                                dresp[i] = resp.data[i];
                                if (dresp[i] == 0) {
                                    done = true;
                                    break;
                                }
                            }
                            if (!done) {
                                resp = this.readBlockWithRetry(_offset + 1 + 10, 11);
                                for (int i = 0; i < 11 * 4; i++) {
                                    dresp[40 + i] = resp.data[i];
                                }
                            }
                            Tools.log_on = true;
                            _h.hostReadDone = 1;
                            _h.mcuWriteDone = 0;
                            _h.writeHeader(_offset, _tag);
                            String srep = Tools.byteArrayToString(dresp);
                            System.out.print(srep);
                            String sreps = srep.substring(0,2);
                            if (sreps.compareToIgnoreCase("OK") == 0 ) {
                                return true;
                            } else if ( sreps.compareToIgnoreCase("KO") == 0) {
                                return false;
                            }
                        }

                    }
                } else {
                    throw new TagException();
                }
            } else {
                throw new TagException();
            }

        } catch (STException e) {
            System.err.println("crash");
            e.printStackTrace();
            throw new TagException();
        }

    }

}
