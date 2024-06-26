/* ==========================================================
 * configSigfox.h - SDK Configuration file for Sigfox communication
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 4 nov. 2018
 *     Author: Paul Pinault aka Disk91
 * ----------------------------------------------------------
 * Copyright (C) 2018 Disk91
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
 * S2LP:
 *    In cube MX declare the CSn and SDN pin as output.
 *      CSn is pullup set to High initially
 *      SDN is set to low initially (no shutdown)
 *    Declare SPI. default configuration normally match.
 *
 * ==========================================================
 */

#ifndef IT_SDK_CONFIG_SIGFOX_H_
#define IT_SDK_CONFIG_SIGFOX_H_

#include <it_sdk/config_defines.h>
#if ITSDK_LORAWAN_LIB == __LORAWAN_SX1276
	#include "spi.h"
#endif

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// | SDK SETTING                   | USER SELECTED VALUE                  | SETTING DESCRIPTION                   |
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +------------LORAWAN------------|--------------------------------------|---------------------------------------|
#if ITSDK_WITH_LORAWAN_LIB == __ENABLE

#define ITSDK_LORAWAN_STACK			__LORAWAN_SEMTECH					   // Select the SEMTECH lorawan stack
#define ITSDK_LORAWAN_ACTTYPE		__LORAWAN_ACTIVATION_DYNAMIC		   // Activation type selected at compilation time
#define ITSDK_LORAWAN_ACTIVATION	__LORAWAN_OTAA						   // Activation OTAA or ABP
#define ITSDK_LORAWAN_NETWORKTYPE   __LORAWAN_NWK_PUBLIC				   // type of network - public / private
#define ITSDK_LORAWAN_DEVEUI_SRC	__LORAWAN_DEVEUI_STATIC				   // Device EUI is static / generated
#define ITSDK_LORAWAN_ADR			__LORAWAN_ADR_OFF					   // Adaptative Data Rate
#define ITSDK_LORAWAN_DEFAULT_DR	__LORAWAN_DR_0						   // Default data rate (SF12 125KHz)
#define ITSDK_LORAWAN_CNF_RETRY		2									   // Number of retry when Confirm mode is applied
#define ITSDK_LORAWAN_MAX_RX_ERROR	0									   // Extends the RX windows with +/- MS
																		   // This is for unprecised clocks (0=default value)
#define ITSDK_LORAWAN_MAX_DWNLNKSZ	32									   // Max downlink Size in Byte for reception buffer

																		   // =============================
																		   // FREQUENCY MAPPING
																		   // =============================
#define ITSDK_LORAWAN_REGION_ALLOWED (								\
										__LORAWAN_REGION_EU868	|	\
										__LORAWAN_REGION_AS923	|	\
										__LORAWAN_REGION_AU915	|	\
										__LORAWAN_REGION_CN470	|	\
										__LORAWAN_REGION_CN779	|	\
										__LORAWAN_REGION_EU433	|	\
										__LORAWAN_REGION_KR920	|	\
										__LORAWAN_REGION_IN865	|	\
										__LORAWAN_REGION_US915	|	\
										__LORAWAN_REGION_RU864	|	\
										__LORAWAN_REGION_NONE		\
									 )										// List of zone to be available as part
																			// of the code compilation. Size optimization

																		   // =============================
																		   // OTAA
																		   // =============================

#define ITSDK_LORAWAN_DEVEUI		{													\
										0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	\
									}									   // Static DEVEUI (big endian)
																		   // Bytes are in the same order as
																		   // in a String.

#define ITSDK_LORAWAN_APPEUI		{													\
										0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x01, 0x7C, 0x93	\
									}									   // Static APPEUI (big endian)
																		   // Bytes are in the same order as
																		   // in a String.

#define ITSDK_LORAWAN_APPKEY		{													\
										0xC9, 0xEA, 0x62, 0xF5, 0xAE, 0xD3, 0x9D, 0xEC, \
										0x5B, 0xFD, 0x51, 0xAA, 0xEE, 0x3E, 0x96, 0x77  \
									}									   // Static APPKEY (byte[])
																		   // Bytes are in the same order compared
																		   // as in a String.


#define ITSDK_LORAWAN_NWKKEY		{													\
										0x00, 0x00, 0x00, 0x00, 0x87, 0xC9, 0x2F, 0x47, \
										0x17, 0xEE, 0xF7, 0x06, 0xAC, 0x09, 0x5D, 0xD8  \
									}									   // Static NETWORK KEY (byte[])
																		   // Bytes are in the same order compared
																		   // as in a String.
																		   // Same as APPKEY for OTAA


																		   // =============================
																		   // ABP
																		   // =============================
#define ITSDK_LORAWAN_FNWKSKEY		{													\
										0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,	\
										0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C	\
									}									   // Static FORWARDING NETWORK sKEY (byte[])
																		   // Bytes are in the same order compared
																		   // as in a String.

#define ITSDK_LORAWAN_SNWKSKEY		{													\
										0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,	\
										0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C	\
									}									   // Static SERVING NETWORK sKEY (byte[])
																		   // Bytes are in the same order compared
																		   // as in a String.

#define ITSDK_LORAWAN_NWKSKEY		{													\
										0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,	\
										0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C	\
									}									   // Static NETWORK SESSION ENCRYPT KEY (byte[])
																		   // Bytes are in the same order compared
																		   // as in a String.
#define ITSDK_LORAWAN_APPSKEY		{													\
										0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, \
										0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C	\
									}									   // Static APPLICATION SESSION ENCRYPT KEY (byte[])
																		   // Bytes are in the same order compared
																		   // as in a String.

#define ITSDK_LORAWAN_NETWORKID		( uint32_t )0x00000000				   // 32b Network ID
#define ITSDK_LORAWAN_DEVADDR		( uint32_t )0x00000000				   // 32b Session Device address

																		   // =============================
																		   // CHANNEL STATIC CONFIG
																		   // =============================

#define ITSDK_LORAWAN_CHANNEL {										\
								5,		/*num of channel*/			\
								{									\
									{	3, 867100000, 867100000, __LORAWAN_DR_0, __LORAWAN_DR_5, 0 }, \
									{	4, 867300000, 867300000, __LORAWAN_DR_0, __LORAWAN_DR_5, 0 }, \
									{	5, 867500000, 867500000, __LORAWAN_DR_0, __LORAWAN_DR_5, 0 }, \
									{	6, 867700000, 867700000, __LORAWAN_DR_0, __LORAWAN_DR_5, 0 }, \
									{	7, 867900000, 867900000, __LORAWAN_DR_0, __LORAWAN_DR_5, 0 }  \
								}									\
							  }												// TTN EU Configuration

																		   // =============================
																		   // ITSDK E2E ENCRYPTION STATIC CONFIG
																		   // =============================

#define ITSDK_LORAWAN_ENCRYPTION		( /* __PAYLOAD_ENCRYPT_NONE | */ \
										    __PAYLOAD_ENCRYPT_AESCTR \
									      | __PAYLOAD_ENCRYPT_SPECK  \
									    )									// Encryption code activated
#define ITSDK_LORAWAN_AES_SHAREDKEY	( 0xB1EF5436 ^ ITSDK_PROTECT_KEY )      //
																			// Shared Key for CTR generation
#define ITSDK_LORAWAN_AES_INITALNONCE ( 0x42 )								//
																			// Initial value for Nonce used for AES128-CRT
#define ITSDK_LORAWAN_SPECKKEY		(   (uint64_t)0xCD3F248BC60A5D27  \
									  ^ (  (uint64_t)ITSDK_PROTECT_KEY \
									     | ((uint64_t)ITSDK_PROTECT_KEY << 32)) \
									)										//
																			// Shared Key for SPECK32/64 Encryption
#define ITSDK_LORAWAN_AES_MASTERKEYH (   (uint64_t)0xAEF4371B53CD7903  \
									  ^ (  (uint64_t)ITSDK_PROTECT_KEY \
									     | ((uint64_t)ITSDK_PROTECT_KEY << 32)) \
									 )										//
																			// Static 16B key used as master key (8B HIGH)
																			// for end to end encryption
#define ITSDK_LORAWAN_AES_MASTERKEYL (   (uint64_t)0x3426E4286BA5CE0C  \
									  ^ (  (uint64_t)ITSDK_PROTECT_KEY \
									     | ((uint64_t)ITSDK_PROTECT_KEY << 32)) \
									 )										//
																		    // Static 16B key used as master key (8B LOW)
																		    // for end to end encryption

#endif
// +-------------SX1276------------|--------------------------------------|---------------------------------------|
#if ITSDK_LORAWAN_LIB == __LORAWAN_SX1276
#define ITSDK_SX1276_SPI			hspi1								   // SPI To BE USED
																		   // SPI Configured by CubeMX
																		   //  Master, 8b, Polarity Low,
																		   //  first bit msb, CRC disable...

#define ITSDK_SX1276_RESET_BANK		 __BANK_C							   // SX1276 GPIO FOR RESET
#define ITSDK_SX1276_RESET_PIN		 __LP_GPIO_0
#define ITSDK_SX1276_NSS_BANK		 __BANK_A							   // SX1276 GPIO FOR Activating Low Slave Select NSS
#define ITSDK_SX1276_NSS_PIN		 __LP_GPIO_15
#define ITSDK_SX1276_TCXO_VCC_BANK	 __BANK_A							   // SX1276 GPIO FOR Activating TCXO
#define ITSDK_SX1276_TCXO_VCC_PIN	 __LP_GPIO_12
#define ITSDK_SX1276_DIO_0_BANK	 	 __BANK_B							   // SX1276 GPIO 0 => Rx/Tx Done interrupt
#define ITSDK_SX1276_DIO_0_PIN	 	 __LP_GPIO_4
#define ITSDK_SX1276_DIO_1_BANK	 	 __BANK_B							   // SX1276 GPIO 1 => Rx Timeout interrupt
#define ITSDK_SX1276_DIO_1_PIN	 	 __LP_GPIO_1
#define ITSDK_SX1276_DIO_2_BANK	 	 __BANK_B							   // SX1276 GPIO 2 => Fhss Change channel
#define ITSDK_SX1276_DIO_2_PIN	 	 __LP_GPIO_0
#define ITSDK_SX1276_DIO_3_BANK	 	 __BANK_C							   // SX1276 GPIO 3 => cadDone/ValidHeader/Crc Error
#define ITSDK_SX1276_DIO_3_PIN	 	 __LP_GPIO_13
#define ITSDK_SX1276_DIO_4_BANK	 	 __BANK_A							   // SX1276 GPIO 4 => CAD Detected / Pll lock
#define ITSDK_SX1276_DIO_4_PIN	 	 __LP_GPIO_5
#define ITSDK_SX1276_DIO_5_BANK	 	 __BANK_A							   // SX1276 GPIO 5 => Mode Ready / Clk out
#define ITSDK_SX1276_DIO_5_PIN	 	 __LP_GPIO_4
																		   // Corresponding Interrupts needs to
																		   // have an interrupt handler configured.
																		   // EXTI activated for Pins in CubeMW for STM32
																		   // And corrsponding PIN add in the Low level Handler (stm32L...it.c)

// +-------------MURATA------------|--------------------------------------|---------------------------------------|

#define ITSDK_MURATA_ANTSW_RX_BANK 	     __BANK_A						   // MURATE ANTENNA SWITCH RX
#define ITSDK_MURATE_ANTSW_RX_PIN	     __LP_GPIO_1
#define ITSDK_MURATA_ANTSW_TXBOOST_BANK  __BANK_C						   // MURATA TX POWER BOOST
#define ITSDK_MURATE_ANTSW_TXBOOST_PIN	 __LP_GPIO_1
#define ITSDK_MURATA_ANTSW_TXRFO_BANK    __BANK_C						   // MURATA TX RFO
#define ITSDK_MURATE_ANTSW_TXRFO_PIN	 __LP_GPIO_2


#endif //__LORAWAN_SX1276

// +-------------OTHERS------------|--------------------------------------|---------------------------------------|

// Some config verifications
#if (ITSDK_TIMER_SLOTS < 3) && (ITSDK_LORAWAN_LIB == __LORAWAN_SX1276)
#error "You need 3 Timer or more to support LoRaWan Stack"
#endif


#endif /* IT_SDK_CONFIG_SIGFOX_H_ */

