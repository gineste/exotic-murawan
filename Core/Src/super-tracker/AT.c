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
 * Date:          17/02/2020 (dd MM YYYY)
 * Author:        Martin CORNU
 * Description:   AT Interface to Send command.
 *
 */

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include <stdint.h>
#include <string.h>

#include <usart.h>

#include <it_sdk/time/timer.h>
#include <it_sdk/logger/logger.h>

#include "super-tracker/AT.h"

/****************************************************************************************
 * Defines
 ****************************************************************************************/
#define AT_TIMEOUT_MIN     (uint32_t)10000u
#define AT_TIMEOUT_MAX     (uint32_t)60000u

#define AT_EOF_SIZE        (uint8_t)2u

#define PREFIX_MSG         "AT"
#define PREFIX_MSG_SIZE    (uint8_t)sizeof(PREFIX_MSG)+4u
#define TX_MSG_SIZE        (uint8_t)28u
#define MAX_SIZE_MSG       (uint8_t)TX_MSG_SIZE + PREFIX_MSG_SIZE

#define AT_RX_BUFFER_SIZE  (uint8_t)64u

/****************************************************************************************
 * Private type declarations
 ****************************************************************************************/


/****************************************************************************************
 * Private function declarations
 ****************************************************************************************/
static void vATTimeOutHandler(void * p_pvParameters);

/****************************************************************************************
 * Variable declarations
 ****************************************************************************************/
volatile uint8_t g_u8WaitingReplyAT = 0u;
volatile uint8_t g_u8MsgReceived = 0u;
volatile uint8_t g_u8MultipleReply = 0u;

volatile uint8_t g_u8RxIdx = 0u;
volatile uint8_t g_u8EOFIdx = 0u;

static uint8_t g_au8ATBuffer[AT_RX_BUFFER_SIZE] = { 0u };
static uint8_t g_au8ATEndOfFrame[AT_EOF_SIZE] = {'\r', '\n' }; //AT_END_OF_FRAME;
static fp_vATCallback_t g_fpCallback;

static uint8_t g_u8UartRxChar = '\0';

/****************************************************************************************
 * Public functions
 ****************************************************************************************/

/**@brief Direct send AT command.
 * @param [in]  p_pu8Msg         : AT message to send
 * @param [in]  p_u8Size         : Size of AT message
 * @param [in/out] p_fpCallback  : Callback for response
 * @return None
 */
void vAT_Send(uint8_t * p_pu8Msg, uint8_t p_u8WaitAsyncResp, fp_vATCallback_t p_fpCallback)
{
   uint8_t l_au8Buffer[MAX_SIZE_MSG] = { 0u };
   uint8_t l_u8Size = strlen(p_pu8Msg);
   uint8_t l_u8UartError = 1u;

   memcpy(l_au8Buffer, p_pu8Msg, l_u8Size);
   // End of frame with one of any kind of whitespace (space,tab,newline,etc.)
   l_au8Buffer[l_u8Size++] = '\r';       //Increase index for Uart Length

   //Generate interrupt when receive a char on uart2
	if (HAL_UART_Receive_IT(&huart2, &g_u8UartRxChar, 1) == HAL_OK)
	{
		/* Clear Cursor and buffer */
		/* Init Cursor Rx Buffer Idx */
      g_u8RxIdx = 0u;
      g_u8EOFIdx = 0u;
      memset(g_au8ATBuffer, 0u, AT_RX_BUFFER_SIZE);

		if(HAL_UART_Transmit(&huart2, l_au8Buffer, l_u8Size, AT_TIMEOUT_MAX) == HAL_OK)
		{
			l_u8UartError = 0u;
			g_fpCallback = p_fpCallback;
			g_u8WaitingReplyAT = 1u;

			if (p_u8WaitAsyncResp)
			{
				g_u8MultipleReply = 3u;
			}
			else
			{
				g_u8MultipleReply = 0u;	//TO REFACTOR
			}

			itsdk_stimer_register(AT_TIMEOUT_MAX,vATTimeOutHandler,0,TIMER_ACCEPT_LOWPOWER);

			//Blocking mode until response or timeout
			while ((g_u8WaitingReplyAT == 1u) || (g_u8MultipleReply != 0u))
			{
					vAT_MessageProcess();
					wdg_refresh();
			}
		}

		HAL_UART_AbortReceive_IT(&huart2);
	}
	else
	{
		log_info("UART ERROR\n");
	}
}

/**@brief Manage messages.
 * @return None.
 */
void vAT_MessageProcess(void)
{
   itsdk_stimer_run();

   if((g_u8WaitingReplyAT == 1u) || (g_u8MultipleReply != 0u))
   {
      if(g_u8MsgReceived != 0u)
      {
         if(g_u8MultipleReply != 0u)
         {
            g_u8MultipleReply--;
            g_u8MsgReceived = 0u;
         }
         else
         {
            if (u8Tools_isStringInBuffer(g_au8ATBuffer, "OK"))
            {
            	itsdk_stimer_stop(vATTimeOutHandler,0);
            	g_u8WaitingReplyAT = 0u;
            	g_u8MsgReceived = 0u;

   				if((*g_fpCallback) != NULL)
   				{
   					(*g_fpCallback)(AT_RET_OK, g_au8ATBuffer, g_u8RxIdx);
   				}
            }
         }
      }
      else
      {  /* Still pending transfer (Wait for reply or Timeout */  }
   }
}

/**@brief Callback function handler for AT command.
 * @param[in] p_u8Data : Input Char.
 * @return None
 */
void vAT_UpdateFrame(const uint8_t p_u8Data)
{
	g_au8ATBuffer[g_u8RxIdx] = p_u8Data;
#if (ITSDK_LOGGER_CONF > 0)
   log_info("%c", p_u8Data);
#endif

   if(g_au8ATBuffer[g_u8RxIdx] == g_au8ATEndOfFrame[g_u8EOFIdx])
	{
      g_u8EOFIdx++;
      if(g_u8EOFIdx >= AT_EOF_SIZE)
      {
      	if (g_u8RxIdx > 2)
      	{
      		g_u8MsgReceived++;
      	}
         g_u8EOFIdx = 0u;
      }
   }

   g_u8RxIdx++;
   if(g_u8RxIdx > AT_RX_BUFFER_SIZE)
   {
      g_u8RxIdx = 0u;
   }
}

/**@brief  Check if there are pending message to transmit.
 * @return 1 if pending command else 0.
 */
uint8_t u8AT_PendingCommand(void)
{
   return (g_u8WaitingReplyAT == 1u)?1u:0u;
}

uint8_t u8AT_PendingReply(void)
{
   return g_u8MultipleReply;
}

void vAT_ClearPending(void)
{
   g_u8MsgReceived = 0u;
   g_u8MultipleReply = 0u;
   g_u8WaitingReplyAT = 0u;
}


/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	vAT_UpdateFrame(g_u8UartRxChar);
	HAL_UART_Receive_IT(&huart2, &g_u8UartRxChar, 1);
}

/****************************************************************************************
 * Private functions
 ****************************************************************************************/
/**@brief  Time out Handler.
 * @param[in] p_pvParameters : Information about timeout
 * @return None.
 */
static void vATTimeOutHandler(void * p_pvParameters)
{
   if(g_fpCallback != NULL)
   {
      (*g_fpCallback)(AT_RET_TIMEOUT, NULL, NULL);
   }
   /* We are not waiting reply anymore */
   g_u8WaitingReplyAT = 0u;
   g_u8MultipleReply = 0u;
}

/****************************************************************************************
 * End Of File
 ****************************************************************************************/


