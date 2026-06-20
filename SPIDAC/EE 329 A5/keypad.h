/*
 * keypad.h
 *
 *  Created on: Oct 2, 2024
 *  Author: asakd (Andrew Sakdikul)
 *  Class: EE 329 Fall 2024
 *  Assignment: A2
 *
 */
#include "stm32l4xx_hal.h"

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

// function prototypes
void Keypad_Config(void);
int Keypad_IsAnyKeyPressed(void);
int Keypad_WhichKeyIsPressed(void);

// macros
#define ROW_PORT (GPIOD)
#define COL_PORT (GPIOD)
#define KEYPAD_PORT (GPIOD)

#define ROW_PINS (0b00001111)
#define COL_PINS (0b01110000)

#define NUM_OF_ROWS (0b0100)
#define NUM_OF_COLS (0b0011)

// miscellaneous stuff that was in the sample code,
// that I didn't want to change
#define SETTLE (2000) //
#define BIT0 (0b00000001)
#define CODE_ZERO (0b0000) // pressing key 0 will light up as 15 on the LEDs,
						   // for A5, turn into 0000
#define KEY_ZERO (0b1011) // solved iKey eqn for position of zero key, was 11
#define NO_KEYPRESS (-1) // translates to false


#endif /* INC_KEYPAD_H_ */
