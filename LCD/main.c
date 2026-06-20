/*******************************************************************************
 * EE 329 A3 LCD TIMER
 *******************************************************************************
 * @file           : main.c
 * @brief          : Keypad and LCD interfaced to create a countdown timer
 * project         : EE 329 F'24 Assignment 3
 * authors         : Mateo Charles - vcharles@calpoly.edu
 * citations 	    : Wyatt Tack - wtack@calpoly.edu
 * 					   	-implemented the nested for loop countdown timer
 * 					   	 during int main( void )
 * date            : 10/14/24
 * compiler        : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 *******************************************************************************
 *PLAN: -cycle through keys until 4 digits are entered R-L
 *       for a timer
 *		  -if the timer gets digits above an hour, they become
 *		  	capped at 59:59
 *		  -at any time, during timer input or countdown, the reset button
 *		  	is valid and resets the timer
 *		  -if the timer reaches 0, turn an LED high and wait for the '#'
 *		  	key to be pressed. Once pressed the LED turns off and the timer
 *		  	is restarted
 *******************************************************************************
 *LINK FOR DEMO: https://youtu.be/TAaIy3yj-dA
 ******************************************************************************/

#include "main.h"
#include "lcd.h"
#include "kpd.h"
#include "delay.h"

/* -----------------------------------------------------------------------------
 * function : SystemClock_Config ()
 * INs      : none
 * OUTs     : none
 * action   : configures SysTick timer to utilize delay_us(0
 * 			  for specified delay
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void SystemClock_Config( void );

/* -----------------------------------------------------------------------------
 * function : MX_GPIO_Init( )
 * INs      : none
 * OUTs     : none
 * action   : configures and initializes GPIO ports
 * authors  : STM32CubeIDE
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
static void MX_GPIO_Init( void );

//Global vars
/*int key_num = 0;*/
uint16_t lcd_cursor = 0xCB;
int key = 0;

uint16_t tenmin_input = 0x00; //key inputs
uint16_t onemin_input = 0x00;
uint16_t tensec_input = 0x00;
uint16_t onesec_input = 0x00;

//bools
bool lastkey = false;
bool begincount = false;
bool exitloop = false;

int main( void ) {

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	SysTick_Init(); //enabling systick timer
	LCD_init(); //initialize LCD
	Keypad_Config(); // activate keypad configuration from keypad.c

	// configure GPIO pins PC0, PC1, PC2, PC3 for LEDS:
	// output mode, push-pull, no pull up or pull down, high speed
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOCEN); //enable pwr and clk
			                                         //to GPIO port C
	GPIOC->MODER   &= ~((GPIO_MODER_MODE0| //clearing bottom 8 bits
								GPIO_MODER_MODE1| //of MODE register
								GPIO_MODER_MODE2|
								GPIO_MODER_MODE3));
	GPIOC->MODER   |=  (GPIO_MODER_MODE0_0 | //setting IO mode to
							  GPIO_MODER_MODE1_0 | //general purpose output
							  GPIO_MODER_MODE2_0 |
							  GPIO_MODER_MODE3_0);
	GPIOC->OTYPER  &= ~(GPIO_OTYPER_OT0 | //configure output type
							  GPIO_OTYPER_OT1 | //to output push pull
							  GPIO_OTYPER_OT2 |
							  GPIO_OTYPER_OT3);
	GPIOC->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | //configure IO pins to
							  GPIO_PUPDR_PUPD1 | //no pull up or pull down
							  GPIO_PUPDR_PUPD2 |
							  GPIO_PUPDR_PUPD3);
	GPIOC->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) | //set logic
							  (3 << GPIO_OSPEEDR_OSPEED1_Pos) | //transition
							  (3 << GPIO_OSPEEDR_OSPEED2_Pos) | //speed
							  (3 << GPIO_OSPEEDR_OSPEED3_Pos));
	GPIOC->BRR = (GPIO_PIN_0 | GPIO_PIN_1 | //preset bottom 4 bits/pins to 0
					  GPIO_PIN_2 | GPIO_PIN_3);

	//printing out opening message
	LCD_PORT->ODR   &= ~( LCD_RS ); //set RS LO
	delay_us( 300 );
	LCD_command( 0x80 ); //set cursor to top line leftmost block
	delay_us( 300 );
	LCD_write_string( "EE 329 A3 TIMER " ); //write string
	delay_us( 300 );
	LCD_command( 0xC0 ); //set cursor to bottom line leftmost block
	LCD_write_string( "*=SET #=GO " ); //write string
	delay_us( 300 );

	uint16_t tenmin_pos = 0xCB; //timer positions
	uint16_t onemin_pos = 0xCC;
	uint16_t tensec_pos = 0xCE;
	uint16_t onesec_pos = 0xCF;

	reset_timer(); //reseting timer to take inputs

	while ( 1 ) {

		tenmin_input = 0x00; //reset key inputs
		onemin_input = 0x00;
		tensec_input = 0x00;
		onesec_input = 0x00;

		while ( lcd_cursor != 0x00 ) {

			//while cursor var is on tenmin_pos
			while( lcd_cursor == tenmin_pos ) {
				if ( Keypad_IsAnyKeyPressed() ) { //check key press
					delay_us( 1600 );
					key = Keypad_WhichKeyIsPressed();
					if ( key == 10 ) { //if * is pressed
						reset_timer(); //reset timer
						lcd_cursor = 0xCB; //move cursor var to tenmin_pos
					}
					else { //reset wasnt pressed
						if ( key == 15 ) { //if 0 is pressed
							LCD_command( tenmin_pos ); //move cursor
							LCD_write_char( 0x30 ); //write key
							delay_us( 1600 );
							lcd_cursor++; //increment cursor var
							tenmin_input = 0x30; //save input
						}
						else { //other key
							LCD_command( tenmin_pos ); //move cursor
							LCD_write_char( key + 0x30 ); //print key to LCD
							delay_us( 1600 );
							lcd_cursor++; //increment cursor var
							tenmin_input = key + 0x30; //save input
						}
					}
				}
				else { // go back to while loop to remain in tenmin_pos
				}
			}

			//while cusrsor var is on onemin_pos
			while( lcd_cursor == onemin_pos ) {
				if ( Keypad_IsAnyKeyPressed() ) { //check key press
					delay_us( 1600 );
					key = Keypad_WhichKeyIsPressed();
					if (key == 10) { //if * is pressed
						reset_timer(); //reset
						lcd_cursor = 0xCB; //move cursor var to tenmin_pos
					}
					else { //reset wasnt pressed
						if ( key == 15 ) { //if 0 is pressed
							LCD_command( onemin_pos ); //move cursor
							LCD_write_char( 0x30 ); //write to LCD
							delay_us(1600);
							lcd_cursor++; //increment cursor var
							onemin_input = 0x30; //save input
						}
						else { //other key
							LCD_command( onemin_pos ); //move cursor
							LCD_write_char( key + 0x30 ); //write to LCD
							delay_us( 1600 );
							lcd_cursor++; //increment cursor var
							onemin_input = key + 0x30; //save input
						}
					}
				}
				else { //go back to while loop to remain in onemin_pos
				}
			}

			//cursor moved to ':'
			while ( lcd_cursor == 0xCD ) {//while cursor var is here
				LCD_command( 0xCD ); //move cursor
				LCD_write_char( ':' );//write ':' to LCD
				delay_us( 1000 );
				lcd_cursor++; //increment cursor var
				delay_us( 1000 );
			}

			//while cursor var is on tensec_pos
			while( lcd_cursor == tensec_pos ) {
				if ( Keypad_IsAnyKeyPressed() ) { //check key press
					delay_us( 1600 );
					key = Keypad_WhichKeyIsPressed();
					if ( key == 10 ) { //if * is pressed
						reset_timer(); //reset
						lcd_cursor = 0xCB; //move cursor var to tenmin_pos
					}
					else { //reset wasnt pressed
						if ( key == 15 ) { //if 0 is pressed
							LCD_command( tensec_pos ); //move cursor
							LCD_write_char( 0x30 ); //write to LCD
							delay_us( 1600 );
							lcd_cursor++; //increment cursor var
							tensec_input = 0x30; //save input
						}
						else { //other key
							LCD_command( tensec_pos ); //move cursor
							LCD_write_char( key + 0x30 ); //write to LCD
							delay_us( 1600 );
							lcd_cursor++; //increment cursor var
							tensec_input = key + 0x30; //save input
						}
					}
				}
				else { //do nothing
				}
			}

			//while cursor is on onesec_pos
			while( lcd_cursor == onesec_pos ) {
				if ( Keypad_IsAnyKeyPressed() ) { //check key press
					delay_us(1600);
					key = Keypad_WhichKeyIsPressed();
					if ( key == 10 ) { //if * is pressed
						reset_timer(); //reset timer
						lcd_cursor = 0xCB; //move cursor var to tenmin_pos
					}
					else { //reset wasnt pressed
						if ( key == 15 ) { //if 0 is pressed
							LCD_command( onesec_pos ); //move cursor
							LCD_write_char( 0x30 ); //write to LCD
							delay_us( 1600 );
							onesec_input = 0x30; //save input
							lastkey = true; //last key was entered
							break;
						}
						else { //other key pressed
							LCD_command( onesec_pos ); //move cursor
							LCD_write_char( key + 0x30 ); //write to LCD
							delay_us( 1600 );
							lcd_cursor = onesec_pos; //set cursor var
							onesec_input = key + 0x30; //save input
							lastkey = true;//last key was entered
							break;
						}
					}
				}
				else {//do nothing}
				}
			}

			while ( lastkey ) { //checking for # to start timer
				if( Keypad_IsAnyKeyPressed() ) {
					key = Keypad_WhichKeyIsPressed();
					if ( key == 12 ) { //begin count
						begincount = true; //change bool to enable timer
						break;
					}
					else if ( key == 10 ) {//if reset is pressed
						reset_timer(); //reset
						lcd_cursor = 0xCB; //move cursor back to tenmin_pos
						break;
					}
				}
			}

			if ( begincount == true ){
				break; //breaking starting while loop to begin timer
			}
		}

		//checking for capped input
		if ( tenmin_input > 0x35 ) {
			tenmin_input = 0x35;
			onemin_input = 0x39;
		}
		if ( tensec_input > 0x35 ) {
			tensec_input = 0x35;
			onesec_input = 0x39;
		}

		//vars for nested for loop timer
		uint16_t tenmin = 0x00;
		uint16_t onemin = 0x00;
		uint16_t tensec = 0x00;
		uint16_t onesec = 0x00;

		//start counting

		//10min
		for ( tenmin = tenmin_input; tenmin > 0x2F; tenmin-- ) {
			lcd_cursor = 0xCB;
			LCD_command( 0xCB ); //setting cursor to tenmin pos
			LCD_write_char( tenmin );
			//1min
			for ( onemin = onemin_input; onemin > 0x2F; onemin-- ) {
				lcd_cursor = 0xCC;
				LCD_command( 0xCC );
				LCD_write_char ( onemin );
				//10sec
				for ( tensec = tensec_input; tensec > 0x2F; tensec-- ) {
					lcd_cursor = 0xCE;
					LCD_command( 0xCE );
					LCD_write_char ( tensec );
					//1 sec
					for ( onesec = onesec_input; onesec > 0x2F; onesec-- ) {
						lcd_cursor = 0xCF;
						LCD_command( 0xCF );
						LCD_write_char ( onesec );
						delay_us( 908000 );
						if ( Keypad_IsAnyKeyPressed() ) { //look for reset
							key = Keypad_WhichKeyIsPressed();
							if ( key == 10 ) {
								lcd_cursor = 0xCB;
								reset_timer();
								exitloop = true;
								break;
							}
						} // continue
						if ( exitloop ) {
							break;
						}
						else {
						}
					}
					if ( exitloop ) {
						break;
					}
					else { //onesec_pos has reached 0
							 //reset to 9
						onesec_input = 0x39;
					}
				}
				if ( exitloop ) {
					break;
				}
				else { //1 min has passed reset
						 //tensec_pos to 5
					tensec_input = 0x35;
				}
			}
			if ( exitloop ) {
				break;
			}
			else { //10min have passed
					 //reset onemin_pos to 9
				onemin_input = 0x39;
			}
		} //timer has ended

		//if no reset was pressed during timer
		//timer has ended and LED is ON
		if ( exitloop == false ) {
			GPIOC -> ODR = 1;
		}

		//if there was a reset, the break chain
		//broke from for loop timer
		//now reset all key inputs for new timer
		if ( exitloop == true ) {
			tenmin_input = 0x00;
			onemin_input = 0x00;
			tensec_input = 0x00;
			onesec_input = 0x00;
		}

		//if there was no reset look for a * or # to
		//turn off LED and begin timer loop again
		while ( exitloop == false ) { //looking for * or #
			if ( Keypad_IsAnyKeyPressed() ) {
				key = Keypad_WhichKeyIsPressed();
				if ( (key == 12) || (key == 10) ) { // a restart
					lcd_cursor = 0xCB;
					GPIOC -> BRR = 0x0F;
					reset_timer();
					exitloop = false;
					break;
				}
			}
			else {//keep looking until LEDS off
			}
		}
	}
}

/* ---------------------------------------------------------------------------
 * function : reset_timer ( )
 * INs      : none
 * OUTs     : none
 * action   : resets timer by changing timer value to
 * 			  '00:00' and resets bools and variables
 * 			  used throughout timer entry
 * authors  : Mateo Charles - vcharles@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void reset_timer ( void ) {//when * key is pressed

	//rewrite timer display to '00:00'
	for ( uint16_t index = 0xCB; index < 0xD0; index++ ) {
		if ( index == 0xCD) {
			LCD_command(index); //set address
			LCD_write_char(':'); //write char
			delay_us(800);
		}
		else {
			LCD_command(index); //set address
			LCD_write_char('0'); //write char
			delay_us(800);
		}
	}

	//reset vars
	lcd_cursor = 0xCB;
	LCD_command(0xCB); //place cursor back to min marker
	delay_us(800);

	//reset bools
	lastkey = false;
	begincount = false;
	exitloop = false;

	//reset key inputs
	tenmin_input = 0x00;
	onemin_input = 0x00;
	tensec_input = 0x00;
	onesec_input = 0x00;
}

void SystemClock_Config( void ) {
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
/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init( void ) {
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_GPIOH_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();
 __HAL_RCC_GPIOG_CLK_ENABLE();
 HAL_PWREx_EnableVddIO2();
 __HAL_RCC_GPIOA_CLK_ENABLE();
 /*Configure GPIO pin Output Level */
 HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);
 /*Configure GPIO pin Output Level */
 HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);
 /*Configure GPIO pin : B1_Pin */
 GPIO_InitStruct.Pin = B1_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
 /*Configure GPIO pins : LD3_Pin LD2_Pin */
 GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 /*Configure GPIO pin : USB_OverCurrent_Pin */
 GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);
 /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
 GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);
 /*Configure GPIO pins : STLK_RX_Pin STLK_TX_Pin */
 GPIO_InitStruct.Pin = STLK_RX_Pin|STLK_TX_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
 GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
 HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
 /*Configure GPIO pins : USB_SOF_Pin USB_ID_Pin USB_DM_Pin USB_DP_Pin */
 GPIO_InitStruct.Pin = USB_SOF_Pin|USB_ID_Pin|USB_DM_Pin|USB_DP_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
 GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}
/* USER CODE BEGIN 4 */
/* USER CODE END 4 */
/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
 __disable_irq();
 while (1)
 {
 }
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




