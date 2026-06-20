/*
 * DAC.h
 *
 *  Created on: Nov 2, 2024
 *      Author: asakd
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

#include "stm32l4xx_hal.h"

// function prototypes
void DAC_init(void);
void SPI_init(void);
void DAC_write(uint16_t data);
uint16_t DAC_volt_conv (uint16_t target_voltage);

#endif /* INC_DAC_H_ */
