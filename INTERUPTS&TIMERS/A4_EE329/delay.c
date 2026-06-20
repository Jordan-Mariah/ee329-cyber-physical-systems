#include "main.h"
#include "delay.h"
void SysTick_Init(void) {
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |     	// enable SysTick Timer
                    SysTick_CTRL_CLKSOURCE_Msk); 	// select CPU clock
	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);  	// disable interrupt
}
void delay_us(const uint32_t time_us) {
   // Ensure SysTick is properly configured
   SysTick->CTRL = 0;  // Disable SysTick during configuration
   SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);  // Set delay
   SysTick->VAL = 0;  // Clear current value register
   SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;  // Enable SysTick
   // Wait for the COUNTFLAG to be set
   while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}
