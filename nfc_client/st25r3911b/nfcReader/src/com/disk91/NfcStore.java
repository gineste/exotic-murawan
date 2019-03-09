package com.disk91;

import com.st.st25pc.model.readers.RFGenericReader;
import com.st.st25sdk.STException;
import com.st.st25sdk.type5.ReadBlockResult;
import com.st.st25sdk.type5.Type5Tag;

public class NfcStore {

    private byte [] _uid;
    private RFGenericReader _reader;
    private Type5Tag _tag;
    private int _offset;
    private Header _h;

    protected class Header {
        int magic;
        int entriesStored;
        int writePointer;
        int storeSize;
        int entrySize;

        public void initFromByteStream(byte [] b) {
            if ( b.length != 12 ) return;
            magic  = Tools.ubyte(b[3]) << 24;
            magic += Tools.ubyte(b[2]) << 16;
            magic += Tools.ubyte(b[1]) << 8;
            magic += Tools.ubyte(b[0]);
            entriesStored = Tools.ubyte(b[4]);
            writePointer = Tools.ubyte(b[5]);
            storeSize = Tools.ubyte(b[6]);
            entrySize = Tools.ubyte(b[7]);
        }

        public byte[] getByteStream() {
            byte[] b = new byte[12];
            b[3] = Tools.toByte((magic >> 24) & 0xFF);
            b[2] = Tools.toByte((magic >> 16) & 0xFF);
            b[1] = Tools.toByte((magic >>  8) & 0xFF);
            b[0] = Tools.toByte((magic >>  0) & 0xFF);
            b[4] = Tools.toByte(entriesStored);
            b[5] = Tools.toByte(writePointer);
            b[6] = Tools.toByte(storeSize);
            b[7] = Tools.toByte(entrySize);
            for (int i = 8 ; i < 12 ; i++) b[i] = 0;
            return b;
        }

    }

    protected class Entry {
        int temp;
        int pressure;
        int light;
        int humidity;
        int vcell1;
        int vcell2;
        int vcell3;
        int vbat;

        public void initFromByteStream(byte [] b) {
            if ( b.length != 12 ) return;
            temp = Tools.ubyte(b[1]) << 8;
            temp += Tools.ubyte(b[0]);
            if ( temp >= 32768 ) temp -= 65536;
            pressure = Tools.ubyte(b[3]) << 8;
            pressure += Tools.ubyte(b[2]);
            light = Tools.ubyte(b[5]) << 8;
            light += Tools.ubyte(b[4]);
            humidity = Tools.ubyte(b[7]) << 8;
            humidity += Tools.ubyte(b[6]);
            vcell1 += Tools.ubyte(b[8]);
            vcell2 += Tools.ubyte(b[9]);
            vcell3 += Tools.ubyte(b[10]);
            vbat += Tools.ubyte(b[11]);
        }

    }

    public NfcStore (
            byte [] uid,
            RFGenericReader reader,
            Type5Tag tag,
            int offset
    ) throws TagException {
        this._uid = uid;
        this._reader = reader;
        this._tag = tag;
        this._offset = offset;

        // get header bloc
        try {
            Tools.log_on=false;
            ReadBlockResult r = tag.readBlocks(offset, 3);
            Tools.log_on=true;
            _h = new Header();
            _h.initFromByteStream(r.data);
            System.out.println("Found nfcStore : " +
                    "magic("+String.format("%04X",_h.magic)+") " +
                    "size("+_h.storeSize+") " +
                    "entrySz("+_h.entrySize+") " +
                    "entries("+_h.entriesStored+") " +
                    "wrPointer("+_h.writePointer+")");

            int blockId=_h.writePointer-1;
            for ( int i = 0 ; i < _h.entriesStored ; i++ ) {
                if ( blockId < 0 ) blockId = _h.storeSize-1;
                int blockAdr = offset+(blockId+1)*_h.entrySize;
                Tools.log_on=false;
                ReadBlockResult re = tag.readBlocks(blockAdr, 3);
                Tools.log_on=true;
                Entry e = new Entry();
                e.initFromByteStream(re.data);
                System.out.println("" + String.format("%02d",i) + " - "+
                        "T ("+String.format("%3.2fC",e.temp/100.0)+") " +
                        "P ("+e.pressure+"hPa) " +
                        "L ("+e.light+"Lux) " +
                        "H ("+e.humidity+"%) " +
                        "- " +
                        "V("+String.format("%2.1f",e.vbat/10.0)+"V" +
                        " - "+String.format("%2.1f",e.vcell1/10.0)+"V" +
                        " - "+String.format("%2.1f",e.vcell2/10.0)+"V" +
                        " - "+String.format("%2.1f",e.vcell3/10.0)+"V" +
                        ")"
                );
                blockId--;
            }

        } catch (STException e) {
            throw new TagException();
        }
    }



}
