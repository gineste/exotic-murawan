/* ==========================================================
 * config.c - configuration management
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 22 févr. 2019
 *     Author: Paul Pinault aka Disk91
 * ----------------------------------------------------------
 * Copyright (C) 2019 Exotic-Systems
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
#include <it_sdk/config.h>
#include <it_sdk/eeprom/sdk_config.h>
#include <it_sdk/console/console.h>
#include <murawan/radio.h>
#include <murawan/machine.h>
#include <drivers/gauge/max17205/max17205.h>

#if ITSDK_WITH_CONSOLE == __ENABLE
  #include <it_sdk/console/console.h>
  static itsdk_console_chain_t __murawan_configMng;
  static itsdk_console_return_e _itsdk_murawan_consolePriv(char * buffer, uint8_t sz);
  static itsdk_console_return_e _itsdk_murawan_consolePublic(char * buffer, uint8_t sz);
#endif

// State
murawan_state_t murawan_state;


// SETUP

/**
 * Setup the murawan software & boards
 */
void murawan_setup() {
#if ITSDK_WITH_CONSOLE == __ENABLE
	__murawan_configMng.console_private = _itsdk_murawan_consolePriv;
	__murawan_configMng.console_public = _itsdk_murawan_consolePublic;
	__murawan_configMng.next = NULL;
	itsdk_console_registerCommand(&__murawan_configMng);
#endif

	// Setup Hardware

	// Hardware configuration
	// Rq: the hardware init is managed by Cube Mx but default setting with functional
	//     impacts are initialized here
	if ( itsdk_config.app.antennaChoice == MURAWAN_ANTENNA_PIFA ){
		murawan_antenna_selectPifa();
	} else {
		murawan_antenna_selectPCB();
	}

	// Init State
	murawan_state.lastTimeUpdateMs = 0;
	murawan_state.lastAckTestS = 0;
	murawan_state.lastConnectTryS = 0;
	murawan_state.lastMeasureS = 0;
	murawan_state.lastBatReportS = 0;
	murawan_state.ackFailed = 0;
	murawan_state.connection = MURAWAN_CONNEXION_INIT;
	murawan_state.connectionFailed = 0;
	murawan_state.lastCell1mV = 0;
	murawan_state.lastCell2mV = 0;
	murawan_state.lastCell3mV = 0;
	murawan_state.lastvBatmV = 0;
	murawan_state.lastCurrent = 0;
	murawan_state.lastCoulomb = 0;
	murawan_state.bootFrameSent = 0;
	// init vbat values values
	switch ( drivers_max17205_isReady() ) {
	case MAX17205_SUCCESS:
		drivers_max17205_getVoltage(MAX17205_CELL3,&murawan_state.lastCell3mV);
		drivers_max17205_getVoltage(MAX17205_CELL2,&murawan_state.lastCell2mV);
		drivers_max17205_getVoltage(MAX17205_CELL1,&murawan_state.lastCell1mV);
	case MAX17205_UNDERVOLT:
		drivers_max17205_getVoltage(MAX17205_VBAT,&murawan_state.lastvBatmV);
	default:
		break;
	}
	murawan_state.lastMeasure.vbat = murawan_state.lastvBatmV / 100;
	murawan_state.lastMeasure.vcell1 = murawan_state.lastCell1mV / 100;
	murawan_state.lastMeasure.vcell2 = murawan_state.lastCell2mV / 100;
	murawan_state.lastMeasure.vcell3 = murawan_state.lastCell3mV / 100;
}

// PRINT STATE
void murawan_print_state() {
	_itsdk_console_printf("state.lastTimeUpdateMs : %d s\r\n",(uint32_t)(murawan_state.lastTimeUpdateMs/1000));
	_itsdk_console_printf("state.lastAckTestS : %d s\r\n",murawan_state.lastAckTestS);
	_itsdk_console_printf("state.lastConnectTryS : %d s\r\n",murawan_state.lastConnectTryS);
	_itsdk_console_printf("state.lastMeasureS : %d s\r\n",murawan_state.lastMeasureS);
	_itsdk_console_printf("state.lastBatReportS : %d s\r\n",murawan_state.lastBatReportS);
	_itsdk_console_printf("state.ackFailed : %d\r\n",murawan_state.ackFailed);

	_itsdk_console_printf("state.lastTemp : %d.%d oC\r\n",murawan_state.lastTemp/1000,(murawan_state.lastTemp/100)-((murawan_state.lastTemp/1000)*10));
	_itsdk_console_printf("state.lastHumidity : %d.%d \r\n",murawan_state.lastHumidity/1000,(murawan_state.lastHumidity/100)-((murawan_state.lastHumidity/1000)*10));
	_itsdk_console_printf("state.lastPressure : %d.%d hPa\r\n",murawan_state.lastPressure/100,(murawan_state.lastPressure/10)-((murawan_state.lastPressure/100)*10));
	_itsdk_console_printf("state.lastMLux : %d.%d lux\r\n",murawan_state.lastMLux/1000,(murawan_state.lastMLux/100)-((murawan_state.lastMLux/1000)*10));

	_itsdk_console_printf("state.lastCell1mV : %d mV\r\n",murawan_state.lastCell1mV);
	_itsdk_console_printf("state.lastCell2mV : %d mV\r\n",murawan_state.lastCell2mV);
	_itsdk_console_printf("state.lastCell3mV : %d mV\r\n",murawan_state.lastCell3mV);
	_itsdk_console_printf("state.lastvBatmV : %d mV\r\n",murawan_state.lastvBatmV);
	_itsdk_console_printf("state.lastCurrent : %d uA\r\n",murawan_state.lastCurrent);
	_itsdk_console_printf("state.lastCoulomb : %d C\r\n",murawan_state.lastCoulomb);

	_itsdk_console_printf("state.lastResetCause : 0x%04X \r\n",murawan_state.lastResetCause);
	_itsdk_console_printf("state.connection : %d \r\n",murawan_state.connection);
	_itsdk_console_printf("state.connectionFailed : %d \r\n",murawan_state.connectionFailed);
	_itsdk_console_printf("stm.currentState : %s \r\n",murawan_stm.stm[murawan_stm.currentState].name);
}


// OVERRIDE THE CONFIG FUNCTIONS

itsdk_config_ret_e itsdk_config_app_resetToFactory() {
	itsdk_config.app.version = ITSDK_USER_VERSION_BYTE;
	itsdk_config.app.sendDuty = MURAWAN_CONFIG_SENDDUTY;
	itsdk_config.app.ackDuty = MURAWAN_CONFIG_ACKDUTY;
	itsdk_config.app.ackRetry = MURAWAN_CONFIG_ACKRETRY;
	itsdk_config.app.sleepDuty = MURAWAN_CONFIG_SLEEPDUTY;
	itsdk_config.app.antennaChoice = MURAWAN_ANTENNA_PIFA;
	itsdk_config.app.batDuty = MURAWAN_CONFIG_BATDUTY;
	return CONFIG_RESTORED_FROM_FACTORY;
}

itsdk_config_ret_e itsdk_config_app_commitConfiguration() {
	if (itsdk_config.app.antennaChoice != itsdk_config_shadow.app.antennaChoice ) {
		if ( itsdk_config_shadow.app.antennaChoice == MURAWAN_ANTENNA_PIFA ){
			murawan_antenna_selectPifa();
		} else {
			murawan_antenna_selectPCB();
		}
	}
	return CONFIG_SUCCESS;
}

void itsdk_config_app_printConfig(itsdk_configuration_nvm_t * c) {
	_itsdk_console_printf("murawan.version : %02X\r\n",c->app.version);
	_itsdk_console_printf("murawan.sendDuty : %d s\r\n",c->app.sendDuty*MURAWAN_CONFIG_TIME_BASE_S);
	_itsdk_console_printf("murawan.ackDuty : %d s\r\n",c->app.ackDuty*c->app.sendDuty*MURAWAN_CONFIG_TIME_BASE_S);
	_itsdk_console_printf("murawan.ackRetry : %d\r\n",c->app.ackRetry);
	_itsdk_console_printf("murawan.sleepDuty : %d s\r\n",c->app.sleepDuty*c->app.sendDuty*MURAWAN_CONFIG_TIME_BASE_S);
	_itsdk_console_printf("murawan.batDuty : %d m\r\n",c->app.batDuty*MURAWAN_CONFIG_BATDUTY_TIME_BASE_M);
	_itsdk_console_printf("murawan.antennaChoice : %d\r\n",c->app.antennaChoice);
}

// ========================================================================================
// EXTENDS THE CONSOLE

#if ITSDK_WITH_CONSOLE == __ENABLE

static itsdk_console_return_e _itsdk_murawan_consolePublic(char * buffer, uint8_t sz) {
   if ( sz == 1 ) {
	  switch(buffer[0]){
		case '?':
			// help
			_itsdk_console_printf("--- Murawan\r\n");
			_itsdk_console_printf("s          : print state\r\n");
		  return ITSDK_CONSOLE_SUCCES;
		  break;
		case 's':
			// print state
			murawan_print_state();
			_itsdk_console_printf("OK\r\n");
  		    return ITSDK_CONSOLE_SUCCES;
			break;
		default:
			break;
	  }
  } //Sz == 1
  return ITSDK_CONSOLE_NOTFOUND;
}

static itsdk_console_return_e _itsdk_murawan_consolePriv(char * buffer, uint8_t sz) {
	if ( sz == 1 ) {
	  switch(buffer[0]){
		case '?':
			// help
			#if ITSDK_WITH_LORAWAN_LIB == __ENABLE
			  _itsdk_console_printf("MC:0:XX    : murawan.sendDuty / %d s (HEXA)\r\n",MURAWAN_CONFIG_TIME_BASE_S);
			  _itsdk_console_printf("MC:1:XX    : murawan.ackDuty (HEXA) - sendDuy repeat\r\n");
			  _itsdk_console_printf("MC:2:XX    : murawan.ackRetry (HEXA) - ackDuty repeat\r\n");
			  _itsdk_console_printf("MC:3:XX    : murawan.sleepDuty (HEXA) - sendDuy repeat\r\n");
			  _itsdk_console_printf("MC:4:n     : murawan.antennaChoice 0 PIFA / 1 PCB\r\n");
			  _itsdk_console_printf("MC:5:XX    : murawan.batDuty (HEXA) / %d m\r\n",MURAWAN_CONFIG_BATDUTY_TIME_BASE_M);
			#endif
		  return ITSDK_CONSOLE_SUCCES;
		  break;
		default:
			break;
	  }
	} else if ( sz >= 6 ) {
		if ( buffer[0] == 'M' && buffer[1] == 'C' && buffer[2] == ':' && buffer[4] == ':' ) {
			switch(buffer[3]) {
			case '0':
				// murawan.sendDuty
				if ( sz >= 7 ) {
					itsdk_config_shadow.app.sendDuty = itdt_convertHexChar2Int(&buffer[5]);
				}
				_itsdk_console_printf("OK\r\n");
				return ITSDK_CONSOLE_SUCCES;
				break;

			case '1':
				// murawan.ackDuty
				if ( sz >= 7 ) {
					itsdk_config_shadow.app.ackDuty = itdt_convertHexChar2Int(&buffer[5]);
				}
				_itsdk_console_printf("OK\r\n");
				return ITSDK_CONSOLE_SUCCES;
				break;

			case '2':
				// murawan.ackRetry
				if ( sz >= 7 ) {
					itsdk_config_shadow.app.ackRetry = itdt_convertHexChar2Int(&buffer[5]);
				}
				_itsdk_console_printf("OK\r\n");
				return ITSDK_CONSOLE_SUCCES;
				break;

			case '3':
				// murawan.sleepDuty
				if ( sz >= 7 ) {
					itsdk_config_shadow.app.sleepDuty = itdt_convertHexChar2Int(&buffer[5]);
				}
				_itsdk_console_printf("OK\r\n");
				return ITSDK_CONSOLE_SUCCES;
				break;

			case '4':
				// murawan.antennaChoice
				if ( buffer[5] == '0' ) {
					itsdk_config_shadow.app.antennaChoice = MURAWAN_ANTENNA_PIFA;
					_itsdk_console_printf("OK\r\n");
					return ITSDK_CONSOLE_SUCCES;
				}
			    if ( buffer[5] == '1' ) {
					itsdk_config_shadow.app.antennaChoice = MURAWAN_ANTENNA_PCB;
					_itsdk_console_printf("OK\r\n");
					return ITSDK_CONSOLE_SUCCES;
			    }
				_itsdk_console_printf("KO\r\n");
				return ITSDK_CONSOLE_FAILED;
				break;

			case '5':
				// murawan.batDuty
				if ( sz >= 7 ) {
					itsdk_config_shadow.app.batDuty = itdt_convertHexChar2Int(&buffer[5]);
				}
				_itsdk_console_printf("OK\r\n");
				return ITSDK_CONSOLE_SUCCES;
				break;

			default:
				break;
			}
		}
	}
  return ITSDK_CONSOLE_NOTFOUND;
}

#endif


