/*
 *    ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
 *   (  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 *    ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \     
 *   (____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/
 *
 * Copyright (c) 2017 EXOTIC SYSTEMS. All Rights Reserved.
 *
 * Licensees are granted free, non-transferable use of the information. NO WARRANTY 
 * of ANY KIND is provided. This heading must NOT be removed from the file.
 *
 * Date:          19 févr. 2020
 * Author:        Martin
 * Description:   File description.
 *
 */

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include "super-tracker/BG96.h"
#include "super-tracker/tools.h"

#include "super-tracker/MQTT.h"

/****************************************************************************************
 * Defines
 ****************************************************************************************/
#define TOKEN "3mWgy02uvYLaHfAcNKu6"

/****************************************************************************************
 * Private type declarations
 ****************************************************************************************/

/****************************************************************************************
 * Private function declarations
 ****************************************************************************************/

/****************************************************************************************
 * Variable declarations
 ****************************************************************************************/ 

/****************************************************************************************
 * Public functions
 ****************************************************************************************/ 
e_BG96_ErrorCode_t vMQTT_send(uint8_t * p_au8Payload)
{
	uint8_t CTRL_Z[] = { 26 };
	uint8_t token_buffer[100] = {0};
	uint8_t l_u8Open = 0u;
	uint8_t l_u8Connect = 0u;
	uint8_t l_u8PublishedHalf = 0u;
	uint8_t l_u8PublishedCmplt = 0u;
	e_BG96_ErrorCode_t l_u8ErrorCode = BG96_ERROR_FAILED;
	uint8_t l_au8ATRespBuffer[BG96_RESP_SIZE_MAX] = {0};

	wdg_refresh();

	l_u8ErrorCode = eBG96_SendCommand("AT+QMTOPEN=1,\"34.247.165.143\",1883","+QMTOPEN: 1,0", 1, l_au8ATRespBuffer);

	itsdk_delayMs(2000);

	if (l_u8ErrorCode == BG96_ERROR_SUCCEED)
	{
		snprintf(token_buffer, 100, "AT+QMTCONN=1,\"34.247.165.143\",\"%s\"", TOKEN);
		l_u8ErrorCode = eBG96_SendCommand(token_buffer,"+QMTCONN: 1,0", 1, l_au8ATRespBuffer);

		itsdk_delayMs(2000);

		if (l_u8ErrorCode == BG96_ERROR_SUCCEED)
		{
			l_u8ErrorCode = eBG96_SendCommand("AT+QMTPUB=1,0,0,0,\"v1/devices/me/telemetry\"", ">", 1, l_au8ATRespBuffer);

			itsdk_delayMs(2000);

			if(l_u8ErrorCode == BG96_ERROR_SUCCEED)
			{
				//l_u8ErrorCode = eBG96_SendCommand(p_au8Payload,"+QMTPUB: 1,0,0", 1, l_au8ATRespBuffer);
				l_u8ErrorCode = eBG96_SendCommand("{\"latitude\": 45.63, \"longitude\": 2.3}","+QMTPUB: 1,0,0", 1, l_au8ATRespBuffer);

				itsdk_delayMs(2000);
			}

			eBG96_SendCommand("AT+QMTDISC=1", "+QMTDISC: 0", 1, l_au8ATRespBuffer);
		}
	}

	return l_u8ErrorCode;
}

/****************************************************************************************
 * Private functions
 ****************************************************************************************/

/****************************************************************************************
 * End Of File
 ****************************************************************************************/




