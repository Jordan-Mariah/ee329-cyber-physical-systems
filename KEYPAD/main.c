/* USER CODE BEGIN Header */
/**

  ******************************************************************************

  * @file           : main.c
  * @brief          : Main program body
  * Project         : EE 329 A1 Assignments 2a, 1b
  * Authors         : Jordan Reichhardt -jordan.reichhardt@gmail.com
  * 				: Bryce Paulson - brycepaulson03@gmail.com
  * Version         : 5
  * Date            : 9/29/24
  ******************************************************************************

  * Plan: Kepad and Debounce
  * 1. Count to 15 in binary on LEDs
  * Iterate through this 3 times then start execution timing

  ******************************************************************************
  LED wiring:
  *PC0- A1 (LSB)
  *PC1- A3
  *PC2- A7
  *PC3- A4
  */

//345678901234567890123456789012345678901234567890123456789012345678901234567890

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "keypad.h"
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
/* Defines ------------------------------------------------------------------*/
#define COL_PINS    (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2)  // PA0 - PA2 for columns
// PD0 - PD3 for rows
#define ROW_PINS    (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3)
#define NoPress 0xAA  // give value if no key was pressed(doesn't really matter the value
//just needs to be different than all of the key presses so make a two digit hex)

void SystemClock_Config(void);
void MX_GPIO_Init(void);  // Remove static because we need to define this function
int AnyKey(void);
void delay(int count);  // allows delay to be used before delay is defined
void DisplayKeyOnLED(uint8_t key);


int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();

      // Enable clock for GPIOC
      RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;// Configure PC0-PC3 as output
      // Clear mode for PC0-PC3
      GPIOC->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
      // Set to output
      GPIOC->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0);
      // Set to push-pull for PC0-PC3
      GPIOC->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
      // No pull-up, no pull-down
      GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
      GPIOC->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
                         (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
                         (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
                         (3 << GPIO_OSPEEDR_OSPEED3_Pos));  // Set high speed for PC0-PC3
     GPIOC->BRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);  // Turn off all LEDs pc0-pc3

    uint8_t lastKeyPressed = NoPress;  // Store the last pressed key value

    while (1) {
        // Check if any key is pressed
        if (AnyKey()) {
            // Get the pressed key
            int key = WhichKey();

            // Map each key to its corresponding 4-bit value using if statements
            if (key == 1) {
                lastKeyPressed = 0x1;  // Key 1
            } else if (key == 2) {
                lastKeyPressed = 0x2;  // Key 2
            } else if (key == 3) {
                lastKeyPressed = 0x3;  // Key 3
            } else if (key == 4) {
                lastKeyPressed = 0x4;  // Key 4
            } else if (key == 5) {
                lastKeyPressed = 0x5;  // Key 5
            } else if (key == 6) {
                lastKeyPressed = 0x6;  // Key 6
            } else if (key == 7) {
                lastKeyPressed = 0x7;  // Key 7
            } else if (key == 8) {
                lastKeyPressed = 0x8;  // Key 8
            } else if (key == 9) {
                lastKeyPressed = 0x9;  // Key 9
            } else if (key == 10) {
                lastKeyPressed = 0xF;  // Key *
            } else if (key == 11) {
                lastKeyPressed = 0x0;  // Key 0
            } else if (key == 12) {
                lastKeyPressed = 0xF;  // Key #
            }
            // Display the current key on the LEDs
            DisplayKeyLED(lastKeyPressed);
        } else {
            // If no key is pressed, keep displaying the last key value
            if (lastKeyPressed != NoPress) {
                DisplayKeyLED(lastKeyPressed);
            }
        }

        delay(200);  // Add a small delay between keypad checks
    }
}

void DisplayKeyLED(uint8_t key) {
    // Clear all LEDs (PC0-PC3) first by resetting their corresponding bits in the GPIOC BRR register
	//This code is from lab A1 and is initially modified in structure from chatGPT
    GPIOC->BRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    // Set the appropriate LEDs based on the 4-bit key value (this code is straight from A1)
    if (key & 0x1) GPIOC->BSRR = GPIO_PIN_0;  // Set PC0 if bit 0 is set in 'key'
    if (key & 0x2) GPIOC->BSRR = GPIO_PIN_1;  // Set PC1 if bit 1 is set in 'key'
    if (key & 0x4) GPIOC->BSRR = GPIO_PIN_2;  // Set PC2 if bit 2 is set in 'key'
    if (key & 0x8) GPIOC->BSRR = GPIO_PIN_3;  // Set PC3 if bit 3 is set in 'key'
}

void delay(int count) {  // delay num is the number of CPU cycles to wait
    for (int delay_num = 0; delay_num < count; delay_num++) {
        __NOP(); // no operation
    }
}

void SystemClock_Config(void)
{
    // System clock configuration code (auto-generated by CubeMX or defined by user)
}

/**
  * @brief GPIO Initialization Function
  * @retval None
  */
void MX_GPIO_Init(void)
{
    // Enable GPIO clocks (assuming using Port A and Port D)
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIODEN);
    // --------- Configure ROW pins (PD0 - PD3) as INPUTS with pull-down ---------
    // Clear mode for PD0-PD3 (input mode)
    GPIOD->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    // Clear pull-up/pull-down for PD0-PD3
    GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
    // Set pull-down resistors for PD0-PD3
    GPIOD->PUPDR |= (GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD1_1 | GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD3_1);


    // --------- Configure COL pins (PA0 - PA2) as OUTPUTS (push-pull, no pull-up/down) ---------
    // Clear mode for PA0-PA2 (output mode
    GPIOA->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 | GPIO_MODER_MODE2);
    // Set to output mode for PA0-PA2
    GPIOA->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0);
    // Set to push-pull for PA0-PA2
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2);
    // No pull-up, no pull-down for PA0-PA2
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2);
    // Set high speed
    GPIOA->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos) | (3 << GPIO_OSPEEDR_OSPEED1_Pos) | (3 << GPIO_OSPEEDR_OSPEED2_Pos));
}

