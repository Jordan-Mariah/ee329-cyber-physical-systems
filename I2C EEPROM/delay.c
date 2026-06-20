/* USER CODE BEGIN Header */
/*******************************************************************************
 * EE329 Fall 2024
 *******************************************************************************
 * @file			: delay.c
 * @brief		: delay program body
 * Project		: Lab A3: Parallel LCD
 * Author(s)	: Thomas McMillan (tem) - temcmill@calpoly.edu
 * 				:
 * Version		: 0.1
 * Date			: 10/09/2024
 * Compiler		: STM32CubeIDE Version: 1.16.0 Build: 21983_20240628_1741 (UTC)
 * Target		: NUCLEO-L4A6ZG
 * Citations	: Copied straight from EE329 Lab Manual (A3, page 15).
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
 *******************************************************************************
* Delay Plan:
* Code sets a X microsecond delay when function is called, where X is the
* specified number of microseconds wanted by the user (i.e. 1000 microseconds =
* 1 millisecond).
********************************************************************************
* GPIO
* No GPIO port needed for this function set.
********************************************************************************
* REVISION HISTORY
* 0.1 	241009 tem create
*
*******************************************************************************/
/* USER CODE END Header */

/* USER CODE BEGIN Functions */
// 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-2345678-8
#include "delay.h"

// --------------------------------------------------- delay.c w/o #includes ---
// configure SysTick timer for use with delay_us().
// warning: breaks HAL_delay() by disabling interrupts for shorter delay timing.
void SysTick_Init ( void ) {
SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk | // enable SysTick Timer
SysTick_CTRL_CLKSOURCE_Msk); // select CPU clock
SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk); // disable interrupt
}
// delay in microseconds using SysTick timer to count CPU clock cycles
// do not call with 0 : error, maximum delay.
// careful calling with small nums : results in longer delays than specified:
// e.g. @4MHz, delay_us(1) = 10=15 us delay.
void delay_us(const uint32_t time_us) {
// set the counts for the specified delay
SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
SysTick->VAL = 0; // clear timer count
SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk); // clear count flag
while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for flag
}

/* USER CODE END Functions */
