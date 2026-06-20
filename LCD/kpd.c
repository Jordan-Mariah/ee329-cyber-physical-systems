/*******************************************************************************
 * EE 329 A3 LCD TIMER
 *******************************************************************************
 * @file           : kpd.c
 * @brief          : Keypad and LCD interfaced to create a countdown timer
 * project         : EE 329 F'24 Assignment 3
 * authors         : Mateo Charles - vcharles@calpoly.edu
 * citations		 : John Penvenne - jpenvenn@calpoly.edu
 * 					   -keypad configurations
 * 					   -functions that check to see if any key is pressed
 * 					   -function that returns an encoded key press
 * date            : 10/14/24
 * compiler        : STM32CubeIDE v.1.12.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 *******************************************************************************
 * KEYPAD WIRING 4 ROWS 3 COLS (pinout NUCLEO-L4A6ZG = L496ZG)
 *      peripheral – Nucleo I/O
 * keypad 1  COL 2 - PD5 = CN9 - 6
 * keypad 2  ROW 1 - PD0 = CN9 - 25
 * keypad 3  COL 1 - PD4 = CN9 - 8
 * keypad 4  ROW 4 - PD3 = CN9 - 10
 * keypad 5  COL 3 - PD6 = CN9 - 4
 * keypad 6  ROW 3 - PD2 = CN8 - 12
 * keypad 7  ROW 2 - PD1 = CN9 - 27
 *******************************************************************************/

#include "main.h"
#include "kpd.h"


/* -----------------------------------------------------------------------------
 * function : Keypad_Config ( )
 * INs      : none
 * OUTs     : none
 * action   : configures row pins to input mode and column
 * 			  pins to output mode
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
void Keypad_Config( void )  {

	//driving columns HIGH
	// configure COLUMS GPIOD pins PD4, PD5, PD6 for:
	// output mode, output push pull, no pull up or pull down
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIODEN); //enable pwr and clk
		                                         //to GPIO port D

	COL_PORT->MODER   &= ~((GPIO_MODER_MODE4| //clearing bottom 8 bits
									GPIO_MODER_MODE5| //of MODE register
									GPIO_MODER_MODE6));

	COL_PORT->MODER   |= (GPIO_MODER_MODE4_0 | //setting IO mode to
							  	 GPIO_MODER_MODE5_0 | //general purpose output
								 GPIO_MODER_MODE6_0);

	COL_PORT->OTYPER  &= ~(GPIO_OTYPER_OT4 | //configure output type
							  	  GPIO_OTYPER_OT5 | //to output push pull (RESET STATE) b'0
								  GPIO_OTYPER_OT6);

	COL_PORT->PUPDR   &= ~(GPIO_PUPDR_PUPD4 | //configure IO pins to
							  	  GPIO_PUPDR_PUPD5 | //no pull up or pull down
								  GPIO_PUPDR_PUPD6); // b'00

	COL_PORT->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED4_Pos) | //set logic
							  	  (3 << GPIO_OSPEEDR_OSPEED5_Pos) | //transition
								  (3 << GPIO_OSPEEDR_OSPEED6_Pos)); // | //speed

	COL_PORT->BRR = (GPIO_PIN_4 |
						  GPIO_PIN_5 | //preset bottom 4 bits/pins to 0
				  		  GPIO_PIN_6);

	//reading ROWS
	//configure ROWS GPIOD pins PD0, PD1, PD2, PD3 for:
	//INPUT MODE, no pull up or pull down

	ROW_PORT->MODER   &= ~(GPIO_MODER_MODE0| //clearing bottom 8 bits
								  GPIO_MODER_MODE1| //of MODE register
								  GPIO_MODER_MODE2| //already set to input mode b'00
								  GPIO_MODER_MODE3);

	ROW_PORT->OTYPER  &= ~(GPIO_OTYPER_OT0 | //configure output type
							  	  GPIO_OTYPER_OT1 | //to output push pull //doesnt matter really
								  GPIO_OTYPER_OT2 |
								  GPIO_OTYPER_OT3);

	ROW_PORT->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | //configure IO pins to
							  	  GPIO_PUPDR_PUPD1 | // pull down b'10
								  GPIO_PUPDR_PUPD2 |
								  GPIO_PUPDR_PUPD3);
	ROW_PORT->PUPDR   |= (GPIO_PUPDR_PUPD0_1 | //configure IO pins to
		  	  	  	  	  	    GPIO_PUPDR_PUPD1_1 | // pull down b'10
								 GPIO_PUPDR_PUPD2_1 |
								 GPIO_PUPDR_PUPD3_1);; //clearing bits to get pins b'10

	ROW_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos) | //set logic
							    (3 << GPIO_OSPEEDR_OSPEED1_Pos) | //transition
								 (3 << GPIO_OSPEEDR_OSPEED2_Pos) | //speed
								 (3 << GPIO_OSPEEDR_OSPEED3_Pos)); //not needed
																			//not driving output on these pins

	ROW_PORT -> BRR = (GPIO_PIN_0 | GPIO_PIN_1 |
							 GPIO_PIN_2 |GPIO_PIN_3); //present rows to 0
}


/* -----------------------------------------------------------------------------
 * function : Keypad_IsAnyKeyPressed( )
 * INs      : none
 * OUTs     : boolean
 * action   : -driving all columns high, checking rows for any high input
 * 			  -if a key is pressed returns true, else returns false
 * 			  -debouncer ensures key is actually pressed due to induced
 * 			   metastability from hardware
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * 			  Mateo Charles - vcharles@calpoly.edu
 * 			  -included debouncer
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
int Keypad_IsAnyKeyPressed( void ) {
// drive all COLUMNS HI; see if any ROWS are HI
// return true if a key is pressed, false if not

	COL_PORT->BSRR = COL_PINS;         	      // set all columns HI
   for ( uint16_t idx=0; idx<SETTLE; idx++ )   	// let it settle
      ;
   int DEBOUNCE_COUNT = 0;
   for ( int i = 0; i < DEBOUNCE_STOP; i++) {
   	if ((ROW_PORT->IDR & ROW_PINS) != 0 ) //got a key press
   		DEBOUNCE_COUNT++;
   	else
   		DEBOUNCE_COUNT = 0;
   }

   if ( DEBOUNCE_COUNT == DEBOUNCE_STOP ) {
   	return ( true );
   }
   else {
   	return ( false );
   }
}

/* -----------------------------------------------------------------------------
 * function : Keypad_WhichKeyIsPressed ()
 * INs      : none
 * OUTs     : encoded integer key
 * action   : assuming previous function Keypad_IsAnyKeyPressed() returns true,
 * 			  this function will return an encoded integer that corresponds
 * 			  to the actual pressed key
 * authors  : John Penvenne - jpenvenn@calpoly.edu
 * date     : 10/14/24
 * -------------------------------------------------------------------------- */
int Keypad_WhichKeyIsPressed( void ) {

	// detect and encode a pressed key at {row,col}
	// assumes a previous call to Keypad_IsAnyKeyPressed() returned TRUE
	// verifies the Keypad_IsAnyKeyPressed() result (no debounce here),
	// determines which key is pressed and returns the encoded key ID

	   int8_t iRow=0, iCol=0, iKey=0;  // keypad row & col index, key ID result
	   int8_t bGotKey = 0;             // bool for keypress, 0 = no press

	   COL_PORT->BSRR = COL_PINS;                       	 // set all columns HI
	   for ( iRow = 0; iRow < NUM_ROWS; iRow++ ) {      	 // check all ROWS
	      if ( ROW_PORT->IDR & (BIT0 << iRow) ) {      	 // keypress in iRow!!
	         COL_PORT->BRR = ( COL_PINS );            	 // set all cols LO
	         for ( iCol = 0; iCol < NUM_COLS; iCol++ ) {   // 1 col at a time
	            COL_PORT->BSRR = ( BIT0 << (4+iCol) );     // set this col HI
	            if ( ROW_PORT->IDR & (BIT0 << iRow) ) {    // keypress in iCol!!
	               bGotKey = 1;
	               break;                                  // exit for iCol loop
	            }
	         }
	      COL_PORT->BRR = ( BIT0 << (4+iCol) );
	      if ( bGotKey )
	            break;
	      }
	   }

	   //	encode {iRow,iCol} into LED word : row 1-3 : numeric, ‘1’-’9’
	   //	                                   row 4   : ‘*’=10, ‘0’=15, ‘#’=12
	   //                                    no press: send NO_KEYPRESS
	   if ( bGotKey ) {
	      iKey = ( iRow * NUM_COLS ) + iCol + 1;  // handle numeric keys ...
	 	if ( iKey == KEY_ZERO )                 //    works for ‘*’, ‘#’ too
	         iKey = CODE_ZERO;
	 	return( iKey );                         // return encoded keypress
	   }
	   return( NO_KEYPRESS );                     // unable to verify keypress
}


