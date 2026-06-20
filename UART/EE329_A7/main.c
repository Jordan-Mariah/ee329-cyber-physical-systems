/*
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 * @file           : main.c
 * @brief          : A7- UART Communications
 * project         : EE 329 FALL 24 Assignment 7
 * function        : This program utilizes UART Communication using serial to take
 * 					 in an input from the keyboard and perform certain functions on
 * 					 a downloaded Serial emulator, Putty. The program plays a starting
 * 					 screen then lets the user move the character using WASD for directions
 * 					 and has the character teleport to the opposite border once it hits
 * 					 any of the border lines.
 * authors         : Jordan Reichhardt
 * 				   : Alec Lumba
 * version         : 0.1
 * date            : 10/28/24
 * target          : NUCLEO-L4A6ZG
 * @attention      : (c) 2023 STMicroelectronics.  All rights reserved.
 ******************************************************************************/
#include "stm32l4xx.h"  // Include the appropriate header file for the STM32L4 series
#include "delay.h"
#include "LPUART.h"

// initialize for vertical and horizontal borders
int vert_coord = 11;
int horz_coord = 37;


int main(void) {
	// All the initializations
	HAL_Init();
	SystemClock_Config();
	LPUART1_init();

	// Invisible cursor
	LPUART_Print("\033[?25l");

	// Initial screen message
	Entry_Screen();
	delay_us(5000000);

	// Fireworks screen
	Splash_Fire();
	// Title Screen
	Print_Title();
	delay_us(5000000);

	// Clears screen
	LPUART_Print("\033[2J");

	// Creates game border and character
	Set_Borders();
	while (1) {
		;
	}
}

/***************Interrupt Handler**************
 * Used to take in keyboard inputs and perform*
 * specific tasks based off that input        *
 **********************************************
 * Authors: Jordan R., Alec L.                *
 **********************************************/
void LPUART1_IRQHandler(void) {
	uint8_t charRecv;
	// If received an interrupt for LPUART1
	if (LPUART1->ISR & USART_ISR_RXNE) {
		charRecv = LPUART1->RDR;
		switch (charRecv) {
		case 'w':
			Delete_Guy();	// deletes character before moving
			// for every case where we are not touching the upper border
			if(vert_coord>1){
				LPUART_Print("\033[1A[._.]/");		// move character up
				vert_coord--;
			}
			// move to the opposite side once hitting upper border
			else {
				LPUART_Print("\033[20B[._.]/");
				vert_coord=21;
			}
			break;

		case 'a':
			Delete_Guy();// deletes character before moving
			// for every case where we are not touching the left border
			if(horz_coord > 1){
				LPUART_Print("\033[1D[._.]/");// move character left
				horz_coord--;
			}
			// move to the opposite side once hitting the left border
			else{
				LPUART_Print("\033[71C[._.]/");
				horz_coord=72;
			}
			break;

		case 's':
			Delete_Guy();// deletes character before moving
			// for every case where we are not touching the bottom border
			if(vert_coord<=20){
				LPUART_Print("\033[1B[._.]/");// move character down
				vert_coord++;
			}
			// move to the opposite side once hitting the bottom border
			else {
				LPUART_Print("\033[20A[._.]/");
				vert_coord=1;
			}
			break;

		case 'd':
			Delete_Guy();// deletes character before moving
			// for every case we are not touching the right border
			if(horz_coord<72){
				LPUART_Print("\033[1C[._.]/");//move the character right
				horz_coord++;
			}
			// move to the opposite side once hitting the right border
			else{
				LPUART_Print("\033[71D[._.]/");
				horz_coord=1;
			}
			break;
		case 'W':
			// Send escape sequence for white color (default)
			LPUART_Print("\033[37m"); // ANSI escape code to turn text white
			break;
		case 'R':
			// Send escape sequence for red color
			LPUART_Print("\033[31m"); // ANSI escape code to turn text red
			break;
		case 'B':
			// Send escape sequence for blue color
			LPUART_Print("\033[34m"); // ANSI escape code to turn text blue
			break;
		case 'G':
			// Send escape sequence for green color
			LPUART_Print("\033[32m"); // ANSI escape code to turn text green
			break;
			//if a character different than these four our pressed
		default:
			// Echo the received character back to the terminal
			while (!(LPUART1->ISR & USART_ISR_TXE)) {
				// Wait until the transmit data register is empty
			}
			//add char to transmit data register
			LPUART1->TDR = charRecv;
			break;
		}  // end switch
	}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}
void Error_Handler(void)
{
	/* User can add their own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
}
