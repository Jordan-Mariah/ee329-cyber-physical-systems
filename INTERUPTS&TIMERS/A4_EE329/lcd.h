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
void LCD_write_string(const char* str);
//void LCD_EN(void);
//define
#define LCD_PORT (GPIOF)
#define LCD_RS (GPIO_PIN_8)
#define LCD_RW (GPIO_PIN_7)
#define LCD_EN (GPIO_PIN_9)
#define LCD_DATA_BITS (GPIO_PIN_0 | GPIO_PIN_1 |GPIO_PIN_2 | GPIO_PIN_3)
#endif /* INC_LCD_H_ */
