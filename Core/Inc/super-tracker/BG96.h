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
#ifndef INC_SUPER_TRACKER_BG96_H_
#define INC_SUPER_TRACKER_BG96_H_

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include "super-tracker/AT.h"

/****************************************************************************************
 * Defines
 ****************************************************************************************/

/****************************************************************************************
 * Type definitions
 ****************************************************************************************/
typedef enum _BG96_ERR_CODE_{
	BG96_ERROR_NONE,
   BG96_ERROR_PARAM,
	BG96_ERROR_SUCCEED,
	BG96_ERROR_FAILED,
	BG96_ERROR_TIMEOUT,
} e_BG96_ErrorCode_t;

typedef fp_vATCallback_t fp_BG96Callback_t;

/****************************************************************************************
 * Public function declarations
 ****************************************************************************************/
/**@brief      Send Command to BG96.
 * @param[in]  p_eCmd : Command.
 * @param[in]  p_pu8Bytes : Data to send.
 * @param[in]  p_u8Size : Size of message.
 * @param[in]  p_pfpCallback : Callback.
 * @return Error Code.
 */
e_BG96_ErrorCode_t eBG96_SendCommand(uint8_t * p_au8Cmd, uint8_t p_u8Size, fp_BG96Callback_t p_pfpCallback);

#endif /* INC_SUPER_TRACKER_BG96_H_ */
