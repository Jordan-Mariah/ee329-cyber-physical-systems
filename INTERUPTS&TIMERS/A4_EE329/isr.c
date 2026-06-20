#include "isr.h"
#include "stm32l4xx.h"
#include "main.h"  // For BUTTON_PIN definition

// TIM2 interrupt handler function
void TIM2_IRQHandler(void) {
    // Check for random delay completion (period event)
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~(TIM_SR_UIF);  // Clear interrupt flag

        // Handle the completion of the random delay
        if (!random_delay_done && reaction_triggered) {
            random_delay_done = 1;  // Random delay is complete
            // Don't reset the timer; let it continue running to capture the reaction time
        }
    }
}
