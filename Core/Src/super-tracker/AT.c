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

#include "super-tracker/ES_Queue.h"

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

#define AT_RX_BUFFER_SIZE  (uint8_t)255u

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

/* Buffer with data to receive */
static uint8_t g_au8UartInputQueueBufferData[AT_RX_BUFFER_SIZE];

/* Buffer manager */
static ES_Queue_Buffer_t g_sUartInputQueueBuffer = {
		.size = AT_RX_BUFFER_SIZE,
		.data = g_au8UartInputQueueBufferData
};

/* Queue manager */
static ES_Queue_t g_sUartInputQueue =  {
		.index_write = 0u,
		.index_read = 0u,
		.element_counter = 0u,
		.overlap_counter = 0u,
		.buffer = &g_sUartInputQueueBuffer
};

/****************************************************************************************
 * Public functions
 ****************************************************************************************/
/**@brief Initialize AT module.
 * @return None
 */
void vAT_Init(void)
{
	ES_Queue_Init(&g_sUartInputQueue, &g_sUartInputQueueBuffer, 0);
	HAL_UART_Receive_IT(&huart2, &g_u8UartRxChar, 1);
}

/**@brief Direct send AT command.
 * @param [in]  p_pu8Msg         : AT message to send
 * @param [in]  p_u8Size         : Size of AT message
 * @param [in/out] p_fpCallback  : Callback for response
 * @return None
 */
void vAT_Send(uint8_t * p_pu8Msg, uint8_t * p_pau8WaitResp, uint8_t p_u8WaitAsyncResp, fp_vATCallback_t p_fpCallback)
{
	uint8_t l_au8Buffer[MAX_SIZE_MSG] = { 0u };
	uint8_t l_u8Size = strlen(p_pu8Msg);
	uint8_t l_u8UartError = 1u;

	//1 case: refuse async si pas de reponse attendue
	if ((p_pau8WaitResp == NULL) && (p_u8WaitAsyncResp == 1u))
	{
		return;
	}


	memcpy(l_au8Buffer, p_pu8Msg, l_u8Size);
	// End of frame with one of any kind of whitespace (space,tab,newline,etc.)
	l_au8Buffer[l_u8Size++] = '\r';       //Increase index for Uart Length

	//Generate interrupt when receive a char on uart2
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

		itsdk_stimer_register(AT_TIMEOUT_MAX,vATTimeOutHandler,0,TIMER_ACCEPT_LOWPOWER);

		//Blocking mode until response or timeout
		while (g_u8WaitingReplyAT == 1u)
		{
			vAT_MessageProcess(p_pau8WaitResp, p_u8WaitAsyncResp);
			wdg_refresh();
		}
	}

}

/**@brief Manage messages.
 * @return None.
 */
void vAT_MessageProcess(uint8_t * p_pau8WaitResp, uint8_t p_u8WaitAsyncResp)
{
	uint8_t l_au8EnqueueBuffer[AT_RX_BUFFER_SIZE] = { 0u };
	uint16_t l_au8EnqueueSize = 0u;
	static uint8_t l_u8isPresent = 0u;
	static uint8_t l_u8isRespPresent = 0u;

	itsdk_stimer_run();

	//1 case: pas async
	//Si pas de rep attendue alors tester si OK present
	//Si OK alors CB OK, si non timeout
	//Si rep attendue alors tester si reponse puis tester si OK
	//Si OK et pas de reponse alors CB error
	//Si response mais pas OK, timeout.
	//Si OK et reponse alors CB OK

	//2 case: async
	//Tester si OK
	//Si ok alors attendre reponse
	//Si reponse alors CB OK, si non attendre jusqu'a timeout

	//Si pas ok alors timeout

	if (ES_Queue_Read(&g_sUartInputQueue, l_au8EnqueueBuffer, &l_au8EnqueueSize) == ES_Queue_Succeed)
	{
		if (p_pau8WaitResp != NULL)
		{
			// +cops: 1 > ok
			if (l_u8isRespPresent == 0u)
			{
				l_u8isRespPresent = u8Tools_isStringInBuffer(l_au8EnqueueBuffer, p_pau8WaitResp);
			}
		}
		else
		{
			// ok
			l_u8isRespPresent = 1u;
		}

		if (l_u8isPresent == 0u)
		{
			l_u8isPresent = u8Tools_isStringInBuffer(l_au8EnqueueBuffer,  "OK");
		}

		if ((l_u8isPresent == 1u))
		{
			if ((p_u8WaitAsyncResp == 0)
					|| ((p_u8WaitAsyncResp == 1) && (l_u8isRespPresent == 1u)))
			{
				if (l_u8isRespPresent == 1u)
				{
					(*g_fpCallback)(AT_RET_OK, l_au8EnqueueBuffer, l_au8EnqueueSize);
				}
				else
				{
					(*g_fpCallback)(AT_RET_ERROR, l_au8EnqueueBuffer, l_au8EnqueueSize);
				}

				g_u8WaitingReplyAT = 0u;
				l_u8isPresent = 0u;
				l_u8isRespPresent = 0u;

				//ES_Queue_ClearAll(&g_sUartInputQueue);
			}
		}
	}


	/*if(g_u8WaitingReplyAT == 1u)
   {
		if (ES_Queue_Read(&g_sUartInputQueue, l_au8EnqueueBuffer, &l_au8EnqueueSize) == ES_Queue_Succeed)
		{
			if (p_pau8WaitResp != NULL)
			{
				if (l_u8isRespPresent == 0u)
				{
					l_u8isRespPresent = u8Tools_isStringInBuffer(l_au8EnqueueBuffer, p_pau8WaitResp);
				}
			}
			else
			{
				l_u8isRespPresent = 1u;
			}

			if (l_u8isPresent == 0u)
			{
				l_u8isPresent = u8Tools_isStringInBuffer(l_au8EnqueueBuffer, "OK");
			}

			if ((l_u8isPresent == 1u))
			{
				itsdk_stimer_stop(vATTimeOutHandler,0);

				if((*g_fpCallback) != NULL)
				{
					if (l_u8isRespPresent == 1u)
					{
						(*g_fpCallback)(AT_RET_OK, l_au8EnqueueBuffer, l_au8EnqueueSize);

						g_u8WaitingReplyAT = 0u;
						l_u8isPresent = 0u;
						l_u8isRespPresent = 0u;

						ES_Queue_ClearAll(&g_sUartInputQueue);
					}
					else
					{
						if (p_u8WaitAsyncResp == 0u)
						{
							(*g_fpCallback)(AT_RET_ERROR, l_au8EnqueueBuffer, l_au8EnqueueSize);

							g_u8WaitingReplyAT = 0u;
							l_u8isPresent = 0u;
							l_u8isRespPresent = 0u;

							ES_Queue_ClearAll(&g_sUartInputQueue);
						}
					}
				}
			}
		}
      else
      {
			// Still pending transfer (Wait for reply or Timeout
		}
   }*/
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

	g_u8RxIdx++;

	if(g_au8ATBuffer[g_u8RxIdx-1] == g_au8ATEndOfFrame[g_u8EOFIdx])
	{
		g_u8EOFIdx++;

		if(g_u8EOFIdx >= AT_EOF_SIZE)
		{
			if (g_u8RxIdx > 2)
			{
				ES_Queue_Write(&g_sUartInputQueue, g_au8ATBuffer, g_u8RxIdx);
				g_u8RxIdx = 0u;
				memset(g_au8ATBuffer, 0u, AT_RX_BUFFER_SIZE);
			}
			g_u8EOFIdx = 0u;
		}
	}

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


