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
static void vCallback_OnDemand(e_AT_RetVal_t p_eResult, uint8_t * p_pu8Buffer, uint8_t p_u8Size);

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
/**@brief Disconnect from network.
 * @return Error code
 */
void vCellular_Disconnect(void)
{
	eBG96_SendCommand(g_cau8Cmd[CELL_CMD_DISC], strlen(g_cau8Cmd[CELL_CMD_DISC]), vCallback_OnDemand);
}


/****************************************************************************************
 * Private functions
 ****************************************************************************************/
void vCallback_OnDemand(e_AT_RetVal_t p_eResult, uint8_t * p_pu8Buffer, uint8_t p_u8Size)
{
   switch(p_eResult)
   {
      case AT_RET_OK:
          log_info("SEND MSG TO BG96 SUCCESS\n");
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




