/* ==========================================================
 * machine.c - The state machine
 * Project : Murawan
 * ----------------------------------------------------------
 * Created on: 13 févr. 2019
 *     Author: Paul Pinault aka Disk91
 * ----------------------------------------------------------
 * Copyright (C) 2019 Exotic Systems
 * ----------------------------------------------------------
 * 
 *
 * ==========================================================
 */
#include <it_sdk/itsdk.h>
#include <it_sdk/time/time.h>
#include <it_sdk/logger/logger.h>
#include <it_sdk/logger/error.h>
#include <it_sdk/statemachine/statemachine.h>
#include <it_sdk/lorawan/lorawan.h>
#include <it_sdk/encrypt/encrypt.h>
#include <it_sdk/eeprom/eeprom.h>
#include <it_sdk/eeprom/sdk_config.h>

#include <drivers/temphygropressure/bosh_bme280/bme280.h>
#include <drivers/light/max44009/max44009.h>
#include <drivers/nfc/st25dv/st25dv.h>
#include <drivers/gauge/max17205/max17205.h>
#include <it_sdk/lorawan/cayenne.h>

#include <murawan/machine.h>

machine_t murawan_stm = {
	MURAWAN_ST_SETUP,			// currentState
    LOOP_INIT_VALUE, 			// loopCounter
    STATE_UNKNOWN,				// lastState
    TOTAL_LOOP_INIT_VALUE,		// totalLoop
    &murawan_stm_updateTiming,	// Update age each time the machine is running - on each cycles.
    {
      //       UID                RESET       PROCE      			 P0      Name:XXXXXXX
        { MURAWAN_ST_SETUP, 	  NULL,	    &murawan_stm_stSetup,    NULL,   	"Setup"  },
        { MURAWAN_ST_WAIT4CONF,   NULL,     &murawan_stm_stWaitC,    NULL,   	"Wait"   },
        { MURAWAN_ST_RUN, 	 	  NULL,     &murawan_stm_stRun,   	 NULL,   	"Run"    },
        { MURAWAN_ST_SEND, 	 	  NULL,     &murawan_stm_stSend,   	 NULL,   	"Send"   },
        { MURAWAN_ST_JOIN, 	 	  NULL,     &murawan_stm_stJoin,   	 NULL,   	"Join"   },
        { STATE_LAST,	 		  NULL,     NULL,  					 NULL,      "Error"  }

    }
};

/**
 * On every machine cycle, call the timing update;
 * The precision is 1S so the scheduler period should not be lower than 1s
 */
void murawan_stm_updateTiming() {
	uint64_t now = itsdk_time_get_ms();
	uint32_t durationS =(uint32_t)((now - murawan_state.lastTimeUpdateMs)/1000);
	murawan_state.lastTimeUpdateMs += (durationS*1000);
	murawan_state.lastAckTestS += durationS;
	murawan_state.lastConnectTryS += durationS;
	murawan_state.lastMeasureS += durationS;
}

/** ***************************************************************************************
 * Setup step, here we are configuring the hardware and software stuff
 */
uint16_t murawan_stm_stSetup(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
	log_info("In Setup %d,%d\r\n",cLoop,tLoop);

	murawan_setup();

	static itsdk_lorawan_channelInit_t channels= ITSDK_LORAWAN_CHANNEL;
	#ifdef ITSDK_LORAWAN_CHANNEL
		itsdk_lorawan_setup(__LORAWAN_REGION_EU868,&channels);
	#else
		itsdk_lorawan_setup(__LORAWAN_REGION_EU868,NULL);
	#endif

	// setup sensors
	if (drivers_bme280_setup(BME280_MODE_WEATHER_MONITORING) != BME280_SUCCESS ) {
		ITSDK_ERROR_REPORT(APP_ERROR_BME280_FAULT,0);
	}
	if (drivers_max44009_setup(MAX44009_MODE_ONREQUEST) != MAX44009_SUCCESS ) {
		ITSDK_ERROR_REPORT(APP_ERROR_MAX44009_FAULT,0);
	}

	// setup Gauge
	switch (drivers_max17205_setup(MAX17205_MODE_3CELLS_INT_TEMP) ) {
	case MAX17205_SUCCESS:
	case MAX17205_UNDERVOLT:
		break;
	default:
		ITSDK_ERROR_REPORT(APP_ERROR_MAX17205_FAULT,0);
		break;
	}

	// setup NFC
	if (drivers_st25dv_setup(ST25DV_MODE_DEFAULT) != ST25DV_SUCCESS ) {
		ITSDK_ERROR_REPORT(APP_ERROR_ST25DV_FAULT,0);
	}


	// Check is the device has been initialized of if still factory default (should be 00,00,00...)
	uint8_t devEui[8];
	itsdk_lorawan_getDeviceEUI(devEui);
	uint8_t i = 0;
	while ( i < 8 && devEui[i] == 0 ) i++;
	if  ( i == 8 ) {
		// The device is not yet initialized
		return MURAWAN_ST_WAIT4CONF;
	} else {
		// Ready to go
		return (MURAWAN_ST_JOIN | STATE_IMMEDIATE_JUMP);
	}

}


/** ***************************************************************************************
 * Until the configuration of the deviceId is correct we stay in the stWait
 */
uint16_t murawan_stm_stWaitC(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {

	// Assuming the more convinient way is to reset after configuration
	return MURAWAN_ST_WAIT4CONF;
}



/** ***************************************************************************************
 * Central decision loop / capture data and manage transmission
 */
uint16_t murawan_stm_stRun(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {


	uint16_t v;
	if ( drivers_max17205_isReady() == MAX17205_SUCCESS ) {
		drivers_max17205_getVoltage(MAX17205_VBAT,&v);
		murawan_state.lastvBatmV = v;
		drivers_max17205_getVoltage(MAX17205_CELL3,&v);
		murawan_state.lastCell3mV = v;
		drivers_max17205_getVoltage(MAX17205_CELL2,&v);
		murawan_state.lastCell2mV = v;
		drivers_max17205_getVoltage(MAX17205_CELL1,&v);
		murawan_state.lastCell1mV = v;
		int32_t t;
		drivers_max17205_getTemperature(&t);
		drivers_max17205_getCurrent(&t);
		murawan_state.lastCurrent = -t;
		drivers_max17205_getCoulomb(&v);
		murawan_state.lastCoulomb = v;
	}


	// Get values when sendDuty time has been reached
	if ( murawan_state.lastMeasureS >= itsdk_config.app.sendDuty*MURAWAN_CONFIG_TIME_BASE_S ) {
		log_info("time is %d\r\n",(uint32_t)itsdk_time_get_ms());

		murawan_state.lastMeasureS = 0;
		if ( drivers_bme280_getSensors(&murawan_state.lastTemp,&murawan_state.lastPressure,&murawan_state.lastHumidity) != BME280_SUCCESS ) {
			ITSDK_ERROR_REPORT(APP_ERROR_BME280_FAULT,0);
		}
		if ( drivers_max44009_getSensors(&murawan_state.lastMLux) != MAX44009_SUCCESS ) {
			ITSDK_ERROR_REPORT(APP_ERROR_MAX44009_FAULT,0);
		}

		if ( murawan_state.connection == MURAWAN_CONNEXION_JOINED )
		   return ( MURAWAN_ST_SEND | STATE_IMMEDIATE_JUMP);
	}

	if ( murawan_state.connection != MURAWAN_CONNEXION_JOINED ) {
  	   // open for discussion, it is immediate or non ?
	   return MURAWAN_ST_JOIN;
	}
	return MURAWAN_ST_RUN;
}


/** ***************************************************************************************
 *  LoRaWan Communication
 */

/**
 * Downlink Management
 */
void proceed_downlink(uint8_t rPort, uint8_t rSize,uint8_t * rData) {
	log_info("Receiving data\r\n");

}

/**
 * Data transmission
 */
uint16_t murawan_stm_stSend(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {

	uint16_t next = MURAWAN_ST_RUN;

		uint8_t frBuffer[128];
		int index = 0;
		itsdk_cayenne_data_u cayenne;
		cayenne.temperature = murawan_state.lastTemp/100;	// from mC to dC
		itsdk_cayenne_encodePayload(0,ITSDK_CAYENNE_TYPE_TEMPERATURE,&cayenne,frBuffer,&index,128);
		cayenne.humidity = murawan_state.lastHumidity / 500; // from m% to 0,5%
		itsdk_cayenne_encodePayload(1,ITSDK_CAYENNE_TYPE_HUMIDITY,&cayenne,frBuffer,&index,128);
		cayenne.barometer = murawan_state.lastPressure/10; // from Pa to 0.1hPa
		itsdk_cayenne_encodePayload(2,ITSDK_CAYENNE_TYPE_BAROMETER,&cayenne,frBuffer,&index,128);
		cayenne.illuminance = murawan_state.lastMLux / 1000; // from mLux to Lux
		itsdk_cayenne_encodePayload(3,ITSDK_CAYENNE_TYPE_ILLUMINANCE,&cayenne,frBuffer,&index,128);

		itsdk_lorawan_sendconf_t ack = LORAWAN_SEND_UNCONFIRMED;
		if ( murawan_state.lastAckTestS > itsdk_config.app.ackDuty*itsdk_config.app.sendDuty*MURAWAN_CONFIG_TIME_BASE_S) {
			murawan_state.lastAckTestS = 0;
			ack = LORAWAN_SEND_CONFIRMED;
		}

		uint8_t rPort,rSize=32;
		uint8_t rData[32];
		itsdk_lorawan_send_t r = itsdk_lorawan_send_sync(
				frBuffer,
				index,
				1,
				__LORAWAN_DR_5,
				ack,
				itsdk_config.sdk.lorawan.retries,
				&rPort,
				&rSize,
				rData,
				/*PAYLOAD_ENCRYPT_AESCTR | PAYLOAD_ENCRYPT_SPECK |*/ PAYLOAD_ENCRYPT_NONE
		);
		switch ( r ) {
		case LORAWAN_SEND_ACKED_WITH_DOWNLINK:
			// case with downlink data
		case LORAWAN_SEND_ACKED_WITH_DOWNLINK_PENDING:
			// case with downlink data + pending downlink on server side.
			proceed_downlink(rPort,rSize,rData);
			murawan_state.ackFailed = 0;
			break;
		case LORAWAN_SEND_ACKED:
			murawan_state.ackFailed = 0;
			break;
		case LORAWAN_SEND_SENT:
			if ( ack == LORAWAN_SEND_CONFIRMED ) {
				murawan_state.ackFailed++;
			}
			if ( murawan_state.ackFailed > MURAWAN_CONFIG_ACKRETRY ) {
				murawan_state.connection = MURAWAN_CONNEXION_DISCONNECTED;
				ITSDK_ERROR_REPORT(APP_ERROR_LORA_DISCONNECT,0);
			}
			break;
		case LORAWAN_SEND_DUTYCYCLE:
			// retry
			next = MURAWAN_ST_SEND;
			break;
		case LORAWAN_SEND_NOT_JOINED:
			murawan_state.connection = MURAWAN_CONNEXION_DISCONNECTED;
			ITSDK_ERROR_REPORT(APP_ERROR_LORA_DISCONNECT,0);
			break;

		default:
			break;
		}
	return next;
}

/** ***************************************************************************************
 *  LoRaWan Join procedure
 */

uint16_t murawan_stm_stJoin(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
	uint16_t next = MURAWAN_ST_JOIN;
	switch ( murawan_state.connection ) {
	case MURAWAN_CONNEXION_INIT:
		// first connection, no wait, loop until connection success
		if ( !itsdk_lorawan_hasjoined() ) {
			if ( itsdk_lorawan_join_sync() == LORAWAN_JOIN_SUCCESS ) {
				murawan_state.connection = MURAWAN_CONNEXION_JOINED;
				murawan_state.connectionFailed = 0;
				next = MURAWAN_ST_RUN;
			} else {
				murawan_state.connectionFailed++;
				if ( murawan_state.connectionFailed > 20 ) {
					// slow down the retry
					murawan_state.connection = MURAWAN_CONNEXION_DISCONNECTED;
				}
			}
		}
		break;
	case MURAWAN_CONNEXION_DISCONNECTED:
		// reconnection try on every
		if (murawan_state.lastConnectTryS > itsdk_config.app.sleepDuty*itsdk_config.app.sendDuty*MURAWAN_CONFIG_TIME_BASE_S ) {
			murawan_state.lastConnectTryS = 0;
			if ( itsdk_lorawan_join_sync() == LORAWAN_JOIN_SUCCESS ) {
				murawan_state.connection = MURAWAN_CONNEXION_JOINED;
				murawan_state.connectionFailed=0;
			} else {
				murawan_state.connectionFailed++;
			}
			next = MURAWAN_ST_RUN;
		}
		break;
	default:
	case MURAWAN_CONNEXION_JOINED:
		// we should not be here
		next = MURAWAN_ST_RUN;
	}
	return next;
}

