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
 * Date:          18 févr. 2020
 * Author:        Martin
 * Description:   Cellular network management.
 *
 */

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
/* Proprietary includes */
#include <stdint.h>
#include <string.h>

/* IT SDK includes */
#include <it_sdk/logger/logger.h>

/* Libraries includes */
#include "super-tracker/tools.h"

/* Driver sensors includes */
#include "super-tracker/BG96.h"

/* Self include */
#include "super-tracker/cellular.h"
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
static const uint8_t g_cau8Cmd[CELL_CMD_MAX][CELL_PREFIX_CMD_SIZE_MAX] = {
   { "AT+COPS=2" },  // Disconnect
   { "AT+COPS?" },   // Check if disconnected
   { "AT+COPS=0" },  // Connect to the best operator available
   { "AT+CGATT?" }   // Check if connected
};

/****************************************************************************************
 * Public functions
 ****************************************************************************************/ 
/**@brief Connect to cellular network.
 * @return Error code
 */
void vCellular_Connect(void)
{
	uint8_t l_u8WaitRegistrationRetry = 0u;
	uint8_t l_u8Registered = 0u;
	uint8_t l_u8Connected = 0u;
	uint8_t l_au8ATRespBuffer[BG96_RESP_SIZE_MAX] = {0};
	e_BG96_ErrorCode_t l_u8ErrorCode = BG96_ERROR_FAILED;

	// Automatic search sequence : LTEM -> LTENB -> GSM
	//eBG96_sendBG96Command("AT+QCFG=\"nwscanseq\",00", true);

	// Allow GSM & LTE
	//eBG96_sendBG96Command("AT+QCFG=\"nwscanmode\",00", true);

	// Search LTE on both LTE-M & NB-IOT frequencies
	//eBG96_sendBG96Command("AT+QCFG=\"iotopmode\",02", true);

	/*eBG96_SendCommand("AT+QGPS?", strlen("AT+QGPS?"), vCallback_OnDemand);
	if (g_u8ATretVal == AT_RET_OK)
	{
		l_u8isConnected = u8Tools_isStringInBuffer(g_au8ATRespBuffer, "+QGPS: 1");
		g_u8ATretVal = AT_RET_ERROR;
	}

	if (l_u8isConnected == 0u)
	{
		//RUI_LOG_PRINTF("GPS not connected. Connexion...");

		eBG96_SendCommand("AT+QGPS=1", strlen("AT+QGPS=1"), vCallback_OnDemand);
			//RUI_LOG_PRINTF("Fail to connect GPS.");

		eBG96_SendCommand("AT+QGPSCFG=\"nmeasrc\",1", strlen("AT+QGPSCFG=\"nmeasrc\",1"), vCallback_OnDemand);
			//RUI_LOG_PRINTF("Fail to config GPS NMEASRC");

		eBG96_SendCommand("AT+QGPSCFG=\"gpsnmeatype\",1", strlen("AT+QGPSCFG=\"gpsnmeatype\",1"), vCallback_OnDemand);
			//RUI_LOG_PRINTF("Fail to config GPS NMEATYPE GGA");
	}*/
	eBG96_SendCommand("ATE0", NULL, 0, l_au8ATRespBuffer);

	eBG96_SendCommand("AT+COPS=2", NULL, 0, l_au8ATRespBuffer);

	// Connect with Orange
	eBG96_SendCommand("AT+COPS=1,0,\"Orange F\",0", NULL, 0, l_au8ATRespBuffer);
	// Connect with best operator
	//eBG96_SendCommand("AT+COPS=0", strlen("AT+COPS=0"), vCallback_OnDemand);

	// Check for cellular network
	l_u8WaitRegistrationRetry = 0u;

	wdg_refresh();
	do
	{
		++l_u8WaitRegistrationRetry;
		l_u8ErrorCode = eBG96_SendCommand("AT+CGATT?", "+CGATT: 1", 0, l_au8ATRespBuffer);

		itsdk_delayMs(1000);
	}
	while ((l_u8WaitRegistrationRetry < 100) && (l_u8ErrorCode != BG96_ERROR_SUCCEED));

	// Launch connection
	eBG96_SendCommand("AT+COPS?", NULL, 0, l_au8ATRespBuffer);

	// Set APN
	eBG96_SendCommand("AT+QICSGP=1,1,\"NXT17.NET\",\"\",\"\",1", NULL, 0, l_au8ATRespBuffer);

	//Active PDP context
	eBG96_SendCommand("AT+QIACT=1", NULL, 0, l_au8ATRespBuffer);

	// Check for PDP context activated
	l_u8WaitRegistrationRetry = 0u;
	wdg_refresh();
	do
	{
		++l_u8WaitRegistrationRetry;
		l_u8ErrorCode = eBG96_SendCommand("AT+QIACT?", "+QIACT: 1", 0, l_au8ATRespBuffer);
		itsdk_delayMs(1000);
	}while((l_u8WaitRegistrationRetry < 100) && (l_u8ErrorCode != BG96_ERROR_SUCCEED));
}


/****************************************************************************************
 * Private functions
 ****************************************************************************************/

/****************************************************************************************
 * End Of File
 ****************************************************************************************/




