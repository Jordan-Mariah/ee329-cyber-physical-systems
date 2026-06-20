#include "main.h"
#include "isr.h"
#include "lcd.h"    // Include the LCD header
#include "delay.h"

// Pin definitions
#define BUTTON_PIN 13   // Assuming the reset button is connected to PC13
#define BUTTON_PORT GPIOC  // GPIOC port
#define LED_PIN 7      // Assuming user LED is connected to PB7
#define LED_PORT GPIOB  // GPIOB port

// Global variables
volatile uint8_t reaction_triggered = 0;
volatile uint32_t start_time = 0;
volatile uint32_t end_time = 0;
volatile uint8_t random_delay_done = 0;
volatile uint8_t button_pressed = 0;
volatile float reaction_time = 0.0;
char time_str[10];

// Function prototypes
void SystemClock_Config(void);
void Error_Handler(void);
void generate_random_delay(void);
int is_button_pressed(void);
void turn_on_led(void);
void turn_off_led(void);
void setup_TIM2(void);
void display_initial_screen(void);

// Finite state machine states

int main(void) {
    HAL_Init();
    SystemClock_Config();
    SysTick_Init();            // Initialize SysTick timer
    LCD_init();                // Initialize LCD display
    setup_TIM2();              // Setup TIM2

    // Enable clock for GPIOB (LED) and GPIOC (Button)
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN;

    // Configure PB7 (LED) as output
    GPIOB->MODER &= ~(0x3 << (LED_PIN * 2));  // Clear mode bits for PB7
    GPIOB->MODER |= (0x1 << (LED_PIN * 2));   // Set PB7 as output mode

    // Configure PC13 (Button) as input
    GPIOC->MODER &= ~(0x3 << (BUTTON_PIN * 2));  // Set PC13 as input mode

    // Display initial message on LCD
    display_initial_screen();

    State state = INIT;  // Start with the INIT state

    while (1) {
        switch (state) {
            case INIT:
                if (is_button_pressed()) {
                    reaction_triggered = 1;
                    random_delay_done = 0;

                    LCD_clear();
                    LCD_write_string("Generating delay...");

                    // Generate random delay after first button press
                    generate_random_delay();

                    state = WAIT_FOR_RANDOM_DELAY;
                }
                break;

            case WAIT_FOR_RANDOM_DELAY:
                if (random_delay_done) {
                    // Random delay complete, turn on the LED
                    turn_on_led();

                    // Now, waiting for the first button press
                    LCD_clear();
                    LCD_write_string("Press button now!");

                    state = WAIT_FOR_SECOND_PRESS;
                }
                break;

            case WAIT_FOR_SECOND_PRESS:
                // Waiting for the second button press to calculate reaction time
                if (is_button_pressed()) {
                    // Second press, record the end time
                    end_time = TIM2->CNT;  // Capture end time
                    reaction_time = (end_time);  // Convert to seconds

                    // Turn off LED after the reaction is measured
                    turn_off_led();

                    // Display reaction time
                    uint32_t reaction_time = end_time;

                    // Convert reaction_time to seconds and milliseconds
                    uint32_t seconds = reaction_time / 100000;
                    uint32_t milliseconds = (reaction_time % 1000000);

                    if (seconds > 10) {
                        seconds -= 10;
                    }

                    // Format the time as "seconds.milliseconds"
                    char time_str[20];
                    sprintf(time_str, "Time: %lu.%03lu s", seconds, milliseconds);

                    // Print the formatted time string to the LCD
                    LCD_clear();
                    LCD_write_string(time_str);
                    LCD_command(0xC0);
                    LCD_write_string("Press to reset");

                    state = REACTION_COMPLETE;
                }
                break;

            case REACTION_COMPLETE:
                // Wait for user to press button to reset
                if (is_button_pressed()) {
                    display_initial_screen();
                    state = INIT;
                    TIM2->CNT = 0;
                    TIM2->SR &= ~(TIM_SR_UIF);  // Clear interrupt flag
                }
                break;
        }
    }
}

// Function to check if the button is pressed (active low) with debouncing
int is_button_pressed(void) {
    if (!(BUTTON_PORT->IDR & (1 << BUTTON_PIN))) {
        delay_us(5000);  // Debounce delay (50 microseconds)
        if (!(BUTTON_PORT->IDR & (1 << BUTTON_PIN))) {
            while (!(BUTTON_PORT->IDR & (1 << BUTTON_PIN)));  // Wait for release
            return 1;
        }
    }
    return 0;
}

// TIM2 setup
void setup_TIM2(void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;  // Enable the clock for TIM2
    TIM2->PSC = 40 - 1;                    // 80 MHz / 80 = 1 MHz (1 µs per tick)
    TIM2->ARR = 0xFFFFFFFF;                // Set ARR to maximum for long counting
    TIM2->DIER |= TIM_DIER_UIE;            // Enable update interrupt
    TIM2->SR &= ~TIM_SR_UIF;               // Clear any pending interrupt flags
    NVIC_EnableIRQ(TIM2_IRQn);             // Enable TIM2 interrupt in NVIC
    __enable_irq();                        // Global IRQ enable
}

// Function to generate a random delay using TIM2
void generate_random_delay(void) {
    int delay_in_ms = (rand() % 3000) + 2000;  // Random delay 2000-5000 ms
    TIM2->ARR = delay_in_ms * 1000;            // Convert to microseconds
    TIM2->SR &= ~TIM_SR_UIF;                   // Clear update interrupt flag
    TIM2->CR1 |= TIM_CR1_CEN;                  // Start TIM2 timer for delay
}

// Turn ON the LED (PB7)
void turn_on_led(void) {
    LED_PORT->ODR |= (1 << LED_PIN);  // Set PB7 high to turn on blue LED
}

// Turn OFF the LED (PB7)
void turn_off_led(void) {
    LED_PORT->ODR &= ~(1 << LED_PIN);  // Set PB7 low to turn off blue LED
}

// Function to display the initial prompt on the LCD
void display_initial_screen(void) {
    LCD_clear();
    LCD_write_string("EE 329 A4 REACT");
    LCD_command(0xC0);
    LCD_write_string("PUSH SW TO TRIG");
}

// System clock configuration
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Configure the main internal regulator output voltage
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }

    // Initializes the RCC Oscillators according to the specified parameters
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    // Initializes the CPU, AHB, and APB buses clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

// Error handler
void Error_Handler(void) {
    __disable_irq();
    while (1) {}
}
