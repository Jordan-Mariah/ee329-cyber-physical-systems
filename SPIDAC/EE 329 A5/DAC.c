/*
 * DAC.c
 *
 *  Created on: Nov 2, 2024
 *      Author: asakd
 */
#include "DAC.h"
/*
 * DAC WIRING
 * VDD  - 3.3V
 * CS   - PA4
 * SCK  - PA5
 * SDI  - PA7
 * LDAC - GND
 * VREF - 3.3V
 * VSS  - GND
 * VOUT - OUT
 */

void DAC_init(void) {
	// enable clock for GPIOA & SPI1
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);             // GPIOA: DAC NSS/SCK/SDO
	RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);                 // SPI1 port
	/* USER ADD GPIO configuration of MODER/PUPDR/OTYPER/OSPEEDR registers HERE */
	// configure AFR for SPI1 function (1 of 3 SPI bits shown here)
	// PA4 is CS, PA5 is SCK, DAC SDI is MOSI which is PA7
	GPIOA->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE7);
	GPIOA->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 // clear
			| GPIO_MODER_MODE7_1); // and with 10, AF Mode
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD7); // no PU/PD,
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT7); // push-pull
	GPIOA->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED4_Pos) // high speed
	| (3 << GPIO_OSPEEDR_OSPEED5_Pos) | (3 << GPIO_OSPEEDR_OSPEED7_Pos));

	// AFR [0] is upper AF Registers, or AFRL
	GPIOA->AFR[0] &= ~((0x000F << GPIO_AFRL_AFSEL7_Pos)); // clear nibble for bit 7 AF
														  // MOSI, PA7
	GPIOA->AFR[0] |= ((0x0005 << GPIO_AFRL_AFSEL7_Pos)); // set b7 AF to SPI1 (fcn 5)

	GPIOA->AFR[0] &= ~((0x000F << GPIO_AFRL_AFSEL5_Pos)); // clear nibble for bit 5 AF
	GPIOA->AFR[0] |= ((0x0005 << GPIO_AFRL_AFSEL5_Pos)); // set bit 5 AF to SPI1 (fcn 5)

	GPIOA->AFR[0] &= ~((0x000F << GPIO_AFRL_AFSEL4_Pos)); // clear nibble for bit 4 AF
	GPIOA->AFR[0] |= ((0x0005 << GPIO_AFRL_AFSEL4_Pos)); // set bit 4 AF to SPI1 (fcn 5)

	// call spi init function
	SPI_init();
}

void SPI_init(void) {
	// SPI config as specified @ STM32L4 RM0351 rev.9 p.1459
	// called by or with DAC_init()
	// build control registers CR1 & CR2 for SPI control of peripheral DAC
	// assumes no active SPI xmits & no recv data in process (BSY=0)
	// CR1 (reset value = 0x0000)
	SPI1->CR1 &= ~( SPI_CR1_SPE);             	// disable SPI for config
	SPI1->CR1 &= ~( SPI_CR1_RXONLY);          	// recv-only OFF
	SPI1->CR1 &= ~( SPI_CR1_LSBFIRST);        	// data bit order MSb:LSb
	SPI1->CR1 &= ~( SPI_CR1_CPOL | SPI_CR1_CPHA); // SCLK polarity:phase = 0:0
	SPI1->CR1 |= SPI_CR1_MSTR;              	// MCU is SPI controller
	// CR2 (reset value = 0x0700 : 8b data)
	SPI1->CR2 &= ~( SPI_CR2_TXEIE | SPI_CR2_RXNEIE); // disable FIFO intrpts
	SPI1->CR2 &= ~( SPI_CR2_FRF);              	// Moto frame format
	SPI1->CR2 |= SPI_CR2_NSSP;              	// auto-generate NSS pulse
	SPI1->CR2 |= SPI_CR2_DS;                	// 16-bit data
	SPI1->CR2 |= SPI_CR2_SSOE;              	// enable SS output
	// CR1
	SPI1->CR1 |= SPI_CR1_SPE;               	// re-enable SPI for ops
}

void DAC_write(uint16_t data) {
	SPI1->CR1 &= ~(SPI_CR1_SSI); // clear SSI bit,
								 // ensure SPI1 can communicate
	while (!(SPI1->SR & SPI_SR_TXE))
		; // if TXE is 1, break out of while
		  // loop, and write data to DR
	SPI1->DR = data;
	while (SPI1->SR & SPI_SR_BSY)
		; // SPI is busy in communication,
		  // TX buffer isn't empty if BSY
		  // set high, break out of loop
		  // when buffer is empty
	SPI1->CR1 |= SPI_CR1_SSI; // set SSI bit, re-enable slave select

}

// convert target voltage into a format suitable for the 12-bit
// DAC, where the reference voltage is 2.048 V
// V ref is the max voltage (2.048 V) when the gain is 1
// D max is = 4095 = 2^n - 1, where n is 12 bits

// target voltage should be in milivolts
uint16_t DAC_volt_conv(uint16_t target_voltage) {
	uint16_t DAC_val;
	int gain = 1; // gain is 1 by default
	uint16_t DAC_control_bits = 0x0000;

	if (target_voltage <= 3300) {
		DAC_control_bits = 0x3000; // 0b0011_0000_0000_0000
								   // bit 13 is output gain,
								   // clear for 2x output gain
								   // set for 1x output gain
								   // bit 12 is the shutdown
								   // control, set it for
								   // active mode
		if (target_voltage > 2048) {
			gain = 2;
			DAC_control_bits = 0x1000;
		}

		DAC_val = (1/1.01)*((target_voltage * 4095) / (gain * 2048))+ 3.86;
	}

	else {
		target_voltage = 3330;
		gain = 2;
		DAC_control_bits = 0x1000;
		DAC_val = (1/1.01)*((target_voltage * 4095) / (gain * 2048))+ 3.86;
	}

	return (DAC_control_bits | (DAC_val & 0x0FFF));
}

