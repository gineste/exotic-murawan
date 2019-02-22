/* ==========================================================
 * machine.h - 
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 13 févr. 2019
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

#ifndef INC_MURAWAN_MACHINE_H_
#define INC_MURAWAN_MACHINE_H_

#include <it_sdk/itsdk.h>
#include <murawan/machine.h>

#define MURAWAN_ST_SETUP 		0
#define MURAWAN_ST_WAIT4CONF 	1
#define MURAWAN_ST_RUN 			2
#define MURAWAN_ST_SEND			3

typedef struct {
	uint64_t	lastTimeUpdateMs;			// in ms => last time we updated the timings
	uint32_t	lastMeasureS;				// time in S since last measure
	uint32_t	lastAckTestS;				// time in S since last test
	uint32_t	lastConnectTryS;			// time in S since last connection try
	uint8_t		ackFailed;					// Number of ack tried failed

	int32_t  	lastTemp;					// Last temperature in m°C
	uint32_t 	lastHumidity;				// Last humidity in m%
	uint32_t 	lastPressure;				// Last pressure in Pa
	uint32_t 	lastMLux;					// Last light int mLux

} murawan_state_t;

extern murawan_state_t murawan_state;

void murawan_stm_updateTiming();
uint16_t murawan_stm_stSetup(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop);
uint16_t murawan_stm_stRun(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop);
uint16_t murawan_stm_stWaitC(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop);
uint16_t murawan_stm_stSend(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop);

void murawan_setup();

#endif /* INC_MURAWAN_MACHINE_H_ */
