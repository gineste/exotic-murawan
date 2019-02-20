/* ==========================================================
 * configDrivers.h - 
 * Project : Disk91 SDK
 * ----------------------------------------------------------
 * Created on: 7 nov. 2018
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
 *
 * ==========================================================
 */

#ifndef INC_IT_SDK_CONFIGDRIVERS_H_
#define INC_IT_SDK_CONFIGDRIVERS_H_


// *************************************** EEPROM *****************************************************************

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// | SDK SETTING                   | USER SELECTED VALUE                  | SETTING DESCRIPTION                   |
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ------------------------------------------------------------------------
// EEPROM : M95640

#define ITSDK_DRIVERS_M95640					__DISABLE					// this driver is activated in build
#if ITSDK_DRIVERS_M95640 == __ENABLE
	#define __SPI_INCLUDED
	#include "spi.h"
#endif
#define ITSDK_DRIVERS_M95640_SPI				hspi1						// Spi port to be used for communications
#define ITSDK_DRIVERS_M95640_SPI_CS_BANK		__BANK_B					// EEPROM Chip Select PIN LAYOUT (Activ Low)
#define ITSDK_DRIVERS_M95640_SPI_CS_PIN			__LP_GPIO_4					//   Config as output, pull-up init High


// ------------------------------------------------------------------------
// Temp / Humidity / Pressure : M95640

#define ITSDK_DRIVERS_BME280					__ENABLE					// BOSH BME 280 Temp / Hygro / Pressure
#if ITSDK_DRIVERS_BME280 == __ENABLE
	#ifndef __I2C_INCLUDED
	  #define __I2C_INCLUDED
	  #include "i2c.h"
	#endif
	#include <drivers/temphygropressure/bosh_bme280/bme280.h>
#endif
#define ITSDK_DRIVERS_BME280_I2C				hi2c1						// I2C port to be used for communications
#define __BME280_SDO_HIGH						0x1
#define __BME280_SDO_LOW						0x0
#define ITSDK_DRIVERS_BME280_ADDRESS			(DRIVER_BME280_DEVICE_ADR | __BME280_SDO_LOW)	//   Device address base on electrical conf

// ------------------------------------------------------------------------
// Light sensor : MAX44009

#define ITSDK_DRIVERS_MAX44009					__ENABLE
#if ITSDK_DRIVERS_MAX44009 == __ENABLE
	#ifndef __I2C_INCLUDED
	  #define __I2C_INCLUDED
	  #include "i2c.h"
	#endif
	#include <drivers/light/max44009/max44009.h>
#endif
#define ITSDK_DRIVERS_MAX44009_I2C				hi2c1						// I2C port to be used for communications
#define __MAX44009_A0_HIGH						0x1
#define __MAX44009_A0_LOW						0x0
#define ITSDK_DRIVERS_MAX44009_ADDRESS			(DRIVER_MAX44009_DEVICE_ADR | __MAX44009_A0_LOW)	// Device address, based on the schematics

#endif /* INC_IT_SDK_CONFIGDRIVERS_H_ */
