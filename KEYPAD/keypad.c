#include "keypad.h"
#include <stdio.h>  // Needed for printf()

void delay(int count);  //Allows delay to be used before delay is defined

// Function to check if any key is pressed
int AnyKey(void) { //this function is adopted from the sample code in A1
    // Drive all columns high
    GPIOA->BSRR = COL_PINS;
    // Small delay for signal to settle
    for (uint16_t idx = 0; idx < 1000; idx++);

    // Check if any row is high (key pressed), each row is low and will be set high when a key is pressed.
    if ((GPIOD->IDR & ROW_PINS) != 0) {
        return 1;  // Key pressed
    } else {
        return 0;  // No key pressed
    }
}

// Now that we know a key was pressed use this to determine which key is pressed
int WhichKey(void) { //this function is adopted from the sample code in A1.
	//and we used chat GPT to debug braces errors
    int8_t iRow, iCol;
    int8_t keyDetected = 0;



    // Set all columns high again
    GPIOA->BSRR = COL_PINS;



    // Detect the row
    for (iRow = 0; iRow < NUM_ROWS; iRow++) {
        if (GPIOD->IDR & (1 << iRow)) {

            // Clear all columns
            GPIOA->BRR = COL_PINS;
            // Check each column by setting one at a time high

            for (iCol = 0; iCol < NUM_COLS; iCol++) {
                GPIOA->BSRR = (1 << (iCol));  // Set the column high

                if (GPIOD->IDR & (1 << iRow)) {  //Key press in this column
                    int stable = 1;

                    for (int debounce_check = 0; debounce_check < DEBOUNCE_COUNT; debounce_check++) {
                        delay(DEBOUNCE_DELAY);  // Introduce delay

                        // Check the key state again
                        if (!(GPIOD->IDR & (1 << iRow))) {
                            stable = 0;  // Key is not pressed
                            break;       // Exit loop if key is released
                        }
                    }

                    // Check if the key was stable
                    if (stable == 1) {
                        keyDetected = 1;  // Key is confirmed pressed
                        break;            // Exit the column loop

                    }

                }
                GPIOA->BRR = (1 << (iCol));  // Set the column low again
            }

            if (keyDetected) break;
        }
    }

    // Map row/column to a key value
    if (keyDetected) {
        if (iRow == 0 && iCol == 0) return 1;   // Key 1
        if (iRow == 0 && iCol == 1) return 2;   // Key 2
        if (iRow == 0 && iCol == 2) return 3; // Key 3
        if (iRow == 1 && iCol == 0) return 4;  // Key 4
        if (iRow == 1 && iCol == 1) return 5;  // Key 5
        if (iRow == 1 && iCol == 2) return 6;   // Key 6
        if (iRow == 2 && iCol == 0) return 7; // Key 7
        if (iRow == 2 && iCol == 1) return 8; // Key 8
        if (iRow == 2 && iCol == 2) return 9;  // Key 9
        if (iRow == 3 && iCol == 0) return 10;  // Key *
        if (iRow == 3 && iCol == 1) return 11;  // Key 0
        return 12;  // Key #
    } else {
        return -1;  // No key was pressed
    }
}


