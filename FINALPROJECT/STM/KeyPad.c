/* -----------------------------------------------------------------------------
 * keypad.h
 * -----------------------------------------------------------------------------
 * @file keypad.h
 * @brief Header file for 4x3 Keypad interface configuration and operations.
 *
 * This module provides functions to configure the GPIO pins connected to a
 * 4x3 keypad, detect key presses, and retrieve the corresponding key values.
 *
 * ### Attributions:
 * - **Main Body of Code:** Keigan Leonard
 * - **Customizations and Adaptations:** William Leiker (wml)
 *
 * ### Hardware Connections:
 * - **Columns:** Connect to PD0, PD1, PD2 (left-to-right).
 * - **Rows:** Connect to PD3, PD4, PD5, PD6 (top-to-bottom).
 *
 * ### Steps to Use:
 * 1. Call `Keypad_Config()` to initialize GPIO for keypad operation.
 * 2. Use `Grab_KeyPress()` to detect and retrieve the pressed key value.
 *
 * ### Dependencies:
 * - Requires inclusion of `stm32l4xx.h` and `main.h`.
 *
 * ### Example:
 * ```c
 * Keypad_Config();
 * uint8_t pressedKey = Grab_KeyPress();
 * ```
 * -----------------------------------------------------------------------------
 */
#include "KeyPad.h"
#include "main.h"

/* USER CODE BEGIN Keypad_Config */
void Keypad_Config(void)  {

	/* Configure Registers:
	 *
     * Cols: output; push-pull; very high speed; pull-down
	 * Rows: input; push-pull; very high speed; pull-down
	 *
     */
	RCC->AHB2ENR	|=  (RCC_AHB2ENR_GPIODEN);

	//clears the mode registers
	KYPD_Port->MODER   &= ~(GPIO_MODER_MODE0|
						    GPIO_MODER_MODE1|
							GPIO_MODER_MODE2|
							GPIO_MODER_MODE3|
							GPIO_MODER_MODE4|
							GPIO_MODER_MODE5|
							GPIO_MODER_MODE6|
							GPIO_MODER_MODE7);

	//sets 3 columns
	KYPD_Port->MODER   |=  (GPIO_MODER_MODE0_0|
						  	GPIO_MODER_MODE1_0|
							GPIO_MODER_MODE2_0);

	//Sets output push pull/open-drain
	KYPD_Port->OTYPER  &= ~(GPIO_OTYPER_OT0|
						    GPIO_OTYPER_OT1|
							GPIO_OTYPER_OT2|
							GPIO_OTYPER_OT3|
							GPIO_OTYPER_OT4|
							GPIO_OTYPER_OT5|
							GPIO_OTYPER_OT6|
							GPIO_OTYPER_OT7);

	//Clears Pull-up/pull-down registers
	KYPD_Port->PUPDR   &= ~(GPIO_PUPDR_PUPD0|
						    GPIO_PUPDR_PUPD1|
							GPIO_PUPDR_PUPD2|
							GPIO_PUPDR_PUPD3|
							GPIO_PUPDR_PUPD4|
							GPIO_PUPDR_PUPD5|
							GPIO_PUPDR_PUPD6|
							GPIO_PUPDR_PUPD7);
    //Set pull-down
	KYPD_Port->PUPDR   |= (GPIO_PUPDR_PUPD0_1|
						   GPIO_PUPDR_PUPD1_1|
						   GPIO_PUPDR_PUPD2_1|
						   GPIO_PUPDR_PUPD3_1|
						   GPIO_PUPDR_PUPD4_1|
						   GPIO_PUPDR_PUPD5_1|
						   GPIO_PUPDR_PUPD6_1|
						   GPIO_PUPDR_PUPD7_1);

	//sets output speed to very high
	KYPD_Port->OSPEEDR |=  ((3 << GPIO_OSPEEDR_OSPEED0_Pos) |
						   (3 << GPIO_OSPEEDR_OSPEED1_Pos)|
						   (3 << GPIO_OSPEEDR_OSPEED2_Pos));
}
/* USER CODE END Keypad_Config */

// --------------------------------------------------------------------------

/* USER CODE BEGIN IsAnyKeyPressed */
int Keypad_IsAnyKeyPressed(void) {
	// drive all COLUMNS HI; see if any ROWS are HI
	// return true if a key is pressed, false if not
	// currently no debounce here - just looking for a key twitch

	KYPD_Port->BSRR = COL_PINS;         	      // set all columns HI
	for ( uint16_t idx=0; idx<SETTLE; idx++ )   	// let it settle
		;

	if ((KYPD_Port->IDR & ROW_PINS) != 0){        // got a keypress!
		return( TRUE );
	}
	else{
		return( FALSE );
	}                          // nope.
}
/* USER CODE END IsAnyKeyPressed */

// --------------------------------------------------------------------------

/* USER CODE BEGIN WhichKeyIsPressed */
int Keypad_WhichKeyIsPressed(void) {
	// detect and encode a pressed key at {row,col}
	// assumes a previous call to Keypad_IsAnyKeyPressed() returned TRUE
	// verifies the Keypad_IsAnyKeyPressed() result (no debounce here),
	// determines which key is pressed and returns the encoded key ID

	int8_t iRow=0, iCol=0, iKey=0;  // keypad row & col index, key ID result
	int8_t bGotKey = 0;             // bool for keypress, 0 = no press

	KYPD_Port->BSRR = COL_PINS;                       // set all columns HI
	for ( iRow=0; iRow < NUM_ROWS; iRow++) {      	  // check all ROWS
		if (KYPD_Port->IDR & (1 << (3+iRow))) {       // keypress in iRow!!
			KYPD_Port->BRR = ( COL_PINS );           	// set all cols LO
			for ( iCol=0; iCol < NUM_COLS; iCol++ ) {   // 1 col at a time
				KYPD_Port->BSRR = ( 1 << (iCol) );     	// set this col HI
				if (KYPD_Port->IDR & (1 << (3+iRow))){  // keypress in iCol!!
					bGotKey = 1;
					break;                              // exit for iCol loop
				}
			}
			if ( bGotKey )
				break;
		}
	}

	//	encode {iRow,iCol} into LED word : row 1-3 : numeric, ‘1’-’9’
	//	                                   row 4   : ‘*’=10, ‘0’=11, ‘#’=12
	//                                     no press: send NO_KEYPRESS
	if ( bGotKey ) {
		iKey = ( iRow * NUM_COLS ) + iCol + 1;  	// handle numeric keys

		switch (iKey) {					// handle non-numeric keys
			case KEY_STAR:
				iKey = CODE_STAR;		// star = 14
				break;
			case KEY_ZERO:
				iKey = CODE_ZERO;		// zero = 0
				break;
			case KEY_POUND:
				iKey = CODE_POUND;		// pound = 15
				break;
			default:
				iKey = iKey;
		}
		return(iKey);
	}
	return(NO_KEYPRESS);                // unable to verify keypress
}
/* USER CODE END WhichKeyIsPressed */

// -----------------------------------------------------------------------------

/* USER CODE BEGIN Debounce */
int debounce(int key) {
    static int stable_key = -1; // Track the last stable key
    static int cnt = 0;         // Counter for stability detection

    if (key == stable_key) {    // If the key is stable and the same as last detected
        cnt++;
        if (cnt >= deb_cnt) {   // Stable for the required cycles
            return TRUE;        // Confirm a stable key press
        }
    } else {
        stable_key = key;       // Update to new key
        cnt = 0;                // Reset counter for the new key
    }
    return FALSE;               // Not stable yet
}

/* USER CODE END Debounce */

// -----------------------------------------------------------------------------

/* USER CODE BEGIN Grab_KeyPress */
int Grab_KeyPress(void) {
    int key = 0;

    while (1) {
    	GPIOB->ODR |= (1 << 7); // Turn ON LED
        if (Keypad_IsAnyKeyPressed()) {                // Checks if any key is pressed
            key = Keypad_WhichKeyIsPressed();          // Finds which key is pressed
            if (debounce(key)) {                       // Ensures stable keypress
                if (key != NO_KEYPRESS) {
                	GPIOC->BRR = 0xF;				   // First, turn off all LEDs
                    GPIOC->BSRR |= (key);              // Set LEDs for feedback
                    while (Keypad_IsAnyKeyPressed());  // Wait until the key is released
                    GPIOB->ODR &= ~(1 << 7); // Turn ON LED
                    return key;                        // Return the stable key
                }
            }
        }
    }
}

/* USER CODE END Grab_KeyPress */












