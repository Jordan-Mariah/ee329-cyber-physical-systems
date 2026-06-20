/*                     Main LPUART source file					 *
 *                   Authors: Jordan R., Alec L.				 *
 *****************************************************************
 * LPUART1_init() : Initializes our LPUART1 to specific settings *
 * LPUART_Print() : Prints out a set of characters to our screen *
 * Entry_Screen() : The first screen of our start up page		 *
 * Splash_Fire()  : Fireworks section for our start up page		 *
 * Print_Fireworks: used in Splash_Fire() to print fireworks	 *
 * Set_Borders()  : Creates our game borders					 *
 * Delete_Guy()   : Deletes "GUY", used for our moving functions *
 * LPUART_ESC_Print(): Program for part 4 of Lab manual          *
 * ****************************************************************/

#include "delay.h"
#include "LPUART.h"

void LPUART1_init(void) {
	// Step 1: Enable the required clocks for GPIO and LPUART1
	PWR->CR2 |= (PWR_CR2_IOSV);                    // Power available on PG[15:2] (LPUART1)
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;                       // Enable GPIOG clock
	RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;                   // Enable LPUART1 clock bridge
	// Select AF mode and specify which function (AF8 for LPUART1 on PG7 and PG8)
	GPIOG->AFR[0] &= ~((0x000F << GPIO_AFRL_AFSEL7_Pos)); // clear PG7 nibble AF
	GPIOG->AFR[0] |=  ((0x0008 << GPIO_AFRL_AFSEL7_Pos)); // set PG7 AF = LPUART1_TX
	GPIOG->AFR[1] &= ~((0x000F << GPIO_AFRL_AFSEL0_Pos)); // clear PG8 nibble AF
	GPIOG->AFR[1] |=  ((0x0008 << GPIO_AFRL_AFSEL0_Pos)); // set PG8 AF = LPUART1_RX
	// Step 2: Configure GPIO pins for LPUART1 TX and RX as alternate function
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
	GPIOG->OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8);  // Set to push-pull for PE0-PE3
	GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);  // No pull-up, no pull-down
	GPIOG->MODER &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8); // Clear mode bits for PG7 and PG8
	GPIOG->MODER |= (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1); // Set alternate function mode (10)
	GPIOG->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED7_Pos)|(3 << GPIO_OSPEEDR_OSPEED8_Pos));

	// Step 3: Configure LPUART1 parameters: baud rate, word length, stop bits, parity, etc.
	LPUART1->CR1 &= ~(USART_CR1_UE);// Disable LPUART1 while configuring

	// Set the baud rate register (LPUART1->BRR) assuming 2 MHz clock, 115200 baud rate
	LPUART1->BRR = 0x115c; // Example BRR value for 115200 baud at 2 MHz clock

	// Data Bits Configuration (8 bits)
	LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);// Clear M1 and M0 (00) to configure as 8 data bits

	// Stop Bits Configuration (1 stop bit)
	LPUART1->CR2 &= ~(USART_CR2_STOP);// Clear STOP[1:0] bits (00) bit 12&13 for 1 stop bit

	// Parity Configuration (None)
	LPUART1->CR1 &= ~(USART_CR1_PCE);// Clear PCE for no parity (bit check error detection)

	// Enable the TX and RX functionality
	LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);// Enable transmit & receive (bits 3 and 2)

	// Enable LPUART1 (we previously disabled for configuration)
	LPUART1->CR1 |= USART_CR1_UE;

	// Enable LPUART1 receive interrupt
	LPUART1->CR1 |= USART_CR1_RXNEIE;

	// Clear Recv-Not-Empty flag (flag for receive interrupts)
	LPUART1->ISR &= ~(USART_ISR_RXNE);

	// Enable LPUART1 ISR in NVIC
	NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));

	// Enable global interrupts
	__enable_irq();

	LPUART_Print("\033[2J");

}
void LPUART_Print(const char* message) {
	uint16_t iStrIdx = 0;
	while (message[iStrIdx] != 0) {
		while(!(LPUART1->ISR & USART_ISR_TXE)) // wait for empty transmit buffer
			;
		LPUART1->TDR = message[iStrIdx];       // send this character
		iStrIdx++;                             // advance index to next char
	}
}


void Entry_Screen(void){
	LPUART_Print("\033[2J");
	LPUART_Print("\033[2J\033[H"); // Clear screen and move to top-left
	LPUART_Print("\033[37m");//turn white using ansi code
	LPUART_Print("------------------------------------------------------\n\n");
	LPUART_Print("\033[1E\033[21D");// Move cursor down 1 line and left 21 spaces
	LPUART_Print("\033[31m");//turn red using ansi code
	LPUART_Print("------------------------------------------------------\n\n");
	LPUART_Print("\033[32m"); //turn green using ansi code
	LPUART_Print("\033[1E\033[21D");// Move cursor down 1 line and left 21 spaces
	LPUART_Print("Enjoy the firework show\n");
	LPUART_Print("\033[1E\033[21D");// Move cursor down 1 line and left 21 spaces
	LPUART_Print("\033[34m"); //turn blue using ansi code
	LPUART_Print("------------------------------------------------------\n\n");
	LPUART_Print("\033[1E\033[21D");// Move cursor down 1 line and left 21 spaces
	LPUART_Print("\033[37m");//turn white using ansi code
	LPUART_Print("------------------------------------------------------\n\n");
}

void Print_Title(void){
	LPUART_Print("\033[H");			// resets cursor to top left

	// Creates top border
	for(int count = 0; count < 79; count++){
		LPUART_Print("-");
	}

	// Creates the side borders
	for(int count = 0; count < 21; count++){
		LPUART_Print("\033[1E|\033[77C|");
	}
	LPUART_Print("\033[1E|");

	// Creates the bottom border
	for(int count = 0; count < 78; count++){
		LPUART_Print("-");
	}
	LPUART_Print("\033[H\033[38C\033[10B EE329-A7!\033[H\033[38C\033[11B THE GAME!");

}

void Splash_Fire(void) {
	// Display catchy game name
	LPUART_Print("\033[2J\033[H"); // Clear screen and move to top-left
	//	LPUART_Print("Enjoy the firework show'\n\n");
	// Loop through the fireworks 5 times can change color inbetweenmanually
	for (int i = 0; i < 3; i++) {
		Print_Fireworks();
		delay_us(500000);  // Half-second delay for effec
		// Clear screen to make each "explode"
		LPUART_Print("\033[2J\033[H");
		delay_us(500000);  // Half-second delay for effec
	}
	// Wait for .5 seconds (total splash screen duration)
	delay_us(500000);
}

void Print_Fireworks(void){
	LPUART_Print("\r        *        *              ***     **  *     *\n");
	LPUART_Print("\033[31m"); // red
	LPUART_Print("\r     *     *  *    *       ***        **    ** * *     *    \n");
	LPUART_Print("\r   *         *     *     *  *       * \n");
	LPUART_Print("\033[34m"); //blue
	LPUART_Print("\r *  *      POW!!  *       *  **\033[31m**     ****     **     * *     *\n");
	LPUART_Print("\r*       *  *  * \033[31m       * *  *        *       *       **     *\n");
	LPUART_Print("\033[37m"); //white
	LPUART_Print("\r *   *       *     \033[31m  *  *       *  *  *     BOOOOM!   * *  * \n");
	LPUART_Print("\r   *         *        *       *  *  * *     *       * *  *   \n");
	LPUART_Print("\033[34m"); //blue
	LPUART_Print("\r     *    * *     *  * *     *   * **     *     *      \n");
	LPUART_Print("\r       POW!! * * *     *  BOOM! * *     *        *         \n");
	LPUART_Print("\r            *    *     * *     *   *     **     * \n");
	LPUART_Print("\033[31m"); // red
	LPUART_Print("\r       **    *  *\033[31m**    *  * **    *  *    *     \n");
	LPUART_Print("\r    ***        *    ***        *           ***        **     * \n");
	LPUART_Print("\r       **     *       *  *    **     *       *  *\n");
	LPUART_Print("\033[37m"); //white
	LPUART_Print("\r        *    **\033[31m**   ***    *  ***    *  * **    *  **     *\n");
	LPUART_Print("\r           *     * *        *    ***        *          *     *\n");
	LPUART_Print("\033[31m"); // red
	LPUART_Print("\r          * BAM!!    *        *    ***     \033[31m   *        *     *      *   \n");
	LPUART_Print("\r  *        *    ***        *                 * *BAMM!!     *      \n");
	LPUART_Print("\r        * *   BAMM!!     *    ***        *                *         \n");
	LPUART_Print("\r        *    ****   ***    *  ***    *  * **    *  **     *     **\n");
	LPUART_Print("\r           *     * *        *    ***ZAMM!        *          \n");
	LPUART_Print("\033[34m"); //blue
	LPUART_Print("\r  *        *    ***        *                 * *BAMM!!     *      \n");
	LPUART_Print("\r        * *   BAMM!!     *    ***        *                *     **         \n");
	LPUART_Print("\r        *    ****   ***    *  ***    *  * **    **     *  *\n");
	LPUART_Print("\033[37m"); //white
	LPUART_Print("\r *   *       *       *  *       *  *  *     BOOOOM!   * * *     * * \n");
	LPUART_Print("\r *  *      POW!!  *       *  ****     ****     **     *");
}

void Set_Borders(void){
	LPUART_Print("\033[H");			// resets cursor to top left

	// Creates top border
	for(int count = 0; count < 79; count++){
		LPUART_Print("-");
	}

	// Creates the side borders
	for(int count = 0; count < 21; count++){
		LPUART_Print("\033[1E|\033[77C|");
	}
	LPUART_Print("\033[1E|");

	// Creates the bottom border
	for(int count = 0; count < 78; count++){
		LPUART_Print("-");
	}

	// prints guy in the center of the screen
	LPUART_Print("\033[H\033[37C\033[11B[._.]/");
}

void Delete_Guy(void){
	LPUART_Print("\033[6D");
	LPUART_Print("      ");
	LPUART_Print("\033[6D");
}

void LPUART_ESC_Print(void) {
	// Move cursor down 3 lines and right 5 spaces
	LPUART_Print("\033[3E\[5C");
	// Print "All good students read the"
	LPUART_Print("All good students read the");
	// Move cursor down 1 line and left 21 spaces
	LPUART_Print("\033[1E\033[21D");
	// Enable blinking mode
	LPUART_Print("\033[5m");
	// Print "Reference Manual"
	LPUART_Print("Reference Manual");
	// Move the cursor back to the top left
	LPUART_Print("\033[H");
	// Remove character attributes (disable blinking text)
	LPUART_Print("\033[0m");
	// Print "Input: "
	LPUART_Print("Input: ");
}
