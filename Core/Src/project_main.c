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
#include <it_sdk/lorawan/lorawan.h>
#include <it_sdk/encrypt/encrypt.h>
#include <it_sdk/eeprom/securestore.h>
#include <it_sdk/lowpower/lowpower.h>
#include <it_sdk/statemachine/statemachine.h>

#include <it_sdk/configDrivers.h>

#include <murawan/radio.h>
#include <murawan/machine.h>

/**
 * Process the state machine on regular basis
 */
extern machine_t murawan_stm;
void task() {
	statem(&murawan_stm);
}


/**
 * On Reset setup
 */
void project_setup() {
	log_info("UUUUUUUUUUUUUUUUUUUUU\r\n");
	log_info("UUUUUUUUUUUUUUUUUUUUU\r\n");
	log_info("UUUUUUUUUUUUUUUUUUUUU\r\n");
	log_info("Booting !!\r\n");

	// Hardware configuration
	// Rq: the hardware init is managed by Cube Mx but default setting with functional
	//     impacts are initialized here
	murawan_antenna_selectPifa();


	itdt_sched_registerSched(12000,ITSDK_SCHED_CONF_IMMEDIATE, &task);
}

/**
 * LowPower loop
 */
void project_loop() {
	itsdk_lorawan_loop();
}

