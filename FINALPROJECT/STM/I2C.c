/* -----------------------------------------------------------------------------
 * i2c.c
 * -----------------------------------------------------------------------------
 * @file i2c.c
 * @brief Header file for I2C module configuration and data transmission.
 *
 * This module provides functions to configure the I2C peripheral for communication
 * with an external device (e.g., ESP32) and perform data transmission. It includes
 * initialization of GPIO pins for I2C functionality and high-level functions for
 * sending data over the I2C bus.
 *
 * ### Hardware Connections:
 * - **I2C SDA:** GPIO pin PB9
 * - **I2C SCL:** GPIO pin PB8
 * - **I2C Slave Address:** 0x51
 *
 * ### Functionality:
 * - Configures GPIO pins for I2C alternate function.
 * - Initializes the I2C peripheral with specified timing.
 * - Provides functionality to send single-byte data to the slave device.
 * -----------------------------------------------------------------------------
 */
#include "i2c.h"
#include "main.h"

/* -----------------------------------------------------------------------------
 * Function : GPIO_I2C1_Init
 * Action   : Configures PB8 and PB9 for I2C SCL and SDA operation
 ----------------------------------------------------------------------------- */
void GPIO_I2C1_Init(void) {
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; // Enable GPIOB clock

    // Configure PB8 (SCL) and PB9 (SDA) as alternate function (AF4)
    GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
    GPIOB->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1); // AF mode
    GPIOB->OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);       // Open-drain
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9); // Medium speed
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);     // No pull-up/pull-down
    GPIOB->AFR[1] |= (4 << GPIO_AFRH_AFSEL8_Pos) | (4 << GPIO_AFRH_AFSEL9_Pos); // AF4
}

/* -----------------------------------------------------------------------------
 * Function : I2C1_Init
 * Action   : Initializes the I2C1 peripheral with 400 kHz settings
 ----------------------------------------------------------------------------- */
void I2C1_Init(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN; // Enable I2C1 clock
    I2C1->CR1 &= ~I2C_CR1_PE;             // Disable I2C peripheral
    I2C1->CR1 &= ~I2C_CR1_ANFOFF;         // Enable analog filter
    I2C1->CR1 &= ~I2C_CR1_DNF;            // Disable digital filter
    I2C1->TIMINGR = 0x00303D5B;           // Timing for 16 MHz SYSCLK
    I2C1->CR2 &= ~I2C_CR2_ADD10;          // 7-bit addressing
    I2C1->CR1 |= I2C_CR1_PE;              // Enable I2C peripheral
}

/* -----------------------------------------------------------------------------
 * Function : I2C_SendNumber
 * Action   : Sends a single byte (number) to the ESP32 over I2C
 ----------------------------------------------------------------------------- */
void I2C_SendNumber(uint8_t number) {
    uint32_t timeout = I2C_TIMEOUT_MS;

    // Wait until the I2C bus is free or timeout
    while ((I2C1->ISR & I2C_ISR_BUSY) && timeout > 0) {
        Delay_ms(1);
        timeout--;
    }

    if (timeout == 0) {
        return;  // Exit if the bus is busy
    }

    // Configure the I2C transaction
    I2C1->CR2 = (I2C_SLAVE_ADDRESS << 1)   // Set slave address (7-bit) for write
                | (1 << I2C_CR2_NBYTES_Pos) // 1 byte to send
                | I2C_CR2_AUTOEND;          // Generate STOP after transmission
    I2C1->CR2 &= ~I2C_CR2_RD_WRN;           // Write mode
    I2C1->CR2 |= I2C_CR2_START;             // Generate START condition

    // Wait for TXIS flag
    timeout = I2C_TIMEOUT_MS;
    while (!(I2C1->ISR & I2C_ISR_TXIS) && timeout > 0) {
        Delay_ms(1);
        timeout--;
    }

    if (timeout == 0) {
        return;  // Exit if TXIS flag is not set
    }

    // Send data byte
    I2C1->TXDR = number;

    // Wait for TC flag
    timeout = I2C_TIMEOUT_MS;
    while (!(I2C1->ISR & I2C_ISR_TC) && timeout > 0) {
        Delay_ms(1);
        timeout--;
    }

    if (timeout == 0) {
        return;  // Exit if TC flag is not set
    }

    // Clear the STOP condition flag
    I2C1->ICR = I2C_ICR_STOPCF;
}
