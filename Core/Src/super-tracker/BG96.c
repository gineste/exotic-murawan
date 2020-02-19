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
 * Description:   BG96 drivers.
 *
 */

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include <stdint.h>
#include <string.h>

#include "super-tracker/BG96.h"
/****************************************************************************************
 * Defines
 ****************************************************************************************/

/****************************************************************************************
 * Private type declarations
 ****************************************************************************************/

/****************************************************************************************
 * Private function declarations
 ****************************************************************************************/
static void vCallback_OnDemand(e_AT_RetVal_t p_eResult, uint8_t * p_pu8Buffer, uint8_t p_u8Size);

/****************************************************************************************
 * Variable declarations
 ****************************************************************************************/ 
uint8_t g_u8ATretVal = AT_RET_ERROR;
uint8_t g_au8ATRespBuffer[BG96_RESP_SIZE_MAX] = {0};

/****************************************************************************************
 * Public functions
 ****************************************************************************************/ 
/**@brief      Send Command to BG96.
 * @param[in]  p_eCmd : Command.
 * @param[in/out]  p_au8Buffer : Buffer to store the response.
 * @return Error Code.
 */
e_BG96_ErrorCode_t eBG96_SendCommand(uint8_t * p_au8Cmd, uint8_t p_u8WaitAsyncResp, uint8_t * p_au8Buffer)
{
   e_BG96_ErrorCode_t l_eErrCode = BG96_ERROR_PARAM;
   uint8_t l_u8ATRespSize = 0u;

   if (p_au8Buffer != NULL
   		&& p_au8Cmd != NULL)
   {
		vAT_Send(p_au8Cmd, p_u8WaitAsyncResp, (fp_vATCallback_t)vCallback_OnDemand);

		if (g_u8ATretVal == AT_RET_OK)
		{
			l_eErrCode = BG96_ERROR_NONE;
			l_u8ATRespSize = strlen(g_au8ATRespBuffer);
			memset(p_au8Buffer, 0u, BG96_RESP_SIZE_MAX);
			memcpy(p_au8Buffer, g_au8ATRespBuffer, l_u8ATRespSize);
		}
		else if (g_u8ATretVal == AT_RET_TIMEOUT)
		{
			l_eErrCode = BG96_ERROR_TIMEOUT;
		}
		else
		{
			l_eErrCode = BG96_ERROR_FAILED;
		}
   }

	return l_eErrCode;
}

/****************************************************************************************
 * Private functions
 ****************************************************************************************/
void vCallback_OnDemand(e_AT_RetVal_t p_eResult, uint8_t * p_pu8Buffer, uint8_t p_u8Size)
{
	g_u8ATretVal = p_eResult;
	memset(g_au8ATRespBuffer, 0u, BG96_RESP_SIZE_MAX);
   switch(p_eResult)
   {
      case AT_RET_OK:
         //log_info("SEND MSG TO BG96 SUCCESS\n");
			memcpy(g_au8ATRespBuffer, p_pu8Buffer, p_u8Size);
         break;
      case AT_RET_TIMEOUT:
			log_info("SEND MSG TO BG96 TIMEOUT\n");
			break;
      default:
          log_info("FAIL TO SEND MSG TO BG96\n");
      break;
   }
}

/****************************************************************************************
 * End Of File
 ****************************************************************************************/




