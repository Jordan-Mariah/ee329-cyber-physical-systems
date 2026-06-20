#ifndef MAIN_H
#define MAIN_H

#include "stm32l4xx_hal.h"
#include "lcd.h"  // Include the LCD header for LCD display functions
#include "delay.h" // Include the delay header for delay functions

// Pin definitions
#define BUTTON_PIN 13   // connect onboard button to PC13
#define BUTTON_PORT GPIOC  // GPIOC port
#define LED_PIN 7      // Blue LED is connected to PB7
#define LED_PORT GPIOB  // GPIOB port (for blue LED)

// Global variables
extern volatile uint8_t reaction_triggered;
extern volatile uint32_t start_time;
extern volatile uint32_t end_time;
extern volatile uint8_t random_delay_done;
extern volatile uint8_t button_pressed;
extern volatile float reaction_time;
extern char time_str[10];

// Function prototypes
void SystemClock_Config(void);   // Configure the system clock
void Error_Handler(void);        // Handle errors
void generate_random_delay(void); // Generate random delay using TIM2
int is_button_pressed(void);     // Check if the button is pressed + debounce
void turn_on_led(void);          // Turn on the blue LED
void turn_off_led(void);         // Turn off the blue LED
void setup_TIM2(void);           // Setup TIM2 (initializing the clock/ timing)
void display_initial_screen(void); // Display the initial message on the LCD

// Finite state machine state initializations
typedef enum {
    INIT,
    WAIT_FOR_RANDOM_DELAY,
    WAIT_FOR_SECOND_PRESS,
    REACTION_COMPLETE
} State;

#endif // MAIN_H
