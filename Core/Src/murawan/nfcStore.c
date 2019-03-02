/* ==========================================================
 * nfcStore.c - Store data in the User1 NFC zone with a
 *             circular buffer
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
#include <drivers/nfc/st25dv/st25dv.h>
#include <it_sdk/logger/error.h>
#include <it_sdk/configError.h>
#include <murawan/nfcStore.h>

/**
 * Setup the NFC Store
 */
nfcStore_return_e nfcStore_setup() {
	nfcStore_header_t head;
	nfcStore_return_e ret = NFCSTORE_SUCCESS;

	drivers_st25dv_goWakeUp();
	// read the header
	if ( drivers_st25dv_blocRead(ST25DV_USERZONE_1, 0, (uint8_t*)&head, sizeof(head)) != ST25DV_SUCCESS ) {
		ITSDK_ERROR_REPORT(APP_ERROR_NFCSTORE_RDFAIL,0);
		ret = NFCSTORE_FAILED;
	} else {
		if ( head.magic != NFCSTORE_DATA_MAGIC || head.storeSize != NFCSTORE_AVAILABLE_ENTRIES || head.entrySize != NFCSTORE_DATA_ENTRY_SIZE) {
			// need to init the nfcStore
			head.magic = NFCSTORE_DATA_MAGIC;
			head.entriesStored = 0;
			head.writePointer = 0;
			head.entrySize = NFCSTORE_DATA_ENTRY_SIZE;
			head.storeSize = NFCSTORE_AVAILABLE_ENTRIES;
			if ( drivers_st25dv_blocWrite(ST25DV_USERZONE_1, 0, (uint8_t*)&head, sizeof(head)) != ST25DV_SUCCESS ) {
				ITSDK_ERROR_REPORT(APP_ERROR_NFCSTORE_WRFAIL,0);
				ret = NFCSTORE_FAILED;
			}
		}
	}
	drivers_st25dv_goLowPower();
	return ret;
}

nfcStore_return_e nfcStore_pushDataEntry(nfcStore_data_entry_t * entry) {
	// read the header
	nfcStore_header_t head;
	nfcStore_return_e ret = NFCSTORE_SUCCESS;
	drivers_st25dv_goWakeUp();

	if ( drivers_st25dv_blocRead(ST25DV_USERZONE_1, 0, (uint8_t*)&head, sizeof(head)) != ST25DV_SUCCESS ) {
		ITSDK_ERROR_REPORT(APP_ERROR_NFCSTORE_RDFAIL,0);
		ret = NFCSTORE_FAILED;
	} else {
		if ( drivers_st25dv_blocWrite(ST25DV_USERZONE_1, (head.writePointer+1)*head.entrySize, (uint8_t*)entry, sizeof(nfcStore_data_entry_t)) != ST25DV_SUCCESS ) {
			ITSDK_ERROR_REPORT(APP_ERROR_NFCSTORE_WRFAIL,0);
			ret = NFCSTORE_FAILED;
		} else {
			head.writePointer++;
			if ( head.entriesStored < head.storeSize ) head.entriesStored++;
			if ( head.writePointer >= head.storeSize ) head.writePointer = 0;
			if ( drivers_st25dv_blocWrite(ST25DV_USERZONE_1, 0, (uint8_t*)&head, sizeof(head)) != ST25DV_SUCCESS ) {
				ITSDK_ERROR_REPORT(APP_ERROR_NFCSTORE_WRFAIL,0);
				ret = NFCSTORE_FAILED;
			}
		}
	}
	drivers_st25dv_goLowPower();
	return ret;
}
