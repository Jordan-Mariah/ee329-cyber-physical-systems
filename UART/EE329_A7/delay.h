/*
 * delay.h
 *
 *  Created on: Oct 10, 2024
 *      Author: Given by Professor Penvenne
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "main.h"	//Provides uint32_t

void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
