/*******************************************************************************
 * EE 329 A3 LCD TIMER
 *******************************************************************************
 * @file           : delay.c
 * @brief          : Keypad and LCD interfaced to create a countdown timer
 * project         : EE 329 F'24 Assignment 3
 * authors         : Mateo Charles - vcharles@calpoly.edu
 * citations		 : John Penvenne - jpenvenn@calpoly.edu
 *							-function that enables systick clock for software delay
 *							 and allows us to call a function that creates a
 *							 delay for a specified amount of microseconds
 * date            : 10/14/24
 * compiler        : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 *******************************************************************************/

#include "delay.h"


/* -----------------------------------------------------------------------------
 * function : SysTick_Init ()
 * INs      : none
 * OUTs     : none
 * action   : Systick timer configured in order to call delay_us()
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void SysTick_Init( void ) {

	SysTick->CTRL |= ( SysTick_CTRL_ENABLE_Msk |     // enable SysTick Timer
                    	 SysTick_CTRL_CLKSOURCE_Msk ); // select CPU clock
	SysTick->CTRL &= ~( SysTick_CTRL_TICKINT_Msk );   // disable interrupt
	}

/* -----------------------------------------------------------------------------
 * function : delay_us ( )
 * INs      : unsigned 32 bit specified time for microseconds
 * OUTs     : none
 * action   : creates a counter using Systick that results in a
 * 			  specified delay in microseconds
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void delay_us( const uint32_t time_us ) {
// set the counts for the specified delay
	SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
	SysTick->VAL = 0;                                   // clear timer count
	SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);     // clear count flag
	while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)); // wait for flag
}
