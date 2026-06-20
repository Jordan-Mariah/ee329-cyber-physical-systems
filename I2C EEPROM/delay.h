/* USER CODE BEGIN Header */
/******************************************************************************
 * EE329 Fall 2024
 ******************************************************************************
 * @file			: delay.h
 * @brief		: Header for delay.c file.
 * Project		: Lab A3: Parallel LCD
 * Author(s)	: Thomas McMillan (tem) - temcmill@calpoly.edu
 * 				:
 * Version		: 0.1
 * Date			: 10/09/2024
 * Compiler		: STM32CubeIDE Version: 1.16.0 Build: 21983_20240628_1741 (UTC)
 * Target		: NUCLEO-L4A6ZG
 * Citations	: Copied straight from EE329 Lab Manual (A3, page 15). The
 * 					header file is mostly blank, only declared prototypes.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
* REVISION HISTORY
* 0.1 	241009 tem create
*
******************************************************************************/
/* USER CODE END Header */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

/* USER CODE BEGIN includes */
#include "stm32l4xx_hal.h"
/* USER CODE END includes */

/* USER CODE BEGIN prototypes */
void SysTick_Init(void);
void delay_us(const uint32_t time_us);
/* USER CODE END prototypes */

#endif /* INC_DELAY_H_ */
