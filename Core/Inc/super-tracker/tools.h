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
#ifndef INC_SUPER_TRACKER_TOOLS_H_
#define INC_SUPER_TRACKER_TOOLS_H_

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include <string.h>
#include <stdint.h>

/****************************************************************************************
 * Defines
 ****************************************************************************************/

/****************************************************************************************
 * Type definitions
 ****************************************************************************************/

/****************************************************************************************
 * Public function declarations
 ****************************************************************************************/
/**@brief      Check if a string is present in a buffer.
 * @param[in]  p_pu8Buffer : This is the main C string to be scanned.
 * @param[in]  p_pu8String : This is the small string to be searched with-in haystack string.
 * @return 1 if present, 0 if not.
 */
uint8_t u8Tools_isStringInBuffer(uint8_t * p_pu8Buffer, uint8_t * p_pu8String);

#endif /* INC_SUPER_TRACKER_TOOLS_H_ */
