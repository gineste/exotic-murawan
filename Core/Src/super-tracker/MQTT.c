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
#include "super-tracker/MQTT.h"

/****************************************************************************************
 * Defines
 ****************************************************************************************/

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
/*void vMQTT_send(uint8_t * p_au8Payload)
{
   uint8_t CTRL_Z[] = { 26 };
   uint8_t token_buffer[100] = {0};
   uint8_t l_u8Open = 0u;

   eBG96_SendCommand("AT+QMTOPEN=1,\"34.247.165.143\",1883", "+QMTOPEN: 1,0", true, 10000);

	if (g_u8ATretVal == AT_RET_OK)
	{
		l_u8Open = u8Tools_isStringInBuffer(g_au8ATRespBuffer, "+QMTOPEN: 1,0");
		g_u8ATretVal = AT_RET_ERROR;
	}

	if(l_eRuiStatus == RUI_STATUS_OK)
	{
		sprintf(token_buffer, "AT+QMTCONN=1,\"34.247.165.143\",\"%s\"", g_u8Token);
   	eBG96_sendBG96Command(token_buffer, "+QMTCONN: 1,0", true, 10000);

		if(l_eRuiStatus == RUI_STATUS_OK)
		{
			l_eRuiStatus = eBG96_SendCommand("AT+QMTPUB=1,0,0,0,\"v1/devices/me/telemetry\"", ">", false, 10000);

			if(l_eRuiStatus == RUI_STATUS_OK)
			{
				eBG96_SendCommand(payload, "+QMTPUB: 1,0,0", false, 10000);
				//rui_cellular_send(CTRL_Z);
			}

			l_eRuiStatus = eBG96_SendCommand("AT+QMTDISC=1", "+QMTDISC: 1,0", true, 10000);
		}
	}

	return l_eRuiStatus;
}*/

/****************************************************************************************
 * Private functions
 ****************************************************************************************/

/****************************************************************************************
 * End Of File
 ****************************************************************************************/




