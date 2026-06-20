#ifndef KEYPAD_H
#define KEYPAD_H

#include "stm32l4xx_hal.h"

// Define ports and pins
#define ROW_PORT    GPIOD  // Rows are connected to Port D
#define COL_PORT    GPIOA  // Columns are connected to Port A

#define ROW_PINS    (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)  // PD0 - PD3
#define COL_PINS    (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2)  // PA0 - PA2 for 4x3 keypad

#define NUM_ROWS    4
#define NUM_COLS    3

// Key values (you can change custom mappings)

#define DEBOUNCE_DELAY 10000
#define DEBOUNCE_COUNT 6


// Function prototypes
void Keypad_Config(void);  // One-time keypad configuration
int Keypad_IsAnyKeyPressed(void);  // Detect any keypress
int Keypad_WhichKeyIsPressed(void);  // Identify which key is pressed

#endif
