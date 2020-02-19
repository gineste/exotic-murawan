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

#include <super-tracker/cellular.h>
#include <super-tracker/MQTT.h>

static void vCreateJsonFrame(float latitude, float longitude, float battery, float temperature, float humidity, uint8_t payload[])
{
    memset(payload,0,256);

    sprintf(payload,"{\n\t\"latitude\": %f,\n"
        "\t\"longitude\": %f,\n"
        "\t\"battery\": %.2f,\n"
        "\t\"temperature\": %.2f,\n"
        "\t\"humidity\": %.2f\n"
        "}\n%c",
        latitude, longitude, battery, temperature, humidity, (char) 26);
}

/**
 * Process the state machine on regular basis
 */
extern machine_t murawan_stm;
void
task ()
{
	statem (&murawan_stm);
}

/**
 * On Reset setup
 */
void
project_setup ()
{
	//log_info ("Booting !!\r\n");

	/* setup cellular connexion */
	uint8_t send_data[256] = {0};
	vCellular_Connect();
	vCreateJsonFrame(63.23361814088533, 4.6292071962105235, 4.0, 10, 42.42, send_data);
	vMQTT_send(send_data);

	murawan_state.lastResetCause = itsdk_getResetCause ();
	itdt_sched_registerSched (MURAWAN_CONFIG_TIME_BASE_S * 1000,
									  ITSDK_SCHED_CONF_IMMEDIATE, &task);
}

/**
 * LowPower loop
 */
void
project_loop ()
{
	st25dv_process ();
	itsdk_lorawan_loop ();
}

