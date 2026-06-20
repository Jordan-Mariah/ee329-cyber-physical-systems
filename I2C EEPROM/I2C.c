/* USER CODE BEGIN Header */
/*******************************************************************************
 * EE329 Fall 2024
 *******************************************************************************
 * @file			: I2C.c
 * @brief		: I2C program body
 * Project		: Lab A9: I2C EEPROM
 * Author(s)	: Thomas McMillan (tem) - temcmill@calpoly.edu
 * 				: Jordan Reichhardt (jmr) - jreichha@calpoly.edu
 * Version		: 0.1
 * Date			: 11/06/2024
 * Compiler		: STM32CubeIDE Version: 1.16.0 Build: 21983_20240628_1741 (UTC)
 * Target		: NUCLEO-L4A6ZG
 * Citations	: The structure of the code, and most of the commands, are
 * 				: taken straight from the lab manual for I2C setup and for
 * 				: EEPROM operations.
 * 				: Juan C. Moreno Ayala also helped to debug the I2C.c file. We
 * 				: worked separately and came up with very similar code sets, yet
 * 				: the code set I wrote would not read from the EEPROM (still not
 * 				: sure why).  I borrowed Juans I2C source file.
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
 *******************************************************************************
* A9 Project Plan:
* Same plan outlined in main.c
********************************************************************************
* I2C-EEPROM GPIO Plan: Port B
* Pin		Header	Function 	EEPROM Pin#		EEPROM Pin Desc.
* PB8		CN7-2		SCL			6					SCL
* PB9		CN7-4		SDA			5					SDA
********************************************************************************
* REVISION HISTORY
* 0.1 	241106 tem create
*
*******************************************************************************/
// 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-2345678-8
/* USER CODE END Header */

#include "I2C.h"

/* -----------------------------------------------------------------------------
* function : EEPROM_Init ( )
* INs : none
* OUTs : none
* action : Initialize EEPROM GPIO pins (port B pins 8 [SCL] and 9 [SDA]) and
* 			 setup I2C function
* authors : Thomas McMillan (tem) - temcmill@calpoly.edu
* version : 0.1
* date : 11/06/2024
* citations:
* ----------------------------------------------------------------------------*/
void EEPROM_Init(void) {
	// enable clock for GPIOB
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN); // GPIOB: I2C SCL and SDA

	// alternate function setup
	GPIOB->AFR[1] &= ~((0x000F << GPIO_AFRH_AFSEL8_Pos)); // SCL
	GPIOB->AFR[1] |= ((0x0004 << GPIO_AFRH_AFSEL8_Pos));
	GPIOB->AFR[1] &= ~((0x000F << GPIO_AFRH_AFSEL9_Pos)); // SDA
	GPIOB->AFR[1] |= ((0x0004 << GPIO_AFRH_AFSEL9_Pos));

	// open drain output
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);
	GPIOB->OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);

	// pull up resistor
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0);

	// very high speed
	GPIOB->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED8_Pos) |
			(3 << GPIO_OSPEEDR_OSPEED9_Pos));

	// set alternate function as the mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
	GPIOB->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);

	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN; // enable I2C bus clock
	I2C1->CR1 &= ~( I2C_CR1_PE ); // put I2C into reset (release SDA, SCL)
	I2C1->CR1 &= ~( I2C_CR1_ANFOFF ); // filters: enable analog
	I2C1->CR1 &= ~( I2C_CR1_DNF ); // filters: disable digital
	I2C1->TIMINGR = 0x00503D58; // 16 MHz SYSCLK timing from CubeMX
	I2C1->CR2 |= ( I2C_CR2_AUTOEND ); // auto send STOP after transmission
	I2C1->CR2 &= ~( I2C_CR2_ADD10 ); // 7-bit address mode
	I2C1->CR1 |= ( I2C_CR1_PE ); // enable I2C
}

// 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-2345678-8
/* -----------------------------------------------------------------------------
* function : EEPROM_Write ( )
* INs : 1byte data to write, and 15bit address in EEPROM to write to
* OUTs : none
* action : take address input, send upper half to EEPROM and then lower half,
*			and then send data byte to EEPROM
* authors : Thomas McMillan (tem) - temcmill@calpoly.edu
* version : 0.1
* date : 11/06/2024
* citations:
* ----------------------------------------------------------------------------*/
void EEPROM_Write ( uint8_t data , uint16_t addr ) {
	//Build EEPROM transaction
	I2C1->CR2 &= ~(I2C_CR2_RD_WRN);         //Set WRITE mode
	I2C1->CR2 &= ~(I2C_CR2_NBYTES);         //Clear Byte count
	I2C1->CR2 |= (3 << I2C_CR2_NBYTES_Pos); //Write 3 bytes (2 addr, 1 data)
	I2C1->CR2 &= ~(I2C_CR2_SADD);           //Clear device address
	I2C1->CR2 |= (EEPROM_ADDRESS << (I2C_CR2_SADD_Pos+1)); //Device addr SHL 1
	I2C1->CR2 |= I2C_CR2_START;             //Start I2C WRITE op

	/* wait for I2C_ISR_TXIS to clear before writing each Byte */
	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;   // wait for TX reg
	I2C1->TXDR = (addr >> 8);              // xmit MSByte of address
	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;
	I2C1->TXDR = ((addr)&(0xFF));          // xmit LSByte of address
	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;
	I2C1->TXDR = data;                     // xmit Byte of data

}

// 45678-1-2345678-2-2345678-3-2345678-4-2345678-5-2345678-6-2345678-7-2345678-8
/* -----------------------------------------------------------------------------
* function : EEPROM_Read ( )
* INs : EEPROM address
* OUTs : the read data (from EEPROM)
* action : write address to EEPROM first, and then carry out read operation to
* 			return data byte from EEPROM
* authors : Thomas McMillan (tem) - temcmill@calpoly.edu
* version : 0.1
* date : 11/06/2024
* citations:
* ----------------------------------------------------------------------------*/
uint8_t EEPROM_Read ( uint16_t addr ) {
	//Build EEPROM transaction
	I2C1->CR2 &= ~(I2C_CR2_RD_WRN);         //Set WRITE mode
	I2C1->CR2 &= ~(I2C_CR2_NBYTES);         //Clear Byte count
	I2C1->CR2 |= (2 << I2C_CR2_NBYTES_Pos); //Write 2 bytes (2 addr)
	I2C1->CR2 &= ~(I2C_CR2_SADD);           //Clear device address
	I2C1->CR2 |= (EEPROM_ADDRESS <<
			(I2C_CR2_SADD_Pos+1)); //Device addr SHL 1
	I2C1->CR2 |= I2C_CR2_START;             //Start I2C WRITE op

	/* wait for I2C_ISR_TXIS to clear before writing each Byte, e.g. ... */
	while(!(I2C1->ISR & I2C_ISR_TXIS)) ; // wait for TX reg
			I2C1->TXDR = (addr >> 8);      // xmit MSByte of address
	while(!(I2C1->ISR & I2C_ISR_TXIS)) ;
	I2C1->TXDR = ((addr)&(0xFF));       // xmit LSByte of address

	uint8_t data = 0;

	I2C1->CR2 &= ~(I2C_CR2_RD_WRN);         //Set READ mode
	I2C1->CR2 |= (I2C_CR2_RD_WRN);
	I2C1->CR2 &= ~(I2C_CR2_NBYTES);         //Clear Byte count
	I2C1->CR2 |= (1 << I2C_CR2_NBYTES_Pos); //Read 1 byte
	I2C1->CR2 &= ~(I2C_CR2_SADD);           //Clear device address
	I2C1->CR2 |= (EEPROM_ADDRESS <<
			(I2C_CR2_SADD_Pos+1)); //Device addr SHL 1
	I2C1->CR2 |= I2C_CR2_START;             //Start I2C READ op
	while(!(I2C1->ISR & I2C_ISR_RXNE)) ;   //Wait for receive reg
	data = I2C1->RXDR;                     //Read byte and return value
	return data;
}
