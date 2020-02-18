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
#ifndef INC_SUPER_TRACKER_CELLULAR_H_
#define INC_SUPER_TRACKER_CELLULAR_H_

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include <stdint.h>

/****************************************************************************************
 * Defines
 ****************************************************************************************/
#define CELL_PREFIX_CMD_SIZE_MAX 10

/****************************************************************************************
 * Type definitions
 ****************************************************************************************/
typedef enum _CELLULAR_ERR_CODE_{
	CELLULAR_ERROR_NONE,
   CELLULAR_ERROR_PARAM,
	CELLULAR_ERROR_FAILED,
	CELLULAR_ERROR_TIMEOUT
}e_Cellular_ErrorCode_t;

typedef enum _CELLULAR_COMMANDS_ {
   CELL_CMD_DISC,       	// Disconnect
   CELL_CMD_CHK_DISC,		// Check if disconnected
   CELL_CMD_CONN_BEST_OP,	// Connect to the best operator available
   CELL_CMD_CHK_PS,			// Check if connected
   CELL_CMD_MAX
}e_Cellular_Commands_t;

/****************************************************************************************
 * Public function declarations
 ****************************************************************************************/
/**@brief Disconnect from network.
 * @return Error code
 */
void vCellular_Disconnect(void);

#endif /* INC_SUPER_TRACKER_CELLULAR_H_ */
