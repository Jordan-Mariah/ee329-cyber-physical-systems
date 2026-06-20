/*******************************************************************************
 * EE 329 A3 LCD TIMER
 *******************************************************************************
 * @file           : kpd.h
 * @brief          : Keypad and LCD interfaced to create a countdown timer
 * project         : EE 329 F'24 Assignment 3
 * authors         : Mateo Charles - vcharles@calpoly.edu
 * date            : 10/14/24
 * compiler        : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 ******************************************************************************
 * @file           : kpd.h
 * @brief          : Header for kpd.c file.
 *                   This file contains the common defines
 *                   and function declarations of the application.
 *****************************************************************************/
#include "stm32l4xx_hal.h"
void Keypad_Config(void);
int Keypad_IsAnyKeyPressed(void);
int Keypad_WhichKeyIsPressed(void);

#include <stdbool.h>


#ifndef INC_KPD_H
#define INC_KPD_H

#define ROW_PORT (GPIOD) //port D
#define COL_PORT (GPIOD)

#define ROW_PINS (0b00001111) //rows PD0-3
#define COL_PINS (0b01110000) //cols PD4-6

#define NUM_ROWS (0b0100)
#define NUM_COLS (0b0011)



#define KEYPAD_PORT (GPIOD)

#define BIT0 (0x00000001)
#define KEY_ZERO (0b1011)
#define CODE_ZERO (0b1111)
#define NO_KEYPRESS (-1)


//debounce
#define DEBOUNCE_STOP (15000)

#define SETTLE (2000)


#endif /* INC_KPD_H_ */
