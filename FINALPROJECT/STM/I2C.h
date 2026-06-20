#ifndef I2C_H
#define I2C_H

#include "stm32l4xx.h"

// I2C Address
#define I2C_SLAVE_ADDRESS 0x51
#define I2C_TIMEOUT_MS 10  // Timeout for I2C operations

// Function Prototypes
void GPIO_I2C1_Init(void);
void I2C1_Init(void);
void I2C_SendNumber(uint8_t number);

#endif /* I2C_H */
