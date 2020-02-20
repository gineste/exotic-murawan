/*
 * AT.h
 *
 *  Created on: 17 févr. 2020
 *      Author: Martin
 */

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
 */

#ifndef INC_SUPER_TRACKER_AT_H_
#define INC_SUPER_TRACKER_AT_H_

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include <stdint.h>

/****************************************************************************************
 * Defines
 ****************************************************************************************/

/****************************************************************************************
 * Type definitions
 ****************************************************************************************/
typedef enum _AT_RETURN_VAL_ {
   AT_RET_OK,
   AT_RET_ERROR,
   AT_RET_TIMEOUT,
   AT_RET_END
}e_AT_RetVal_t;

typedef void (*fp_vATCallback_t)(e_AT_RetVal_t p_eRet, uint8_t * p_pu8Buffer, uint8_t p_u8Size);

/****************************************************************************************
 * Public function declarations
 ****************************************************************************************/
/**@brief Initialize AT module.
 * @return None
 */
void vAT_Init(void);
void vAT_Send(uint8_t * p_pu8Msg, uint8_t * p_pau8WaitResp, uint8_t p_u8WaitAsyncResp, fp_vATCallback_t p_fpCallback);
void vAT_MessageProcess(uint8_t * p_pau8WaitResp);
void vAT_UpdateFrame(const uint8_t p_u8Data);
uint8_t u8AT_PendingCommand(void);
uint8_t u8AT_PendingReply(void);
void vAT_ClearPending(void);

#endif /* INC_SUPER_TRACKER_AT_H_ */


