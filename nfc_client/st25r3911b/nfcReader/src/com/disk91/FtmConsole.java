package com.disk91;

import com.st.st25pc.model.readers.RFGenericReader;
import com.st.st25sdk.STException;
import com.st.st25sdk.STRegister;
import com.st.st25sdk.type5.ReadBlockResult;
import com.st.st25sdk.type5.Type5Tag;
import com.st.st25sdk.type5.st25dv.ST25DVTag;

import java.util.Scanner;

public class FtmConsole {

    private byte [] _uid;
    private RFGenericReader _reader;
    private ST25DVTag _tag;

    public FtmConsole (
            byte [] uid,
            RFGenericReader reader,
            ST25DVTag tag
    ) throws TagException {
        this._uid = uid;
        this._reader = reader;
        this._tag = tag;

        // Verify everything is ok before proceeding
        try {
            Tools.log_on=false;
            // Read EH_CTRL_DYN
            STRegister r = _tag.getDynamicRegister(0x02);
            if ( ( r.getRegisterValue() & 0x0C ) != 0x0C ) {
                System.err.println("VCC or RFField not detected, communication non possible");
                throw new TagException();
            }
            // Read MB_CTRL_DYN
            if ( ! _tag.isMailboxEnabled(true) ) {
                System.err.println("MailBox not enabled");
                throw new TagException();
            }
        } catch (STException e) {
            System.err.println(e.getMessage());
            throw new TagException();
        }
    }

    public void interractiveCommand() throws TagException {
        byte [] buffer;
        int len;
        try {
            Tools.log_on=true;
            while ( true ) {
                // Ready to transfert
                System.out.print("ftmSerial >");
                Tools.log_on=false;
                if (_tag.hasHostPutMsg(true)) {
                    // Pending data to read - clean this
                    len = _tag.readMailboxMessageLength();
                    buffer = _tag.readMailboxMessage(0, len);
                }
                Tools.log_on=true;
                System.out.print("> ");
                Scanner scanner = new Scanner(System.in);
                String cmd = scanner.nextLine();
                if ( cmd.length() == 0 ) return;
                try {
                    this.executeCommand(cmd);
                } catch (TagException e) {
                    // normal exit of the command
                    break;
                }
            }
        } catch (STException e) {
            System.err.println(e.getMessage());
            throw new TagException();
        }

    }



    public boolean executeCommand(String cmd) throws TagException {

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
        Tools.log_on = false;
        writeFtmWithRetry(ba,ba.length);
        try {
            while (true) {
                Tools.log_on = false;
                int tries = 0;
                byte [] buffer;
                // Wait for HOST_PUT_MSG flag
                STRegister r = _tag.getDynamicRegister(0x0D);   // MB_CTRL_Dyn
                do {
                   Thread.sleep(100);
                   r.invalidateCache(); // retry needed
                   try {
                       if ((r.getRegisterValue() & 0x02) == 0x02) {
                           break;
                       }
                   }catch (STException e) {
                       // Failed to read... we are in a retry loop, get benefic of it
                   }
                   tries++;
                } while (tries < 30 ); // 3s max wait
                if ( tries == 30 ) return false;
                buffer = readFtmWithRetry();
                Tools.log_on = true;
                String srep = Tools.byteArrayToString(buffer);
                System.out.print(srep);
                if ( srep.length() >= 2 ) {
                    String sreps = srep.substring(0, 2);
                    if (sreps.compareToIgnoreCase("OK") == 0) {
                        return true;
                    } else if (sreps.compareToIgnoreCase("KO") == 0) {
                        return false;
                    }
                }
            }
        } catch (TagException e) {
            Tools.log_on = true;
            System.err.println(e.getMessage());
            throw new TagException();
        }  catch (InterruptedException e) {
            Tools.log_on = true;
            throw new TagException();
        }

    }


    protected byte writeFtmWithRetry (byte[] buffer,int len) throws TagException {
        int tries = 0;
        byte r;
        while ( tries < 5 ) {
            try {
                r = _tag.writeMailboxMessage(len,buffer);
                return r;
            } catch (STException e) {
                tries++;
                try {
                    Thread.sleep(50);
                }catch (InterruptedException f) {}
            }
        }
        throw new TagException("Failed to write FTM");
    }

    protected byte [] readFtmWithRetry() throws TagException{
        int tries = 0;
        while ( tries < 5 ) {
            try {
                int len = _tag.readMailboxMessageLength();
                byte [] buffer = _tag.readMailboxMessage(0, len);
                return buffer;
            } catch (STException e) {
                tries++;
                try {
                    Thread.sleep(50);
                }catch (InterruptedException f) {}
            }
        }
        throw new TagException("Failed to read FTM");
    }




}
