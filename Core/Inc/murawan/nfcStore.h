/* ==========================================================
 * nfcStore.h - NFC Memory storage
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 2 mars 2019
 *     Author: Paul Pinault aka Disk91
 * ----------------------------------------------------------
 * Copyright (C) 2019 Disk91
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU LESSER General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * ----------------------------------------------------------
 * 
 *
 * ==========================================================
 */

#ifndef INC_MURAWAN_NFCSTORE_H_
#define INC_MURAWAN_NFCSTORE_H_

#include <stdint.h>

#define NFCSTORE_DATA_MAGIC			0x4449534B
#define NFCSTORE_DATA_ENTRY_SIZE	3															    // size of the entry in block of 32b
#define NFCSTORE_DATA_SIZE			128
#define NFCSTORE_AVAILABLE_ENTRIES	((NFCSTORE_DATA_SIZE / (4*NFCSTORE_DATA_ENTRY_SIZE)) -1)  // Number of block we can store in the Zone 1

// The block Header is on block starting a offset of the USER1 zone
// The first entry block will be at entrySize*blockId
typedef struct {
  uint32_t	magic;				// Just to see if already setup.
  uint8_t	entriesStored;		// Number of entry in the nfcStore
  uint8_t 	writePointer;
  uint8_t	storeSize;			// size of the store in entry blocks
  uint8_t	entrySize;			// size of an entry in NFC block (32b)
} nfcStore_header_t;

typedef struct {
  int16_t	temp;		// 1 block		- in c°C
  uint16_t	pressure;	//				- in hPa
  uint16_t	light;		// 2 block 		- in Lux
  uint16_t	humidity;	//				- in %age
  uint8_t	vcell1;		// 3 block		- in 0.1V
  uint8_t	vcell2;		//				- in 0.1V
  uint8_t	vcell3;		//				- in 0.1V
  uint8_t	vbat;		//				- in 0.1V
} nfcStore_data_entry_t;

typedef enum {
   NFCSTORE_SUCCESS = 0,
   NFCSTORE_FAILED,

} nfcStore_return_e;

nfcStore_return_e nfcStore_pushDataEntry(nfcStore_data_entry_t * entry);
nfcStore_return_e nfcStore_setup();


#endif /* INC_MURAWAN_NFCSTORE_H_ */
