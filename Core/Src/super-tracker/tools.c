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
 * Description:   File description.
 *
 */

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include "super-tracker/tools.h"
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
uint8_t u8Tools_isStringInBuffer(uint8_t * p_pu8Buffer, uint8_t * p_pu8String)
{
	uint8_t l_u8isStringInBuffer = 0u;

	if (p_pu8String != NULL
		 && p_pu8Buffer != NULL)
	{
		if (strstr(p_pu8Buffer, p_pu8String) != NULL)
		{
			l_u8isStringInBuffer = 1u;
		}
	}

	return l_u8isStringInBuffer;
}

/****************************************************************************************
 * Private functions
 ****************************************************************************************/

/****************************************************************************************
 * End Of File
 ****************************************************************************************/



