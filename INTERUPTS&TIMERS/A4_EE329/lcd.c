#include "lcd.h"
#include "delay.h"
#include <stdint.h>

uint8_t ascii;
void SysTick_Init(void);

// ------------------------------------------------------ excerpt from lcd.c ---
/* -----------------------------------------------------------------------------
* function : LCD_init
* Purpose  : initializes PF0-PF3 for the data line and PF7-PF8 for RS, RW,
*            and E. also initializes the LCD display
* input(s) : none
* output(s): none
* authors  : Jordan Reichhardt
* version  : 1.0
* date     : 240424
* -------------------------------------------------------------------------- */
void LCD_init(void) {
    // Enable the clock for GPIO Port F
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOFEN); // Enable clock to GPIO Port F

    // Configure LCD data pins (PF0 - PF3) as general-purpose output
    LCD_PORT->MODER &= ~((GPIO_MODER_MODE0 | // Clear PF0 - PF3 mode bits
                          GPIO_MODER_MODE1 |
                          GPIO_MODER_MODE2 |
                          GPIO_MODER_MODE3));
    LCD_PORT->MODER |= (GPIO_MODER_MODE0_0 | // Set PF0 - PF3 as general-purpose
                        GPIO_MODER_MODE1_0 | // output
                        GPIO_MODER_MODE2_0 |
                        GPIO_MODER_MODE3_0);

    // Configure output type for LCD data pins as push-pull
    LCD_PORT->OTYPER &= ~(GPIO_OTYPER_OT0 |
                          GPIO_OTYPER_OT1 |
                          GPIO_OTYPER_OT2 |
                          GPIO_OTYPER_OT3);

    // No pull-up/pull-down configuration for LCD data pins
    LCD_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD0 |
                         GPIO_PUPDR_PUPD1 |
                         GPIO_PUPDR_PUPD2 |
                         GPIO_PUPDR_PUPD3);

    // Set output speed for LCD data pins to high speed
    LCD_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED3_Pos));

    // Configure LCD control pins (PF7, PF8, PF9) for RS, RW, and E
    LCD_PORT->MODER &= ~((GPIO_MODER_MODE7 | // Clear PF7, PF8, PF9 mode bits
                          GPIO_MODER_MODE8 |
                          GPIO_MODER_MODE9));
    LCD_PORT->MODER |= (GPIO_MODER_MODE7_0 | // Set PF7, PF8, PF9 as
                        GPIO_MODER_MODE8_0 | // general-purpose output
                        GPIO_MODER_MODE9_0);

    // Configure output type for LCD control pins as push-pull
    LCD_PORT->OTYPER &= ~(GPIO_OTYPER_OT7 |
                          GPIO_OTYPER_OT8 |
                          GPIO_OTYPER_OT9);

    // No pull-up/pull-down configuration for LCD control pins
    LCD_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD7 |
                         GPIO_PUPDR_PUPD8 |
                         GPIO_PUPDR_PUPD9);

    // Set output speed for LCD control pins to high speed
    LCD_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED7_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED8_Pos) |
                          (3 << GPIO_OSPEEDR_OSPEED9_Pos));

    // Wait for LCD power-up to stabilize
    delay_us(40000);  // Wait for 40 ms

    // Initialize LCD in 4-bit mode
    for (int idx = 0; idx < 3; idx++) {
        LCD_4b_command(0x30);  // Send wake-up sequence
        delay_us(200);          // Wait 200 µs between commands
    }

    // Set LCD to 4-bit mode and configure display
    LCD_4b_command(0x20);    // Set LCD to 4-bit mode
    delay_us(400);           // Wait for 400 µs
    LCD_command(0x28);       // Function set: 4-bit mode, 2 lines, 5x8 dots
    delay_us(400);
    LCD_command(0x10);       // Set cursor move direction
    delay_us(400);
    LCD_command(0x0F);       // Display ON, cursor ON, blink ON
    delay_us(400);
    LCD_command(0x06);       // Entry mode set: Increment cursor, no shift
    delay_us(400);
    LCD_command(0x01);       // Clear display
    delay_us(400);
}

/* -----------------------------------------------------------------------------
* function : LCD_pulse_ENA
* Purpose  : pulses the enable (ENA) pin of the LCD to latch commands/data
* input(s) : none
* output(s): none
* authors  : Jordan Reichhardt
* version  : 1.0
* date     : 131024
* -------------------------------------------------------------------------- */
void LCD_pulse_ENA(void) {
    LCD_PORT->ODR |= (LCD_EN);   // Set ENA high
    delay_us(500);                // Wait for the ENA pulse duration
    LCD_PORT->ODR &= ~(LCD_EN);  // Set ENA low (falling edge triggers latch)
    delay_us(500);                // Wait for data to latch properly
}

/* -----------------------------------------------------------------------------
* function : LCD_4b_command
* Purpose  : sends a command in 4-bit mode to the LCD (higher nibble only)
* input(s) : uint8_t command - command to send to the LCD
* output(s): none
* authors  : Jordan Reichhardt
* version  : 1.0
* date     : 131024
* -------------------------------------------------------------------------- */
void LCD_4b_command(uint8_t command) {
    // Send the higher nibble of the command to the LCD
    LCD_PORT->ODR &= ~(LCD_DATA_BITS);  // Clear existing data bits
    LCD_PORT->ODR |= (command >> 4);    // Set the higher nibble of the command
    delay_us(500);
    LCD_pulse_ENA();  // Pulse the ENA line to latch the command
}

/* -----------------------------------------------------------------------------
* function : LCD_command
* Purpose  : sends an 8-bit command to the LCD in two 4-bit transfers
* input(s) : uint8_t command - command to send to the LCD
* output(s): none
* authors  : Jordan Reichhardt
* version  : 1.0
* date     : 131024
* -------------------------------------------------------------------------- */
void LCD_command(uint8_t command) {
    // Send a full 8-bit command using two 4-bit transfers
    LCD_PORT->ODR &= ~(LCD_DATA_BITS);  // Clear data bits
    LCD_PORT->ODR |= ((command >> 4) & LCD_DATA_BITS);  // Send higher nibble
    delay_us(500);
    LCD_pulse_ENA();  // Pulse ENA for higher nibble
    LCD_PORT->ODR &= ~(LCD_DATA_BITS);  // Clear data bits
    LCD_PORT->ODR |= (command & LCD_DATA_BITS);  // Send lower nibble
    delay_us(500);
    LCD_pulse_ENA();  // Pulse ENA for lower nibble
}

/* -----------------------------------------------------------------------------
* function : LCD_write_char
* Purpose  : writes a character (data) to the LCD for display
* input(s) : uint8_t letter - ASCII character to display
* output(s): none
* authors  : Jordan Reichhardt
* version  : 1.0
* date     : 131024
* -------------------------------------------------------------------------- */
void LCD_write_char(uint8_t letter) {
    LCD_PORT->ODR |= (LCD_RS);   // Set RS high (data mode)
    delay_us(500);
    LCD_command(letter);         // Send the character
    LCD_PORT->ODR &= ~(LCD_RS);  // Set RS low (command mode)
}

/* -----------------------------------------------------------------------------
* function : LCD_space
* Purpose  : writes a space character on the LCD
* input(s) : none
* output(s): none
* authors  : Jordan Reichhardt
* version  : 1.0
* date     : 131024
* -------------------------------------------------------------------------- */
void LCD_space(void) {
    LCD_write_char(' ');  // ASCII space character
}

/* -----------------------------------------------------------------------------
* function : LCD_clear
* Purpose  : clears the entire LCD display
* input(s) : none
* output(s): none
* authors  : Jordan Reichhardt
* version  : 1.0
* date     : 131024
* -------------------------------------------------------------------------- */
void LCD_clear(void) {
    LCD_command(0x01);  // Send clear display command
}

/* -----------------------------------------------------------------------------
* function : LCD_num
* Purpose  : grabs a specific number from an array for printing on LCD
* input(s) : number
* output(s): none
* authors  : Jordan Reichhardt
* version  : 1.0
* date     : 131024
* -------------------------------------------------------------------------- */
void LCD_num(uint8_t num) {
    // Select individual number
    ascii = (0b00110000) + num;
    LCD_write_char(ascii);
}

/* -----------------------------------------------------------------------------
* function : LCD_write_string
* Purpose  : writes a string to the LCD for display
* input(s) : char* str - pointer to the string to display
* output(s): none
* authors  : Jordan Reichhardt
* version  : 1.2
* date     : 171024
* -------------------------------------------------------------------------- */
void LCD_write_string(const char* str) {
    while (*str) {
        LCD_write_char((uint8_t)(*str));  // Write each character to the LCD
        str++;  // Move to the next character
    }
}
