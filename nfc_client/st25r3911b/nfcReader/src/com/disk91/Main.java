package com.disk91;

import com.st.st25pc.model.readers.RFGenericReader;
import com.st.st25pc.model.readers.st.STReader;
import com.st.st25sdk.RFReaderInterface;
import com.st.st25sdk.STException;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.logging.LogManager;

public class Main {

    public RFGenericReader mActiveRFReader = null;
    public Tag t = null;

    public Main() throws TagException {
        // Try to instantiate a STReader (CR95HF or ST25R3911B-DISCO) to determine if one is connected
        STReader stReader = new STReader();

        // Because the lib is printing stuff on screen we do not want
        System.setOut(new PrintStream(System.out) {
            @Override
            public void println(String x) {
                if ( Tools.log_on )
                    super.println(x);
            }
        });

        System.setErr(new PrintStream(System.err) {
            @Override
            public void println(String x) {
                if ( Tools.log_on )
                    super.println("## "+x);
            }
        });
        // Also on the Logger
        LogManager.getLogManager().reset();

        if (stReader.connect()) {
            // Now able to communicate with the reader
            mActiveRFReader = stReader;
        } else {
            System.out.println("No reader is connected - please connect reader.");
            throw new TagException();
        }

    }

    public void connect() throws TagException {

        try {
            System.out.println(mActiveRFReader.getName() + " is connected");
        } catch ( STException e ) {
            e.printStackTrace();
        };
        System.out.println("Waiting for a tag ...");
        List<byte[]> uidList = new ArrayList<byte[]>();

        boolean found = false;
        do {
            try {
                // Call reader's 15693 anti-collision algorithm
                Tools.log_on = false;
                uidList = mActiveRFReader.getTransceiveInterface().inventory(RFReaderInterface.InventoryMode.NFC_TYPE_5);
                Tools.log_on = true;
                if (!uidList.isEmpty()) {
                    for (byte[] b : uidList) {
                        System.out.println("Found Tag: " + Tools.bytesToHex(b));
                    }
                    found=true;
                } else {
                    Thread.sleep(500);
                }
            } catch (STException e) {
                System.err.println("Error during tag initialization - contact lost ?");
                throw new TagException();
            } catch (InterruptedException e) {
                throw new TagException();
            }
        } while ( !found );
        if ( uidList.size() == 1 ) {
            t = new Tag(mActiveRFReader,uidList.get(0));
            try {
                t.init();
            } catch (STException e) {
                System.err.println("Error during tag initialization - contact lost ?");
                throw new TagException();
            }
        } else {
            System.out.println("Found too many tags. Only one is supported");
            throw new TagException();
        }
    }

    public enum mode_e {
        INTERACTIVE, LAST_VALUES, UZSERIAL, FTMSERIAL, UZSERIAL_INTERACTIVE, FTMSERIAL_INTERACTIVE, LAST_VALUES_INTERACTIVE
    }

    public static void main(String[] args) {

        mode_e mode = mode_e.INTERACTIVE;
        if( args.length > 0 ) {
            String arg = args[0];
            if ( arg.startsWith("-values")) {
                mode = mode_e.LAST_VALUES;
            }
            if ( arg.startsWith("-ftms:")) {
                mode = mode_e.FTMSERIAL;
            }
            if ( arg.startsWith("-uzs:")) {
                mode = mode_e.UZSERIAL;
            }
            if ( args.length > 1 || arg.startsWith("-h") || arg.startsWith("--help") ) {
                System.out.println("NFC Tool for ItSdK / Murawan");
                System.out.println("Options are [-values | -ftms:pass:cmd1,cmd2,...,cmdn | -uzs:pass:cmd1,cmd2,...,cmdn ]");
                System.out.println("    * With no arguments, the interactive mode is selected");
                System.out.println("    * -values : dump the memory zone to extract last measured values");
                System.out.println("    * -ftms / -uzs  : automatically send command to the device using");
                System.out.println("          > Serial-NFC over Fast Transfer Mail Box");
                System.out.println("          > Serial-NFC over UserMemory zone");
                System.out.println("       pass is the console password to be used");
                System.out.println("       cmd1,... is the list of command to be executed");
                return;
            }
        }

        boolean quit = false;
        try {
            Main m = new Main();
            m.connect();

            while ( !quit ) {
                if (mode == mode_e.INTERACTIVE || mode == mode_e.LAST_VALUES_INTERACTIVE) {
                    System.out.println("Select operation:");
                    System.out.println("1) Dump last values from NFC memory");
                    System.out.println("2) Access console on UZSerial");
                    System.out.println("3) Access console on FTM");
                    System.out.println("9) Exit");
                    System.out.print("# ");
                    Scanner scanner = new Scanner(System.in);
                    int choice = scanner.nextInt();
                    switch (choice) {
                        case 1:
                            mode = mode_e.LAST_VALUES_INTERACTIVE;
                            break;
                        case 2:
                            mode = mode_e.UZSERIAL_INTERACTIVE;
                            break;
                        case 3:
                            mode = mode_e.FTMSERIAL_INTERACTIVE;
                            break;
                        default:
                            return;
                    }
                }

                // Dump the last Memory Values
                if ( mode == mode_e.LAST_VALUES ) {
                    m.t.lastValues(0);
                    quit = true;
                } else if ( mode == mode_e.LAST_VALUES_INTERACTIVE ) {
                    m.t.lastValues(0);
                } else if ( mode == mode_e.UZSERIAL_INTERACTIVE ) {
                    m.t.uzConsole(42);
                    mode = mode_e.INTERACTIVE;
                }

            }
        } catch (TagException e) {
            return;
        }



    }
}
