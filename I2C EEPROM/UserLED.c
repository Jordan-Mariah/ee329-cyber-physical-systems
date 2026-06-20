/* USER CODE BEGIN Header */
/*******************************************************************************
 * EE329 Fall 2024
 *******************************************************************************
 * @file			: UserLED.c
 * @brief		: UserLED program body
 * Project		: Lab A4: Interrupts and Timers
 * Author(s)	: Thomas McMillan (tem) - temcmill@calpoly.edu
 * 				:
 * Version		: 0.1
 * Date			: 10/19/2024
 * Compiler		: STM32CubeIDE Version: 1.16.0 Build: 21983_20240628_1741 (UTC)
 * Target		: NUCLEO-L4A6ZG
 * Citations	:
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
* A4 Project Plan - User LEDs:
* Set up separate init functions and toggle functions for the on-board user
* LEDs: one blue and one red.
********************************************************************************
* GPIO Plan:
* No physical connection required.  LEDs are hard-lined to the GPIO pins. The
* blue LED uses Port B pin 7, and the red LED uses Port B pin 14.
********************************************************************************
* REVISION HISTORY
* 0.1 	241019 tem create
*
*******************************************************************************/
// 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-2345678-8
/* USER CODE END Header */

/* USER CODE BEGIN includes */
#include "UserLED.h"
#include "stm32l4xx_hal.h"
/* USER CODE END includes */

// Blue LED on pin 7 of port B
// Red LED on pin 14 of port B

/* USER CODE BEGIN functions */
/* -----------------------------------------------------------------------------
* function : Initialize_Blue ( )
* INs : none
* OUTs : none
* action : Set up GPIO port config for pin 7. Set to output mode, push-pull, no
* 				PUPD resistor, high speed.
* authors : Thomas McMillan (tem) - temcmill@calpoly.edu
* 			 : Miles A. Hunt (mah) - mhunt15@calpoly.edu
* version : 0.1
* date : 10/19/2024
* citations:
* --------------------------------------------------------------------------- */
void Initialize_Blue (void) {
	RCC->AHB2ENR	|= (RCC_AHB2ENR_GPIOBEN);
	GPIOB->MODER &= ~ (MODE7);
	GPIOB->MODER |= (MODE7_0);
	GPIOB->OTYPER 	&= ~(OT7);
	GPIOB->PUPDR 	&= ~(PUPD7);
	GPIOB->OSPEEDR 	|= (3 << OSPD7);
	GPIOB->BRR = (GPIO_PIN_7);
}

// 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-2345678-8
/* -----------------------------------------------------------------------------
* function : Initialize_Red ( )
* INs : none
* OUTs : none
* action : Set up GPIO port config for pin 14. Set to output mode, push-pull, no
* 				PUPD resistor, high speed.
* authors : Thomas McMillan (tem) - temcmill@calpoly.edu
* 			 : Miles A. Hunt (mah) - mhunt15@calpoly.edu
* version : 0.1
* date : 10/19/2024
* citations:
* --------------------------------------------------------------------------- */
void Initialize_Red (void) {
	RCC->AHB2ENR	|= (RCC_AHB2ENR_GPIOBEN);
	GPIOB->MODER &= ~ (MODE14);
	GPIOB->MODER |= (MODE14_0);
	GPIOB->OTYPER 	&= ~(OT14);
	GPIOB->PUPDR 	&= ~(PUPD14);
	GPIOB->OSPEEDR 	|= (3 << OSPD14);
	GPIOB->BRR = (GPIO_PIN_14);
}

// 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-2345678-8
/* -----------------------------------------------------------------------------
* function : Toggle_Blue ( ) or Toggle_Red ( )
* INs : none
* OUTs : none
* action : Both functions below are identical, except one toggles on/off the
* 				blue LED and the other toggles on/off the red LED.
* authors : Thomas McMillan (tem) - temcmill@calpoly.edu
* 			 : Miles A. Hunt (mah) - mhunt15@calpoly.edu
* version : 0.1
* date : 10/19/2024
* citations:
* --------------------------------------------------------------------------- */
void Toggle_Blue (void) {
	GPIOB->ODR ^= (GPIO_PIN_7);
}

void Toggle_Red (void) {
	GPIOB->ODR ^= (GPIO_PIN_14);
}

// End of code
/* USER CODE END functions */
