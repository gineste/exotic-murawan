#include "libS8.h"
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define HEADAD	42
#define HEADSZ	1
#define HEADSZb	4
#define DATAAD	43
#define BUFSZ	84

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
	uint16_t	magic;
	uint8_t		hostRfu:4;	
	uint8_t		hostIsWritting:1;
	uint8_t		hostWriteDone:1;
	uint8_t		hostReadDone:1;

	uint8_t		mcuRfu:4;	
	uint8_t		mcuIsWritting:1;
	uint8_t		mcuWriteDone:1;
	uint8_t		mcuReadDone:1;
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

int __readBlocks(
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
	int _try = 0;
	*rlen=0;
	for ( int i = 0 ; i < blockNum ; i++ ) {
		while ( _try < 10 && (_ret=fw_readblock(icdev,flags,startBlock+i,1,uid,&_len,buffer+4*i)) != 0 ) {
			_try++;
		}
		if ( _try < 10 ) {
			*rlen+=_len;
		} else return _ret;
	}
	return 0;
}

int __read84Blocks(
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
	int _try = 0;
	*rlen=0;
	for ( int i = 0 ; i < blockNum ; i++ ) {
		while ( _try < 10 && (_ret=fw_readblock(icdev,flags,startBlock+i,1,uid,&_len,buffer+4*i)) != 0 ) {
			_try++;
		}
		if ( _try < 10 ) {
			*rlen+=_len;
			for ( int l = 0 ; l < 4 ; l++ ) {
				if ( buffer[4*i+l] == 0 ) {
					// end of line
					return 0;
				}
			}
		} else return _ret;
	}
	return 0;
}

void printHeader(header_t * header) {
	printf("magic (%04X) hostIsWr(%d) hostWrDone(%d) hostRdDone(%d) mcuIsWr(%d) mcuWrDone(%d) mcuRdDone(%d)\n",
		    	  	header->magic,
		    	  	header->hostIsWritting, 
		    	  	header->hostWriteDone,
		    	  	header->hostReadDone,
		    	  	header->mcuIsWritting,
		    	  	header->mcuWriteDone,
		    	  	header->mcuReadDone );
}

int __writeBlocks(
	int icdev, 
	unsigned char flags, 
	unsigned char startBlock,
	unsigned char blockNum,
	unsigned char * uid,
	unsigned char wlen,
	unsigned char * buffer
) {
	int _ret;
	int _try = 0;
	for ( int i = 0 ; i < blockNum ; i++ ) {
		while ( _try < 10 && (_ret=fw_writeblock(icdev,flags,startBlock+i,1,uid,4,buffer+4*i)) != 0 ) {
			_try++;
			usleep(100000);
		}
		if ( _try == 10 ) return _ret;
	}
	return 0;
}


void strupper(char * str) {
	while ( *str != 0 ) {
		*str = toupper(*str);
		str++;
	}
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
	    	unsigned char rBuffer[128];
	    	uint8_t countError = 0;

	    	while (1) {
		    	sleep(1);
		    	int ret;
		    	// reader header block 0
		    	if ( (ret=__readBlocks(
			    		icdev,
			    		0x22,
			    		HEADAD,		// block 32 => User land 2
			    		HEADSZ,		// 2 block for header entries
			    		&uid[1],	// Device address
			    		&rlen,		// Read size in byte
			    		(unsigned char*)&header	// where to store the result 
		    		  )) == 0 
		    	) {
		    	  printf("magic (%04X) hostIsWr(%d) hostWrDone(%d) hostRdDone(%d) mcuIsWr(%d) mcuWrDone(%d) mcuRdDone(%d)\n",
		    	  	header.magic,
		    	  	header.hostIsWritting, 
		    	  	header.hostWriteDone,
		    	  	header.hostReadDone,
		    	  	header.mcuIsWritting,
		    	  	header.mcuWriteDone,
		    	  	header.mcuReadDone );
		    	  int blockId = 0x20;	// first block of data
		    	  if ( header.magic != 0xCAFE ) break;
		    	  if ( header.mcuIsWritting == 0 ) {
		    	  	printf("# ");
		    	  	scanf("%s",rBuffer);
		    	  	int l = strlen(rBuffer);
		    	  	rBuffer[l]='\r';
		    	  	rBuffer[l+1]='\n';
		    	  	rBuffer[l+2]=0;
		    	  	l+=3;
		    	  	int lb = ((l & 3) == 0 )?l>>2:1+(l>>2);
		    	  	if ( l == 3 ) break;
		    	  	// change status
		    	  	header.hostIsWritting = 1;
		    	  	header.hostWriteDone = 0;
		    	  	header.mcuReadDone = 0;
		    	  	if ( (ret = __writeBlocks(icdev,0x22,HEADAD,HEADSZ,&uid[1],(unsigned char)HEADSZb,(unsigned char*)&header)) == 0 ) {
			    	  	// send the message
			    	  	if ( (ret = __writeBlocks(icdev,0x22,DATAAD,lb,&uid[1],(unsigned char)l,(unsigned char*)rBuffer )) == 0 ) {
			    	  		header.hostIsWritting = 0;
			    	  		header.hostWriteDone = 1;
			    	  		header.mcuWriteDone = 0;
				    	  	if ( (ret = __writeBlocks(icdev,0x22,HEADAD,HEADSZ,&uid[1],(unsigned char)HEADSZb,(unsigned char*)&header)) == 0 ) {
				    	  		// wait for response.
				    	  		//do {
								//	__readBlocks(icdev,0x22,HEADAD,HEADSZ,&uid[1],&rlen,(unsigned char*)&header);
									//printHeader(&header);
				    	  		//} while ( header.mcuReadDone == 0);
				    	  		int endof = 0;
				    	  		while (endof==0) {
					    	  		// wait for response
					    	  		int try=0;
					    	  		do {
										__readBlocks(icdev,0x22,HEADAD,HEADSZ,&uid[1],&rlen,(unsigned char*)&header);
										try++;
										//printHeader(&header);
					    	  		} while ( header.mcuWriteDone == 0 && try < 20 );
					    	  		if ( try == 20 ) {
					    	  			endof=1;
					    	  		} else {
						    	  		// get response
										__read84Blocks(icdev,0x22,DATAAD,BUFSZ/4,&uid[1],&rlen,(unsigned char*)rBuffer);
										header.hostReadDone=1;
										header.mcuWriteDone=0;
										__writeBlocks(icdev,0x22,HEADAD,HEADSZ,&uid[1],(unsigned char)HEADSZb,(unsigned char*)&header);
										printf("%s",rBuffer);
										strupper(rBuffer);
										if (    (rBuffer[0] == 'O' && rBuffer[1] == 'K')
											 || (rBuffer[0] == 'K' && rBuffer[1] == 'O')
										) endof = 1;
									}
								}
				    	  	}
			    	  	} else {
				    		printf("Write failed\n");
				    	}
		    	  	}
		    	  }
		    	} else {
		    	  	//printf("Error %X\n",ret);
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
