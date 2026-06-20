/* USER CODE BEGIN Header */
/*******************************************************************************
 * EE329 Fall 2024
 *******************************************************************************
 * @file			: UserLED.h
 * @brief		: Header for UserLED.c file.
 * Project		: Lab A4: Interrupts and Timers
 * Author(s)	: Thomas McMillan (tem) - temcmill@calpoly.edu
 * 				:
 * Version		: 0.1
 * Date			: 10/19/2024
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
* 0.1 	241019 tem create
*
*******************************************************************************/
// 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-2345678-8
/* USER CODE END Header */

/* USER CODE BEGIN user_led_header */
#ifndef INC_USERLED_H_
#define INC_USERLED_H_
#include "stm32l4xx_hal.h"

//#define USER_LED_PORT (GPIOB)

#define SEC_DELAY (1000000)

#define MODE7 (GPIO_MODER_MODE7)
#define MODE14 (GPIO_MODER_MODE14)

#define MODE7_0 (GPIO_MODER_MODE7_0)
#define MODE14_0 (GPIO_MODER_MODE14_0)

#define OT7 (GPIO_OTYPER_OT7)
#define OT14 (GPIO_OTYPER_OT14)

#define PUPD7 (GPIO_PUPDR_PUPD7)
#define PUPD14 (GPIO_PUPDR_PUPD14)

#define OSPD7 (GPIO_OSPEEDR_OSPEED7_Pos)
#define OSPD14 (GPIO_OSPEEDR_OSPEED14_Pos)

void Initialize_Blue (void);
void Initialize_Red (void);
void Toggle_Blue (void);
void Toggle_Red (void);

#endif /* INC_USERLED_H_ */
/* USER CODE END user_led_header */
