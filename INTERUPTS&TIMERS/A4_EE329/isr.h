#ifndef ISR_H
#define ISR_H
#include "stm32l4xx.h"  // Adjust based on your STM32 series
#include "main.h"       // Include main.h to access shared variables
#include <stdint.h>
extern volatile uint8_t reaction_triggered;
extern volatile uint32_t start_time;
extern volatile uint32_t end_time;
extern volatile uint8_t waiting_for_response;
extern volatile uint8_t random_delay_done;
extern volatile uint8_t button_pressed;
extern volatile float reaction_time;
// Function prototypes for the interrupt handler
void TIM2_IRQHandler(void);
#endif // ISR_H
