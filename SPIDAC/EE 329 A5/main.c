/* USER CODE BEGIN Header */
/**
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
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DAC.h"
#include "keypad.h"
#include "delay.h"

void SystemClock_Config(void);

int main(void) {

	HAL_Init();
	SystemClock_Config();

	DAC_init();

	// part 7
	/*int DAC_sixteen_bit_word;
	DAC_sixteen_bit_word = DAC_volt_conv(2482);
	while (1) {
		DAC_write(DAC_sixteen_bit_word);
	}*/


	Keypad_Config();

	uint16_t target_voltage = 0;
	int digit_count = 0;
	int key = 0;

	// while loop for continuous writing to the dac
	while (1) {
		while (digit_count < 3) { // if less than three keys
									 // get pressed, then store
									 // values
			// wait for the key press
			while (!Keypad_IsAnyKeyPressed());
			// if a key is pressed, returns true, which gets negated to false,
			// breaks out of the loop to do keypad handling
			// is a key isn't pressed returns false, which gets negated to true,
			// which will keep us in this loop, until a key is pressed
			// get the key press
			key = Keypad_WhichKeyIsPressed();
			// if the asterisk is pressed
			if (key == 10) {
				break;
			}
			// update voltage from left to right
			if (digit_count == 0) {
				target_voltage += key * 1000;
				// ones place, ex: 2V = 2000 mV
			} else if (digit_count == 1) {
				target_voltage += key * 100;
				// tenths place, ex: 0.2 V = 200 mV
			} else if (digit_count == 2) {
				target_voltage += key * 10;
				// hundreths place, ex: 0.02 = 20 mV
			}
			// increment the number of keys pressed
			digit_count++;
			// let it settle
			delay_us(300000);
		}
		// delay once more
		delay_us(300000);
		// reset the number of keys pressed
		digit_count = 0;

		// cap the voltage to 3.3 V
		if (target_voltage > 3300) {
			target_voltage = 3300;
		}
		// if there is no reset, write voltage to DAC
		if (key != 10) {
			DAC_write(DAC_volt_conv(target_voltage));
		}
		// set the target_voltage back to zero for a new
		// loop
		target_voltage = 0;
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
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
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */

void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
