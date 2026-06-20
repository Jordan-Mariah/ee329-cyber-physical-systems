/* USER CODE BEGIN Header */
/*******************************************************************************
 * ADC AND COIL CONTROL WITH UART OUTPUT
 *******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file
 * project         : EE 329 Current Measurement Project
 * authors         : Kalon Bienz, Jordan Reichhardt
 * version         : 1.2
 * date            : 171124
 * compiler        : STM32CubeIDE
 * target          : NUCLEO-L4A6ZG
 *******************************************************************************
 * @attention
 * This header defines constants, globals, and function prototypes for the
 * ADC-based current measurement system with UART output.
 ******************************************************************************/
/* USER CODE END Header */

#ifndef ADC_H
#define ADC_H

/* Standard library includes */
#include <stdint.h>
#include <string.h>

/* STM32 HAL includes */
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_rcc.h"
#include "stm32l4xx_hal_gpio.h"
#include "stm32l4xx_hal_conf.h"

/* Constants */
#define SAMPLE_SIZE        20        // Number of ADC samples to collect
#define ADC_MAX_VALUE     4095       // 12-bit ADC maximum value
#define V_REF            3.3f        // Reference voltage in volts
#define BAUD_RATE        4444        // UART baud rate (2MHz Clock)
#define R_SENSE          48.0f       // Current sense resistor value in ohms

/* Function Prototypes */
/* Initialization functions */
void ADC_Init(void);                 // Initialize ADC with interrupts
void UART_Init(void);                // Initialize LPUART communication
void SysTick_Init(void);             // Initialize SysTick timer
void Setup_Button(void);             // Configure GPIO for button input
void Setup_Coil(void);               // Configure GPIO for coil control
void SystemClock_Config(void);       // Configure system clock to 2 MHz

/* Processing and conversion functions */
void Process_Samples(void);          // Calculate min, max, avg from samples
void Convert_ADC_To_Voltage(uint16_t adc_val, char* buffer);    // ADC to voltage
void Convert_To_Current_String(float current, char* buffer);     // Float to string

/* Communication and display functions */
void LPUART_Print(const char* message);   // Send string via UART
void Display_Results(void);               // Format and show measurements

/* Utility functions */
void Delay_us(uint32_t us);              // Microsecond delay
void Error_Handler(void);                // System error handler

/* Interrupt handlers */
void ADC1_2_IRQHandler(void);           // ADC conversion complete handler

#endif /* ADC_H */
