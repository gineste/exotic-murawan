/* ==========================================================
 * project_main.c - 
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 6 sept. 2018
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
 * 
 * Add a such file in the main Src directory for executing
 * you own workload.
 *
 * ==========================================================
 */
#include <it_sdk/config.h>
#include <it_sdk/itsdk.h>
#include <it_sdk/time/time.h>
#include <it_sdk/logger/logger.h>
#include <it_sdk/sched/scheduler.h>
#include <it_sdk/statemachine/statemachine.h>
#include <it_sdk/eeprom/eeprom.h>
#include <it_sdk/lorawan/lorawan.h>
#include <it_sdk/encrypt/encrypt.h>
#include <it_sdk/eeprom/securestore.h>
#include <it_sdk/lowpower/lowpower.h>


#define ST_INIT 0
#define ST_SETUP 1
#define ST_SLEEP 2

/*
void updateTiming() {
	log_debug("In update timing\r\n");
}

uint8_t stInit(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
	log_debug("In Init %d,%d\r\n",cLoop,tLoop);

	//uint16_t t = adc_getTemperature();
	//log_debug("Temp : %d\r\n",t);
	return ST_SETUP;
}

uint8_t stSetup(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
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


	return ST_SLEEP;
}

uint8_t led = GPIO_PIN_SET;
uint8_t stSleep(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop) {
	log_debug("In Sleep %d,%d\r\n",cLoop,tLoop);
	led = (led==GPIO_PIN_SET)?GPIO_PIN_RESET:GPIO_PIN_SET;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7,led);

	log_info("time is %d\r\n",(uint32_t)itsdk_time_get_ms());
	return ST_SLEEP;
}
*/

/*
machine_t machine = {
    ST_INIT,				// currentState
    LOOP_INIT_VALUE, 		// loopCounter
    STATE_UNKNOWN,			// lastState
    TOTAL_LOOP_INIT_VALUE,	// totalLoop
    &updateTiming,			// Update age each time the machine is running - on each cycles.
    {
      //       UID        RESET       PROCE     P0      Name:XXXXXXX
        { ST_INIT,        NULL,      &stInit,    NULL,   	"Init"   },
        { ST_SETUP, 	  NULL,	   &stSetup,   	 NULL,   	"Setup"  },
        { ST_SLEEP, 	  NULL,     &stSleep,    NULL,   	"Sleep"  },
        { STATE_LAST,	  NULL,         NULL,    NULL,      "Error"  }

    }
};
*/


void task() {
//	statem(&machine);
	log_info("+ \r\n");
}


void project_setup() {
	log_init(ITSDK_LOGGER_CONF);
	log_info("Booting \r\n");

	// Init at boot time
	//loadConfig();

	// Init LoRaWan stack
	static itsdk_lorawan_channelInit_t channels= ITSDK_LORAWAN_CHANNEL;
	#ifdef ITSDK_LORAWAN_CHANNEL
		itsdk_lorawan_setup(__LORAWAN_REGION_EU868,&channels);
	#else
		itsdk_lorawan_setup(__LORAWAN_REGION_EU868,NULL);
	#endif

	itdt_sched_registerSched(30000,ITSDK_SCHED_CONF_IMMEDIATE, &task);
}

void project_loop() {
	itsdk_lorawan_loop();
}

