/* USER CODE BEGIN Header */
/*
 * *************************************************************************
 * Attributions:
 *
 * Main Body of Code received from Keigan Leonard
 * Minor Personalizations and alterations to fit specific 4x3 keypad by wml
 *
 ***************************************************************************
 *
 * keypad.h
 *
 *  Created on: Oct 2, 2024
 *      Author: William Leiker (wml)
 *
 *  Purpose:
 *  Initialize the L4A6ZG pins for the Cols and rows associated with
 *  the keypad
 *
 *  Description:
 *  1. Columns are given the A pins
 *  2. Rows are given the F pins
 *
 ***************************************************************************
*/
//345678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-23456
/* USER CODE END Header */

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#define clr (0xf)

#define KYPD_Port (GPIOD)			// port used by keypad

// function prototyping
int Keypad_IsAnyKeyPressed(void);
int Keypad_WhichKeyIsPressed(void);
void Keypad_Config(void);
int Grab_KeyPress(void);

int debounce(int key);
#define deb_cnt (8)

/*
Keypad Pins left-to-right:
1, 2, 3, 4, 5, 6, 7

Rows (top-to-bottom): 2, 7, 6, 4
Columns (left-to-right):  3, 1, 5
 */

// Columns
#define c1 (1<<0)
#define c2 (1<<1)
#define c3 (1<<2)
#define COL_PINS (c1|c2|c3)

// Rows
#define r1 (1<<3)
#define r2 (1<<4)
#define r3 (1<<5)
#define r4 (1<<6)
#define ROW_PINS (r1|r2|r3|r4)


#define KEY_ZERO (11) 		//checks output of special function
#define CODE_ZERO (0) 		//LED value of 0

#define KEY_STAR (10) 		//checks output of special function
#define CODE_STAR (14) 		//LED value of 14

#define KEY_POUND (12) 		//checks output of special function
#define CODE_POUND (15) 	//LED value of 15

#define NO_KEYPRESS (-1)

#define NUM_COLS (3)
#define NUM_ROWS (4)
#define SETTLE (1000)

#define TRUE (1)
#define FALSE (0)

#endif /* INC_KEYPAD_H_ */
