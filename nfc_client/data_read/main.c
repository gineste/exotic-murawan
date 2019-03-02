#include "libS8.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
/**
 * Some Notes :
 *     Flags =>  0x22 => Addressed
 *               0x02 => UnAddressed
 *               0x42 => UnAddressed with option flag
 *               0x62 => Addressed with option flag
 *		Status => 0x00 => OK
 *                0x0101 => Cmd not supported
 *				  0x0102 => Not recognised
 *				  0x0103 => Option not supported
 *				  0x010F => Unknow error
 *				  0x0110 => Block not available
 * 				  0x0111 => Block already locked
 *				  0x0112 => Block already locked, content can't be changed
 * 				  0x0113 => Programmind was unsuccessful
 *				  0x0114 => Locking / Kill was unsuccessful
 *				  0x0115 => Block protected in read
 *				  0x01A1 => Start block must be even
 *				  0x01A2 => On or both blocks already loacked
 *				  0x01B0 => Read Access denied
 */

typedef struct {
	uint32_t	magic;
	uint8_t		entriesStored;	
	uint8_t		writePointer;
	uint8_t		storeSize;
	uint8_t		entrySize;
} header_t;

typedef struct {
	int16_t		temp;
	uint16_t	pressure;
	uint16_t	light;
	uint16_t	humidity;
	uint8_t		vcell1;
	uint8_t		vcell2;
	uint8_t		vcell3;
	uint8_t		vbat;
} entry_t;

int readBlocks(
	int icdev, 
	unsigned char flags, 
	unsigned char startBlock,
	unsigned char blockNum,
	unsigned char * uid,
	unsigned char * rlen,
	unsigned char * buffer
) {
	uint8_t _len;
	int _ret;
	*rlen=0;
	for ( int i = 0 ; i < blockNum ; i++ ) {
		if ( (_ret=fw_readblock(icdev,flags,startBlock+i,1,uid,&_len,buffer+4*i)) != 0 ) {
			return _ret;
		} else {
			*rlen+=_len;
		}
	}
	return 0;
}

int main(int argc,char* argv[])
{
	int st,number;
	char szNode[128];

	// open the NFC reader from USB port
	int icdev;
	do {
		char szVer[128];
		int iNode = 0;
		sprintf(szNode, "/dev/usb/hiddev%d", iNode);
		iNode++;
		if((icdev=fw_init_ex(2,szNode,115200))== -1) {
			printf("NFC reader not found\n");
			return 0;
		} else{
			st = fw_getver(icdev, szVer);
			if( st ==0 ) {
			 	printf("NFC reader found on %s\n",szNode);
			 	printf("Version %s\n",szVer);
			 	printf("icdev %d\n",icdev);
			} else { 
				printf("%s init error\n",szNode);
			}
		}
	} while(icdev == -1);

	// Init success, make a beep... we love beep     
    fw_beep(icdev,10);
   	fw_config_card(icdev, 0x31);	// configure for ISO-15693
    do {
	    // Search for card
	    int found = 0;
	    unsigned char rlen=0;
	    unsigned char uid[9];
	    unsigned char security[128];
	   	while ( !found ) {
			st= fw_inventory(
			   icdev,
			   0x36,		 // find single card
			   0,			 // Application ID
			   0,			 // Masklen
			   &rlen,		 // Len of received data
			   uid			 // Received Data : DSFID + 8B UID
			);       
			if ( ! st ) {
				// Card found
				printf("Found NFC device DSFID [ %02X ] UID [ ",uid[0]);
				for (int i = 0 ; i < 8 ; i++) printf("%02X",uid[i+1]);
				printf(" ]\n");
			    found=1;
			} else {
				sleep(2);	 // retry later
			}
	   	}
	    fw_beep(icdev,10);
	    if ( fw_select_uid(icdev,0x22,&uid[1]) != 0 ) {
	    	printf("Impossible to select the device\n");
	    } else {
	    	header_t header;
	    	uint8_t rlen=0;
	    	unsigned char rBuffer[64];
	    	uint8_t countError = 0;
	    	while (1) {
		    	sleep(1);
		    	uint8_t ret;
		    	// reader header block 0
		    	if ( (ret=readBlocks(
			    		icdev,
			    		0x22,
			    		0x0,		// block 0
			    		3,			// 3 block for Data entries
			    		&uid[1],	// Device address
			    		&rlen,		// Read size in byte
			    		(unsigned char*)&header	// where to store the result 
		    		  )) == 0 
		    	) {
		    	  printf("magic (%08X) size(%d) entrySz(%d) entries(%d) wrPointer(%d)\n",
		    	  	header.magic,
		    	  	header.storeSize, 
		    	  	header.entrySize,
		    	  	header.entriesStored,
		    	  	header.writePointer );
		    	  int blockId = header.writePointer-1;
		    	  entry_t entry;
		    	  for ( int i = 0 ; i < header.entriesStored ; i++ ) {
		    	  	if ( blockId < 0 ) blockId = header.storeSize-1;
		    	  	int blockAdr = (blockId+1)*header.entrySize; 	
		    	  	//printf("adr (%X) ",blockAdr);
		    	  	if ( (ret=readBlocks(
		    	  			icdev,
		    	  			0x22,
		    	  			blockAdr,
		    	  			header.entrySize,
		    	  			&uid[1],
		    	  			&rlen,
		    	  			(unsigned char*)&entry
		    	  		)) == 0 ) {
		    	  		printf("%d - temp (%3.2f°C) pressure (%dhPa) light (%dLux) humidity (%d%) - V(%2.1fV - %2.1fV %2.1fV %2.1fV)\n",
		    	  			i,
		    	  			entry.temp/100.0,
		    	  			entry.pressure,
		    	  			entry.light,
		    	  			entry.humidity,
		    	  			entry.vbat/10.0,
		    	  			entry.vcell1/10.0,
		    	  			entry.vcell2/10.0,
		    	  			entry.vcell3/10.0
		    	  		);
			    	  	blockId--;
		    	  	} else {
		    	  		countError++;
		    	  		if ( countError < 4 ) {
		    	  			sleep(1);
		    	  			i--;
		    	  			continue;
		    	  		} else {
				    	  	printf("Error %X\n",ret);
			    	  		break;
			    	  	}
		    	  	}
		    	  }
		    	  if ( ret != 0 ) {
		    	  	break;
		    	  }

		    	} else {
		    		countError++;
		    		if ( countError > 3) {
		    			break;
		    		}
		    	}
		    }
	    }
	    sleep(10);

	} while (1);	
    fw_exit(icdev);
    return 0; 
}
