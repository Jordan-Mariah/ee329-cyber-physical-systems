/*
 * delay.h
 *
 *  Created on: Oct 11, 2024
 *      Author: asakd
 */

#include "stm32l4xx_hal.h"

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

// function prototypes
void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
