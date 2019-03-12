package com.disk91;

import com.st.st25pc.model.readers.RFGenericReader;
import com.st.st25sdk.NFCTag;
import com.st.st25sdk.RFReaderInterface;
import com.st.st25sdk.STException;
import com.st.st25sdk.TagHelper;
import com.st.st25sdk.command.Iso15693Command;
import com.st.st25sdk.command.Iso15693Protocol;
import com.st.st25sdk.type5.Type5Tag;
import com.st.st25sdk.type5.st25dv.ST25DVTag;

public class Tag {

    private byte [] _uid;
    private RFGenericReader _reader;
    private ST25DVTag recognizedType5Tag;

    public Tag(RFGenericReader reader, byte[] uid) {
        this._uid = uid;
        this._reader = reader;
    }

    /**
     * Identify the tag and set-it up
     * @throws STException
     * @throws TagException
     */
    public void init() throws STException, TagException{
        Tools.log_on = false;
        Iso15693Command cmd = new Iso15693Command(_reader.getTransceiveInterface(), null);
        cmd.setFlag(Iso15693Protocol.HIGH_DATA_RATE_MODE);
        cmd.resetToReady();

        RFReaderInterface readerInterface = _reader.getTransceiveInterface();

        // You can also make direct calls to the API of the reader's RF implementation
        byte [] reversed = Tools.reverseByteArray(_uid);

        NFCTag.NfcTagTypes tagType = readerInterface.decodeTagType(reversed);
        recognizedType5Tag = null;
        // Use a method from the TagHelper class to determine the tag's name
        TagHelper.ProductID productName;
        if (tagType == NFCTag.NfcTagTypes.NFC_TAG_TYPE_V) {
            productName = TagHelper.identifyTypeVProduct(readerInterface, reversed);
        } else {
            productName = TagHelper.ProductID.PRODUCT_UNKNOWN;
        }
        switch (productName) {
            case PRODUCT_ST_ST25DV04K_I:
            case PRODUCT_ST_ST25DV04K_J:
            case PRODUCT_ST_ST25DV16K_I:
            case PRODUCT_ST_ST25DV16K_J:
            case PRODUCT_ST_ST25DV64K_I:
            case PRODUCT_ST_ST25DV64K_J:
                recognizedType5Tag = new ST25DVTag(readerInterface, reversed);
                recognizedType5Tag.setName(productName.toString());
                break;
            default:
                Tools.log_on = true;
                System.out.println("Invalid Tag type");
        }
        Tools.log_on = true;
    }

    /**
     * Access the last recorded values from the user zone
     * Offset is the address where the values are starting to be stored.
     * The User Zone is 1 and publically accessible. No password
     * @param offset
     */
    public void lastValues(int offset) {

        try {
            NfcStore n = new NfcStore(_uid,_reader,recognizedType5Tag,offset);

        } catch ( TagException e ) {
            Tools.log_on = true;
            return;
        }

    }

    /**
     * Access to the user land memory serial console
     * @param offset
     */
    public void uzConsole(int offset) {
        try {
            UzConsole n = new UzConsole(_uid,_reader,recognizedType5Tag,offset);
            n.interractiveCommand();
        } catch ( TagException e ) {
            Tools.log_on = true;
            return;
        }
    }

    public boolean uzCommand(String cmd, int offset) {
        try {
            UzConsole n = new UzConsole(_uid,_reader,recognizedType5Tag,offset);
            return n.executeCommand(cmd);
        } catch ( TagException e ) {
            Tools.log_on = true;
            return false;
        }
    }

    /**
     * Access to the ftm serial console
     */
    public void ftmConsole() {
        try {
            FtmConsole n = new FtmConsole(_uid,_reader,recognizedType5Tag);
            n.interractiveCommand();
        } catch ( TagException e ) {
            Tools.log_on = true;
            return;
        }
    }

    public boolean ftmCommand(String cmd) {
        try {
            FtmConsole n = new FtmConsole(_uid,_reader,recognizedType5Tag);
            return n.executeCommand(cmd);
        } catch ( TagException e ) {
            Tools.log_on = true;
            return false;
        }
    }


}
