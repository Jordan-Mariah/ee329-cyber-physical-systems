/*******************************************************************************
 * EE 329 A3 LCD TIMER
 *******************************************************************************
 * @file           : delay.h
 * @brief          : Keypad and LCD interfaced to create a countdown timer
 * project         : EE 329 F'24 Assignment 3
 * authors         : Mateo Charles - vcharles@calpoly.edu
 * date            : 10/14/24
 * compiler        : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 ******************************************************************************
 * @file           : delay.h
 * @brief          : Header for delay.c file.
 *                   This file contains the common defines and function
 *                   declarations of the application.
 *****************************************************************************/

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"

//function declarations
void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
