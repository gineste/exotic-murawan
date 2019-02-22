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

/**
 * Setup step, here we are configuring the hardware and software stuff
 */
uint8_t murawan_stm_stSetup(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
	log_debug("In Setup %d,%d\r\n",cLoop,tLoop);

	murawan_setup();

	static itsdk_lorawan_channelInit_t channels= ITSDK_LORAWAN_CHANNEL;
	#ifdef ITSDK_LORAWAN_CHANNEL
		itsdk_lorawan_setup(__LORAWAN_REGION_EU868,&channels);
	#else
		itsdk_lorawan_setup(__LORAWAN_REGION_EU868,NULL);
	#endif

	// setup sensors
	if (drivers_bme280_setup(BME280_MODE_WEATHER_MONITORING) != BME280_SUCCESS ) {
		log_error("[BME280] Init Failed\r\n");
	}
	if (drivers_max44009_setup(MAX44009_MODE_ONREQUEST) != MAX44009_SUCCESS ) {
		log_error("[MAX44009] Init Failed\r\n");
	}


	uint8_t devEui[8];
	itsdk_lorawan_getDeviceEUI(devEui);
	uint8_t i = 0;
	while ( i < 8 && devEui[i] == 0 ) i++;
	if  ( i == 8 ) {
		// The device is not yet initialized
		return MURAWAN_ST_WAIT4CONF;
	} else {
		// Ready to go
		return MURAWAN_ST_RUN;
	}

}


/**
 * Until the configuration of the deviceId is correct we stay in the stWait
 */
uint8_t murawan_stm_stWaitC(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {

	// Assuming the more convinient way is to reset after configuration
	return MURAWAN_ST_WAIT4CONF;

}


/**
 * Main event loop
 */

void send_callback(itsdk_lorawan_send_t status, uint8_t port, uint8_t size, uint8_t * rxData) {
	switch ( status ) {
	case LORAWAN_SEND_ACKED_WITH_DOWNLINK:
		// case with downlink data
	case LORAWAN_SEND_ACKED_WITH_DOWNLINK_PENDING:
		// case with downlink data + pending downlink on server side.
	case LORAWAN_SEND_ACKED:
		break;
	case LORAWAN_SEND_SENT:
		break;
	default:
		break;
	}
}

uint8_t murawan_stm_stRun(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
	log_debug("In Sleep %d,%d\r\n",cLoop,tLoop);


	// Get the sensor values
	int32_t  temp;
	uint32_t humidity;
	uint32_t pressure;
	uint32_t mLux;
	if ( drivers_bme280_getSensors(&temp,&pressure,&humidity) != BME280_SUCCESS ) {
		ITSDK_ERROR_REPORT(APP_ERROR_BME280_FAULT,0);
	} else {
		log_info("Temp is %d.%d oC\r\n",temp/1000,(temp/100)-((temp/1000)*10));
		log_info("Humidity is %d.%d \%\r\n",humidity/1000,(humidity/100)-((humidity/1000)*10));
		log_info("Pressure is %d.%d hPa\r\n",pressure/100,(pressure/10)-((pressure/100)*10));
	}

	if ( drivers_max44009_getSensors(&mLux) != MAX44009_SUCCESS ) {
		ITSDK_ERROR_REPORT(APP_ERROR_MAX44009_FAULT,0);
	} else {
		log_info("Lux is %d.%d lux\r\n",mLux/1000,(mLux/100)-((mLux/1000)*10));
	}

	if ( !itsdk_lorawan_hasjoined() ) {
		if ( itsdk_lorawan_join_sync() == LORAWAN_JOIN_SUCCESS ) {
			//
		} else {
			//
		}
	} else {

		uint8_t frBuffer[128];
		int index = 0;
		itsdk_cayenne_data_u cayenne;
		cayenne.temperature = temp/100;	// from mC to dC
		itsdk_cayenne_encodePayload(0,ITSDK_CAYENNE_TYPE_TEMPERATURE,&cayenne,frBuffer,&index,128);
		cayenne.humidity = humidity / 500; // from m% to 0,5%
		itsdk_cayenne_encodePayload(1,ITSDK_CAYENNE_TYPE_HUMIDITY,&cayenne,frBuffer,&index,128);
		cayenne.barometer = pressure/10; // from Pa to 0.1hPa
		itsdk_cayenne_encodePayload(2,ITSDK_CAYENNE_TYPE_BAROMETER,&cayenne,frBuffer,&index,128);
		cayenne.illuminance = mLux / 1000; // from mLux to Lux
		itsdk_cayenne_encodePayload(3,ITSDK_CAYENNE_TYPE_ILLUMINANCE,&cayenne,frBuffer,&index,128);

		itsdk_lorawan_send_t r = itsdk_lorawan_send_async(
				frBuffer,
				index,
				1,
				__LORAWAN_DR_5,
				LORAWAN_SEND_CONFIRMED,
				itsdk_config.sdk.lorawan.retries,
				send_callback,
				/*PAYLOAD_ENCRYPT_AESCTR | PAYLOAD_ENCRYPT_SPECK |*/ PAYLOAD_ENCRYPT_NONE
		);
		if ( r != LORAWAN_SEND_RUNNING ) {
			log_warn("an error has been reported : %d\r\n",r);
		}
	}

	log_info("time is %d\r\n",(uint32_t)itsdk_time_get_ms());
	return MURAWAN_ST_RUN;
}
