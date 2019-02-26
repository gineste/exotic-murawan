/* ==========================================================
 * configNvm.h - Application specific configuration file
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 21 févr. 2019
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

#ifndef INC_IT_SDK_CONFIGNVM_H_
#define INC_IT_SDK_CONFIGNVM_H_

// The configuration version
#define ITSDK_CONFIGURATION_APP_VERSION		ITSDK_USER_VERSION_BYTE

typedef enum __attribute__ ((__packed__)) {
	MURAWAN_ANTENNA_PIFA=0,
	MURAWAN_ANTENNA_PCB=1
} murawan_antenna_choice_e;


#define MURAWAN_CONFIG_TIME_BASE_S 			10		// Base of time in Second, use as unit for 	MURAWAN_CONFIG_SENDDUTY and others
#define MURAWAN_CONFIG_BATDUTY_TIME_BASE_M	10		// Base of time in Minutes for the BADDUTY

#define MURAWAN_CONFIG_SENDDUTY		3		// By default send new data every 3xMURAWAN_CONFIG_TIME_BASE_Ss
#define MURAWAN_CONFIG_ACKDUTY	   10		// Every 10x*(MURAWAN_CONFIG_ACKDUTY) => 5m ack is performed
#define MURAWAN_CONFIG_ACKRETRY		6		// 6 ACK retry ( 30m ) before failing (this is not LoRaWan retry)
#define MURAWAN_CONFIG_SLEEPDUTY   20		// 20*MURAWAN_CONFIG_SENDDUTY (10 min) time before trying to reconnect
#define MURAWAN_CONFIG_ANTENNA		MURAWAN_ANTENNA_PIFA
#define MURAWAN_CONFIG_BATDUTY	   12		// Every 12*10m = 2h send the battery status frame

/**
 * The application configuration specific fields are stored in this structure
 * The structure size must be 32b aligned.
 */
typedef struct {
	uint8_t							version;		// configuration version
	uint8_t							sendDuty;		// Period between two communications x10s
	uint8_t							ackDuty;		// Number of sendDuty period before requesting a ACK to verify Network connectivity x sendDuty
	uint8_t							ackRetry;		// Number of ackDuty retry before considering reconnection x ackDuty
	uint8_t							sleepDuty;		// Period between two connection try when disconnected x sendDuty
	uint8_t							batDuty;		// Period between two battery status communications x10m
	murawan_antenna_choice_e		antennaChoice;	// Select the antenna to transmit

	uint8_t							alignment[1];	// For 32bits size alignment
} itsdk_configuration_app_t;


#endif /* INC_IT_SDK_CONFIGNVM_H_ */
