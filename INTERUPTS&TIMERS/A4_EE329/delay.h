#ifndef INC_DELAY_H_
#define INC_DELAY_H_
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
void SysTick_Init(void);
void delay_us(const uint32_t time_us);
#endif /* INC_DELAY_H_ */
