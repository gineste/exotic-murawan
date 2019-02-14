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

#define MURAWAN_ST_INIT  0
#define MURAWAN_ST_SETUP 1
#define MURAWAN_ST_SLEEP 2


void murawan_stm_updateTiming();
uint8_t murawan_stm_stInit(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop);
uint8_t murawan_stm_stSetup(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop);
uint8_t murawan_stm_stSleep(void * p, uint8_t cState, uint16_t cLoop, uint32_t tLoop);



#endif /* INC_MURAWAN_MACHINE_H_ */
