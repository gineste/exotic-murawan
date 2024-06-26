/* ==========================================================
 * configError.h - 
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 17 f�vr. 2019
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
#ifndef INC_IT_SDK_CONFIGERROR_H_
#define INC_IT_SDK_CONFIGERROR_H_

#include <it_sdk/logger/error.h>

// Format for any error code:
// +----+---+---+--------------------+----------------+
// | XX | Y | Z | AAAA AAAA AAAA AAAA| KKKK KKKK KKKK |
// +----+---+---+--------------------+----------------+
//
// XX -> Level : 00 - Info / 01 - WARN / 10 - ERROR / 11 - FATAL
// Y  -> Category : 0 - SDK / 1 - Application
// Z  -> Value : 0 - No associated value / 1 - Associated value
// AAAA .... AAAA -> 16b associated value
// KKKK .... KKKK -> 12b error code


							// Error code under WWW symbol
								//  _______WWW
#define	APP_ERROR_EXAMPLE			0x00000001 | (ITSDK_ERROR_LEVEL_INFO  | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE)
#define APP_ERROR_BME280_FAULT  	0x00000002 | (ITSDK_ERROR_LEVEL_FATAL | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE)
#define APP_ERROR_MAX44009_FAULT  	0x00000003 | (ITSDK_ERROR_LEVEL_FATAL | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE)
#define APP_ERROR_LORA_DISCONNECT  	0x00000004 | (ITSDK_ERROR_LEVEL_INFO  | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE)
#define APP_ERROR_ST25DV_FAULT  	0x00000005 | (ITSDK_ERROR_LEVEL_FATAL | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE)
#define APP_ERROR_MAX17205_FAULT  	0x00000006 | (ITSDK_ERROR_LEVEL_FATAL | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE)
#define APP_ERROR_NFCSTORE_RDFAIL  	0x00000007 | (ITSDK_ERROR_LEVEL_ERROR | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE)
#define APP_ERROR_NFCSTORE_WRFAIL  	0x00000008 | (ITSDK_ERROR_LEVEL_ERROR | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE)
#define APP_ERROR_MURAWAN_DOWNLINK 	0x00000009 | (ITSDK_ERROR_LEVEL_INFO  | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE) // Downlink received
#define APP_ERROR_MURAWAN_INVDLNK 	0x0000000A | (ITSDK_ERROR_LEVEL_WARN  | ITSDK_ERROR_TYPE_APP | ITSDK_ERROR_WOUT_VALUE) // Invalid Downlink received

#endif /* INC_IT_SDK_CONFIGERROR_H_ */
