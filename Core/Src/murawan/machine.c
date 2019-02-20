/* ==========================================================
 * machine.c - The state machine
 * Project : Murawan
 * ----------------------------------------------------------
 * Created on: 13 f�vr. 2019
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

#include <drivers/temphygropressure/bosh_bme280/bme280.h>
#include <drivers/light/max44009/max44009.h>
#include <it_sdk/lorawan/cayenne.h>

#include <murawan/machine.h>

machine_t murawan_stm = {
	MURAWAN_ST_INIT,			// currentState
    LOOP_INIT_VALUE, 			// loopCounter
    STATE_UNKNOWN,				// lastState
    TOTAL_LOOP_INIT_VALUE,		// totalLoop
    &murawan_stm_updateTiming,	// Update age each time the machine is running - on each cycles.
    {
      //       UID                RESET       PROCE      			 P0      Name:XXXXXXX
        { MURAWAN_ST_INIT,        NULL,      &murawan_stm_stInit,    NULL,   	"Init"   },
        { MURAWAN_ST_SETUP, 	  NULL,	    &murawan_stm_stSetup,    NULL,   	"Setup"  },
        { MURAWAN_ST_SLEEP, 	  NULL,     &murawan_stm_stSleep,    NULL,   	"Sleep"  },
        { STATE_LAST,	 		 NULL,          NULL,  				  NULL,     "Error"  }

    }
};


void murawan_stm_updateTiming() {
	log_debug("In update timing\r\n");
}

uint8_t murawan_stm_stInit(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
	log_debug("In Init %d,%d\r\n",cLoop,tLoop);

	return MURAWAN_ST_SETUP;
}

uint8_t murawan_stm_stSetup(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
	log_debug("In Setup %d,%d\r\n",cLoop,tLoop);


	struct conf {
		uint8_t	v1;
		uint16_t v2;
		uint32_t v3;
	} s_conf;

	uint8_t v;
	if ( ! eeprom_read(&s_conf, sizeof(s_conf), 1,&v) ) {
		log_debug("Flashing\r\n");
		s_conf.v1 = 10;
		s_conf.v2 = 0xA5A5;
		s_conf.v3 = 0xFF5AA5FF;
		eeprom_write(&s_conf, sizeof(s_conf), 1);
	} else {
		log_debug("Load version %d\r\n",v);
		log_debug("v2 : %0X\r\n",s_conf.v2);
	}

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

	return MURAWAN_ST_SLEEP;
}


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

uint8_t murawan_stm_stSleep(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
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
				ITSDK_LORAWAN_CNF_RETRY,
				send_callback,
				/*PAYLOAD_ENCRYPT_AESCTR | PAYLOAD_ENCRYPT_SPECK |*/ PAYLOAD_ENCRYPT_NONE
		);
		if ( r != LORAWAN_SEND_RUNNING ) {
			log_warn("an error has been reported : %d\r\n",r);
		}
	}

	log_info("time is %d\r\n",(uint32_t)itsdk_time_get_ms());
	return MURAWAN_ST_SLEEP;
}
