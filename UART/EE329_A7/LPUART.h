/*
 * LPUART1.h
 *
 *  Created on: Oct 24, 2024
 *      Author: alecl
 */

#ifndef INC_LPUART_H_
#define INC_LPUART_H_

#include "main.h"

// Definitions for LPUART1
#define LPUART1_TX_PIN        7           // PG7
#define LPUART1_RX_PIN        8           // PG8
#define LPUART1_AF            8           // Alternate Function 8 for LPUART1
#define GPIOGEN               RCC_AHB2ENR_GPIOGEN
#define LPUART1EN             RCC_APB1ENR2_LPUART1EN


void LPUART1_init(void);
void LPUART_Print(const char* message);
void LPUART1_IRQHandler(void);
void SystemClock_Config(void);
void Set_Borders(void);
void Entry_Screen(void);
void Splash_Fire(void);
void Print_Title(void);
void Delete_Guy(void);
void LPUART_ESC_Print(void);
#endif /* INC_LPUART_H_ */
