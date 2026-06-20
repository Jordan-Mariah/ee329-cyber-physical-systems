/*******************************************************************************
 * EE 329 A3 LCD TIMER
 *******************************************************************************
 * @file           : lcd.h
 * @brief          : Keypad and LCD interfaced to create a countdown timer
 * project         : EE 329 F'24 Assignment 3
 * authors         : Mateo Charles - vcharles@calpoly.edu
 * date            : 10/14/24
 * compiler        : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 ******************************************************************************
 * @file           : lcd.h
 * @brief          : Header for lcd.c file.
 *                   This file contains the common defines and function
 *                   declarations of the application.
 *****************************************************************************/

#include "stm32l4xx_hal.h"
#ifndef INC_LCD_H_
#define INC_LCD_H_

//function declarations
void LCD_init( void );
void LCD_pulse_ENA( void );
void LCD_4b_command( uint8_t command);
void LCD_command( uint8_t command );
void LCD_write_char( uint8_t letter );
void delay_us();
void LCD_init(void);
void LCD_write_string( const char *str);

//define
#define LCD_PORT (GPIOF) //using port F

#define LCD_RS (GPIO_PIN_8) //PF7-9
#define LCD_RW (GPIO_PIN_7)
#define LCD_EN (GPIO_PIN_9)

//PF0-3
#define LCD_DATA_BITS (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)

#endif /* INC_LCD_H_ */


