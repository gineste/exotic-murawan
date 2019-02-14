/* ==========================================================
 * radio.c - All about the radio configuration specific to
 *           murawan
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
#include <it_sdk/config.h>
#include <murawan/radio.h>
#include <murawan/hwconfig.h>
#include <it_sdk/wrappers.h>

void murawan_antenna_selectPifa() {
	gpio_reset(MURAWAN_RFSWITCHA_PORT,MURAWAN_RFSWITCHA_PIN);
	gpio_set(MURAWAN_RFSWITCHB_PORT,MURAWAN_RFSWITCHB_PIN);
}

void murawan_antenna_selectPCB() {
	gpio_set(MURAWAN_RFSWITCHA_PORT,MURAWAN_RFSWITCHA_PIN);
	gpio_reset(MURAWAN_RFSWITCHB_PORT,MURAWAN_RFSWITCHB_PIN);
}
