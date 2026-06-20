/*******************************************************************************
 * EE 329 A3 LCD TIMER
 *******************************************************************************
 * @file           : lcd.c
 * @brief          : Keypad and LCD interfaced to create a countdown timer
 * project         : EE 329 F'24 Assignment 3
 * authors         : Mateo Charles - vcharles@calpoly.edu
 * citations		 : John Penvenne - jpenvenn@calpoly.edu
 * 					   -systick timer initialization
 * 					   -LCD initialization
 * 					   -pulse enable function
 * 					   -sending instructions to LCD in 4 nibble mode
 * 					   -writing characters to LCD
 * date            : 10/14/24
 * compiler        : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 *******************************************************************************
 *LCD WIRING (pinout NUCLEO-L4A6ZG = L496ZG)
 *
 *      peripheral – Nucleo I/O
 * 		LCD 1 VSS - GND CN8
 * 		LCD 2 VDD - 3.3V CN8
 * 		LCD 4 R/S - PF8 CN9
 * 		LCD 5 R/W - PF7 CN9
 * 		LCD 6 E   - PF9 CN9
 * 		LCD 11 DB4 - PF0 CN9
 * 		LCD 12 DB5 - PF1 CN9
 * 		LCD 13 DB6 - PF2 CN9
 * 		LCD 14 DB7 - PF3 CN8
 * 		LCD 15 LED+ - 3.3V CN8
 * 		LCD 16 LED- - GND CN8
 ****************************************************************************/

#include "lcd.h"

/* -----------------------------------------------------------------------------
 * function : SysTick_Init ()
 * INs      : none
 * OUTs     : none
 * action   : Systick timer configured in order to call delay_us(), a delay
 * 			  in microseconds
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void SysTick_Init( void );

/* -----------------------------------------------------------------------------
 * function : LCD_init ( )
 * INs      : none
 * OUTs     : none
 * action   : initializes LCD ports for R/S, R/W, E and data bits
 * 			  then sets and clears display for entry
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void LCD_init( void )  {

	RCC -> AHB2ENR   |=  (RCC_AHB2ENR_GPIOFEN);//enable pwr and clk
   														 //to GPIO port F

	//LCD data bits config
	LCD_PORT->MODER   &= ~((GPIO_MODER_MODE0| //clearing bottom 8 bits
   						   	GPIO_MODER_MODE1| //of MODE register
									GPIO_MODER_MODE2|
									GPIO_MODER_MODE3));

   LCD_PORT->MODER   |=  (GPIO_MODER_MODE0_0 | //setting IO mode to
   						  	  GPIO_MODER_MODE1_0 | //general purpose output
								  GPIO_MODER_MODE2_0 |
								  GPIO_MODER_MODE3_0);

   LCD_PORT->OTYPER  &= ~(GPIO_OTYPER_OT0 | //configure output type
   						  	  GPIO_OTYPER_OT1 | //to output push pull
								  GPIO_OTYPER_OT2 |
								  GPIO_OTYPER_OT3);

   LCD_PORT->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | //configure IO pins to
   						  	  GPIO_PUPDR_PUPD1 | //no pull up or pull down
								  GPIO_PUPDR_PUPD2 |
								  GPIO_PUPDR_PUPD3);

   LCD_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos) | //set logic
   						  	 (3 << GPIO_OSPEEDR_OSPEED1_Pos) | //transition
								 (3 << GPIO_OSPEEDR_OSPEED2_Pos) | //speed
								 (3 << GPIO_OSPEEDR_OSPEED3_Pos));

   //LCD RS,RW, E config
   LCD_PORT->MODER   &= ~((GPIO_MODER_MODE8| //clearing bottom 8 bits
      						   	GPIO_MODER_MODE7| //of MODE register
   									GPIO_MODER_MODE9));

   LCD_PORT->MODER   |=  (GPIO_MODER_MODE8_0 | //setting IO mode to
      						  	  GPIO_MODER_MODE7_0| //general purpose output
   								  GPIO_MODER_MODE9_0);

   LCD_PORT->OTYPER  &= ~(GPIO_OTYPER_OT8 | //configure output type
      						  	  GPIO_OTYPER_OT7 | //to output push pull
   								  GPIO_OTYPER_OT9);

   LCD_PORT->PUPDR   &= ~(GPIO_PUPDR_PUPD8 | //configure IO pins to
      						  	  GPIO_PUPDR_PUPD7 | //no pull up or pull down
   								  GPIO_PUPDR_PUPD9);

   LCD_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED8_Pos) | //set logic
      						 (3 << GPIO_OSPEEDR_OSPEED7_Pos) | //transition
   							 (3 << GPIO_OSPEEDR_OSPEED9_Pos)); //speed));



	delay_us( 40000 );                   // power-up wait 40 ms
   for ( int idx = 0; idx < 3; idx++ ) {// wake up 1,2,3: DATA = 0011 XXXX
      LCD_4b_command( 0x30 );           // HI 4b of 8b cmd, low nibble = X
      delay_us( 200 );
   }
   LCD_4b_command( 0x20 ); // fcn set #4: 4b cmd set 4b mode
   delay_us( 40 );
   LCD_command( 0x28 ); //fcn set: 4-bit/2-line
   delay_us( 40 );
   LCD_command( 0x10 ); //set cursor
   delay_us( 40 );
   LCD_command( 0x0F ); //display ON; blinking cursor
   delay_us( 40 );
   LCD_command( 0x06 ); //entry mode set
   delay_us( 40 );
   LCD_command( 0x01 ); //clears display
   delay_us( 40 );
}

/* -----------------------------------------------------------------------------
 * function : LCD_pulse_ENA ( )
 * INs      : none
 * OUTs     : none
 * action   : enable line sending command on falling edge, giving enough time
 * 			  for data to latch
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void LCD_pulse_ENA( void )  {
// ENAble line sends command on falling edge
// set to restore default then clear to trigger
   LCD_PORT->ODR   |= ( LCD_EN );         	// ENABLE = HI
   delay_us(500);                         // TDDR > 320 ns
   LCD_PORT->ODR   &= ~( LCD_EN );        // ENABLE = LOW
   delay_us( 500 );                         // low values flakey
}

/* -----------------------------------------------------------------------------
 * function : LCD_4b_command (  )
 * INs      : 8 bit unsigned command
 * OUTs     : none
 * action   : used for wake up commands
 * 			  or LCD initializations
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void LCD_4b_command( uint8_t command )  {
// LCD command using high nibble only - used for 'wake-up' 0x30 commands
   LCD_PORT->ODR   &= ~(LCD_DATA_BITS); 	// clear DATA bits
   LCD_PORT->ODR   |= ( command >> 4 );   // DATA = command
   delay_us( 500 );
   LCD_pulse_ENA( );
}

/* -----------------------------------------------------------------------------
 * function : LCD_command ( )
 * INs      : 8 bit unsigned commands
 * OUTs     : none
 * action   : sends instructions to the LCD 4 bits at a time
 * 			  by sending upper nibble first then lower
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void LCD_command( uint8_t command )  {
// send command to LCD in 4-bit instruction mode
// HIGH nibble then LOW nibble, timing sensitive
   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( (command>>4) & LCD_DATA_BITS ); // HIGH shifted low
   delay_us(500);
   LCD_pulse_ENA( );                                    // latch HIGH NIBBLE

   LCD_PORT->ODR   &= ~( LCD_DATA_BITS );               // isolate cmd bits
   LCD_PORT->ODR   |= ( command & LCD_DATA_BITS );      // LOW nibble
   delay_us( 500 );
   LCD_pulse_ENA( );                                    // latch LOW NIBBLE
}

/* -----------------------------------------------------------------------------
 * function : LCD_write_char ( )
 * INs      : 8 bit unsigned letter
 * OUTs     : none
 * action   : writes a character (given in hex or binary) to LCD at
 * 			  certain address
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void LCD_write_char( uint8_t letter )  {
// calls LCD_command() w/char data; assumes all ctrl bits set LO in LCD_init()
   LCD_PORT->ODR   |= (LCD_RS);       // RS = HI for data to address
   delay_us( 500 );
   LCD_command( letter );             // character to print
   LCD_PORT->ODR   &= ~(LCD_RS);      // RS = LO
}

/* -----------------------------------------------------------------------------
 * function : LCD_write_string ( )
 * INs      : string
 * OUTs     : none
 * action   : prints a string to the LCD
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void LCD_write_string( const char *str) {
	while ( *str != '\0'  ){
		LCD_write_char( *str );
		str++;
		delay_us( 60 );
	}
}

