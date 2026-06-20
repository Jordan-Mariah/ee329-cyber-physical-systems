/*
 * keypad.c
 *
 *  Created on: Oct 2, 2024
 *  Author: asakd (Andrew Sakdikul)
 *  Class: EE 329 Fall 2024
 *  Assignment: A2
 */

#include "keypad.h"
#include <stdbool.h> // added to use true and false boolean values.
// --------------------------------------------------- excerpt from keypad.c --
void Keypad_Config(void) {
	// configure the seven pins needed to use the keypad itself
	// Four rows and three columns

	// Columns are PD4, PD5, PD6, three columns
	RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIODEN); // give power to peripherals
	// columns are high, and they are also outputs
	COL_PORT->MODER   &=  ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |
	  	  	  	  	  	  	GPIO_MODER_MODE6 );
	COL_PORT->MODER   |=  (GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 |
			  	  	  	   GPIO_MODER_MODE6_0 ); // output mode, 01
	COL_PORT->OTYPER   &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 |
	  	  	  	  	  	    GPIO_OTYPER_OT6 ); // push pull is 0
	COL_PORT->PUPDR   &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 |
					       GPIO_PUPDR_PUPD6 );
					       // clear lower 4 pins using mask
	                       // 00, no pull up or pull down
	COL_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED4_Pos) | // high speed, 10
            			  (3 << GPIO_OSPEEDR_OSPEED5_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED6_Pos) );
	COL_PORT->BRR = (GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 );
					// preset PD0, PD1, PD2 to 0

	// rows are PD0, PD1, PD2, PD3, four rows
	ROW_PORT->MODER   &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1 |
  	  	  	  			   GPIO_MODER_MODE2 | GPIO_MODER_MODE3 );
						 // input mode, 00
	ROW_PORT->OTYPER   &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 |
		  	  	  	  	    GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 );
						  // push pull, 0
	ROW_PORT->PUPDR   &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 |
		       	   	   	   GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 );
						 // clear lower 4 pins using mask
	ROW_PORT->PUPDR   |=  (GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD1_1 |
						   GPIO_PUPDR_PUPD2_1 | GPIO_PUPDR_PUPD3_1 );
						 // bitwise or with a bit-shifted mask
						 // 10, represents pull down on rows, which are inputs
	ROW_PORT->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED0_Pos) | // high speed, 10
			  	  	  	  (3 << GPIO_OSPEEDR_OSPEED1_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED2_Pos) |
						  (3 << GPIO_OSPEEDR_OSPEED3_Pos) );
	ROW_PORT->BRR = (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
				    // preset the rows to zero
}

// -----------------------------------------------------------------------------
int Keypad_IsAnyKeyPressed(void) {
// drive all COLUMNS HI; see if any ROWS are HI
// return true if a key is pressed, false if not

   uint16_t DEBOUNCE_COUNT = 0;
   uint16_t DEBOUNCE_STOP = 10;
   COL_PORT->BSRR = COL_PINS;         	      // set all columns HI
   for ( uint16_t idx=0; idx<SETTLE; idx++ )   	// let it settle
      ;
   for(uint16_t i = 0; i < DEBOUNCE_STOP; i++) { // debounce loop checks ten
	   	   	   	   	   	   	   	   	   	   	   	 // times
	   if ((ROW_PORT->IDR & ROW_PINS) != 0 ) {       // got a keypress!
		  DEBOUNCE_COUNT = DEBOUNCE_COUNT + 1;   // increment the counter
	   }
	   else {
		  DEBOUNCE_COUNT = 0;                    // if not pressed, count = 0
	   }
	   if (DEBOUNCE_COUNT == DEBOUNCE_STOP){     // if count = 10
		  return( true );                        // key is reliably pressed
	   }
   }
   return( false );                          // nope.
}

// -----------------------------------------------------------------------------
int Keypad_WhichKeyIsPressed(void) {
// detect and encode a pressed key at {row,col}
// assumes a previous call to Keypad_IsAnyKeyPressed() returned TRUE
// verifies the Keypad_IsAnyKeyPressed() result (no debounce here),
// determines which key is pressed and returns the encoded key ID

   int8_t iRow=0, iCol=0, iKey=0;  // keypad row & col index, key ID result
   int8_t bGotKey = 0;             // bool for keypress, 0 = no press

   COL_PORT->BSRR = COL_PINS;                       	 // set all columns HI
   for ( iRow = 0; iRow < NUM_OF_ROWS; iRow++ ) {      	 // check all ROWS
      if ( ROW_PORT->IDR & (BIT0 << iRow) ) {      	 // keypress in iRow!!
         COL_PORT->BRR = ( COL_PINS );            	 // set all cols LO
         for ( iCol = 0; iCol < NUM_OF_COLS; iCol++ ) {   // 1 col at a time
            COL_PORT->BSRR = ( BIT0 << (4+iCol) );     // set this col HI
            if ( ROW_PORT->IDR & (BIT0 << iRow) ) {    // keypress in iCol!!
               bGotKey = 1;
               break;                                  // exit for iCol loop
            }
         }
         if ( bGotKey )
            break;
      }
   }
   //	encode {iRow,iCol} into LED word : row 1-3 : numeric, ‘1’-’9’
   //	                                   row 4   : ‘*’=10, ‘0’=15, ‘#’=12
   //                                    no press: send NO_KEYPRESS
   if ( bGotKey ) {
      iKey = ( iRow * NUM_OF_COLS ) + iCol + 1;  // handle numeric keys ...
      if ( iKey == KEY_ZERO )                 //    works for ‘*’, ‘#’ too
         iKey = CODE_ZERO;
      return( iKey );                         // return encoded keypress
   }
   return( NO_KEYPRESS );                     // unable to verify keypress
}
