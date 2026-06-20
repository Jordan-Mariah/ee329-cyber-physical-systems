/* USER CODE BEGIN Header */
/*******************************************************************************
 * EE329 Fall 2024
 *******************************************************************************
 * @file			: I2C.h
 * @brief		: Header for I2C.c file.
 * Project		: Lab A9: I2C EEPROM
 * Author(s)	: Thomas McMillan (tem) - temcmill@calpoly.edu
 * 				: Jordan Reichhardt (jmr) - jreichha@calpoly.edu
 * Version		: 0.1
 * Date			: 11/06/2024
 * Compiler		: STM32CubeIDE Version: 1.16.0 Build: 21983_20240628_1741 (UTC)
 * Target		: NUCLEO-L4A6ZG
 * Citations	:
 *******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *******************************************************************************
* REVISION HISTORY
* 0.1 	241106 tem create
*
*******************************************************************************/
// 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-2345678-8
/* USER CODE END Header */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "stm32l4xx_hal.h"
#include "delay.h"

#define EEPROM_ADDRESS (0b1010111) // "0x57" EEPROM I2C address

void EEPROM_Init ( void );
void EEPROM_Write ( uint8_t data, uint16_t addr);
uint8_t EEPROM_Read ( uint16_t address );

#endif /* INC_I2C_H_ */
