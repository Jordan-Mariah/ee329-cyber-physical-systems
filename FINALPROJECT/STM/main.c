/* ------------------------------------------------------------------------------
 * main.c
 * ------------------------------------------------------------------------------
 * @file main.c
 * @brief Main application for interfacing a keypad and transmitting key presses
 *  via I2C.
 *
 * This module is the entry point for the application. It initializes hardware
 * peripherals, handles the main execution loop, and coordinates the keypad and
 * I2C communication functionality. The application detects key presses from a
 * 4x3 keypad and sends the corresponding key value to an ESP32 via I2C.
 *
 * ### Hardware Connections:
 * - **Keypad Columns (Outputs):** GPIO pins PD0, PD1, PD2
 * - **Keypad Rows (Inputs):** GPIO pins PD3, PD4, PD5, PD6
 * - **I2C SDA:** GPIO pin PB9
 * - **I2C SCL:** GPIO pin PB8
 * - **I2C Slave Address:** 0x51
 * - **Status LED:** GPIO pin PB7
 *
 * ### Functionality:
 * - Detect and debounce key presses from the keypad.
 * - Encode key presses into numerical values or control characters.
 * - Transmit the detected key value to an I2C slave device.
 * - Provide system clock configuration and delay functionality.
 * -----------------------------------------------------------------------------
 */

#include "main.h"
#include "i2c.h"
#include "keypad.h"
/* Main ---------------------------------------------------------------------- */
int main(void) {
    // Initialize HAL, clock, and GPIO
    HAL_Init();
    SystemClock_Config();
    GPIO_I2C1_Init();
    I2C1_Init();
    Keypad_Config();

    // Configure LED on PB7
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    GPIOB->MODER &= ~(GPIO_MODER_MODE7); // Clear mode bits
    GPIOB->MODER |= (1 << GPIO_MODER_MODE7_Pos); // Set as output

    uint8_t number_to_send = 6;

    while (1) {
        number_to_send = Grab_KeyPress();  // Detect key press
        I2C_SendNumber(number_to_send);   // Send the pressed key
        Delay_ms(10);  // Delay for stability
    }
}

/* -----------------------------------------------------------------------------
 * Function : Delay_ms
 * Action   : Simple millisecond delay using SysTick
 ----------------------------------------------------------------------------- */
void Delay_ms(uint32_t ms) {
    SysTick->LOAD = 16000 - 1; // 1 ms delay at 16 MHz
    SysTick->VAL = 0;          // Clear current value
    SysTick->CTRL = 5;         // Enable SysTick with processor clock

    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & 0x10000)); // Wait for COUNTFLAG
    }

    SysTick->CTRL = 0; // Disable SysTick
}

/* -----------------------------------------------------------------------------
 * Function : SystemClock_Config
 * Action   : Configures the system clock
 ----------------------------------------------------------------------------- */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }

    // Initialize the RCC Oscillators according to the specified parameters
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_8;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Initialize the CPU, AHB, and APB buses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/* -----------------------------------------------------------------------------
 * Function : Error_Handler
 * Action   : Handles errors by entering an infinite loop
 ----------------------------------------------------------------------------- */
void Error_Handler(void) {
    __disable_irq();
    while (1) {
    }
}
