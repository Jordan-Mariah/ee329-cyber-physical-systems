/* USER CODE BEGIN Header */
/*******************************************************************************
 * ADC AND COIL CONTROL WITH UART OUTPUT
 *******************************************************************************
 * @file           : main.c
 * @brief          : ADC sampling and coil control with UART data display
 * project         : EE 329 Current Measurement Project
 * authors         : Kalon Bienz, Jordan Reichhardt
 * version         : 1.2
 * date            : 171124
 * compiler        : STM32CubeIDE
 * target          : NUCLEO-L4A6ZG
 * clocks          : 2 MHz MSI
 *******************************************************************************
 * OPERATION:
 * - Samples ADC when button is pressed
 * - Controls coil via GPIO output
 * - Displays voltage and current measurements via UART
 * - Uses interrupts for ADC sampling
 *******************************************************************************
 * WIRING:
 * - PC13: User Button (Input, Pull-down)
 * - PB6: Coil Drive (Output)
 * - PA0: ADC Input
 * - PG7/PG8: LPUART TX/RX
 *******************************************************************************
 * REVISION HISTORY
 * 1.0 171124 KB/JR  Initial version
 * 1.1 171124 KB/JR  Added current measurement
 * 1.2 171124 KB/JR  Added UART output formatting
 ******************************************************************************/
/* USER CODE END Header */

#include "main.h"

/* Global Variables */
volatile uint8_t  eoc_flag = 0;
volatile uint8_t  sample_index = 0;
volatile uint16_t adc_samples[SAMPLE_SIZE];
volatile uint16_t min_val, max_val, avg_val;
volatile float    min_current, max_current, avg_current;

/* -----------------------------------------------------------------------------
 * function : main
 * purpose  : Main program loop controlling ADC sampling and coil operation
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.0
 * date     : 171124
 * -------------------------------------------------------------------------- */
int main(void) {
   HAL_Init();
   __HAL_RCC_SYSCFG_CLK_ENABLE();
   SystemClock_Config();
   SysTick_Init();
   ADC_Init();
   UART_Init();
   Setup_Button();
   Setup_Coil();

   while (1) {
      if (GPIOC->IDR & GPIO_PIN_13) {              // Button pressed
         Delay_us(10000);
         if (GPIOC->IDR & GPIO_PIN_13) {
            GPIOB->ODR |= (GPIO_PIN_6);            // Turn on coil
         }
      } else {
         GPIOB->ODR &= ~(GPIO_PIN_6);              // Turn off coil
      }

      if (eoc_flag) {
         eoc_flag = 0;
         Process_Samples();
         Display_Results();
         Delay_us(300000);
      } else {
         ADC1->CR |= ADC_CR_ADSTART;               // Start ADC conversion
      }
   }
}

/* -----------------------------------------------------------------------------
 * function : UART_Init
 * purpose  : Initialize LPUART for data output
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void UART_Init(void) {
   PWR->CR2 |= (PWR_CR2_IOSV);                    // Power for PG[15:2]
   RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOGEN);         // Enable GPIOG clock
   RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;       // Enable LPUART clock

   /* GPIO Configuration */
   GPIOG->MODER &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8);
   GPIOG->MODER |= (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1);
   GPIOG->OTYPER &= ~(GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8);
   GPIOG->PUPDR &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD8);
   GPIOG->OSPEEDR |= ((3 << GPIO_OSPEEDR_OSPEED7_Pos) |
                      (3 << GPIO_OSPEEDR_OSPEED8_Pos));

   /* Alternate Function Configuration */
   GPIOG->AFR[0] &= ~(0x000F << GPIO_AFRL_AFSEL7_Pos);
   GPIOG->AFR[0] |= (0x0008 << GPIO_AFRL_AFSEL7_Pos);
   GPIOG->AFR[1] &= ~(0x000F << GPIO_AFRH_AFSEL8_Pos);
   GPIOG->AFR[1] |= (0x0008 << GPIO_AFRH_AFSEL8_Pos);

   /* LPUART Configuration */
   LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0);
   LPUART1->CR1 |= USART_CR1_UE;
   LPUART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);
   LPUART1->CR1 |= USART_CR1_RXNEIE;
   LPUART1->ISR &= ~(USART_ISR_RXNE);
   LPUART1->BRR = BAUD_RATE;

   NVIC->ISER[2] = (1 << (LPUART1_IRQn & 0x1F));
   __enable_irq();
}

/* -----------------------------------------------------------------------------
 * function : LPUART_Print
 * purpose  : Send string via LPUART
 * inputs   : message - String to send
 * outputs  : None
 * authors  : Jordan Reichhardt, Kalon Bienz
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void LPUART_Print(const char* message) {
   uint16_t iStrIdx = 0;
   while (message[iStrIdx] != 0) {
      while(!(LPUART1->ISR & USART_ISR_TXE));     // Wait for empty buffer
      LPUART1->TDR = message[iStrIdx];            // Send character
      iStrIdx++;
   }
}

/* -----------------------------------------------------------------------------
 * function : Display_Results
 * purpose  : Format and display ADC results via UART
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void Display_Results(void) {
   char buffer[50];

   LPUART_Print("\rADC counts    volts\n");

   /* Display minimum values */
   itoa(min_val, buffer, 10);
   LPUART_Print("\rMIN  ");
   LPUART_Print(buffer);
   LPUART_Print("    ");
   Convert_ADC_To_Voltage(min_val, buffer);
   LPUART_Print(buffer);
   LPUART_Print(" V\n");

   /* Display maximum values */
   itoa(max_val, buffer, 10);
   LPUART_Print("\rMAX  ");
   LPUART_Print(buffer);
   LPUART_Print("    ");
   Convert_ADC_To_Voltage(max_val, buffer);
   LPUART_Print(buffer);
   LPUART_Print(" V\n");

   /* Display average values */
   itoa(avg_val, buffer, 10);
   LPUART_Print("\rAVG  ");
   LPUART_Print(buffer);
   LPUART_Print("    ");
   Convert_ADC_To_Voltage(avg_val, buffer);
   LPUART_Print(buffer);
   LPUART_Print(" V\n\n");

   /* Display current measurements */
   LPUART_Print("\rCoil Current Measurements\n");

   LPUART_Print("\rMIN: ");
   Convert_To_Current_String(min_current, buffer);
   LPUART_Print(buffer);
   LPUART_Print(" Amps\n");

   LPUART_Print("\rMAX: ");
   Convert_To_Current_String(max_current, buffer);
   LPUART_Print(buffer);
   LPUART_Print(" Amps\n");

   LPUART_Print("\rAVG: ");
   Convert_To_Current_String(avg_current, buffer);
   LPUART_Print(buffer);
   LPUART_Print(" Amps\n\n");
}

/* -----------------------------------------------------------------------------
 * function : ADC_Init
 * purpose  : Initialize ADC with interrupts
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void ADC_Init(void) {
   /* Enable GPIOA clock */
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

   /* Configure PA0 in analog mode */
   GPIOA->MODER |= GPIO_MODER_MODE0;
   GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;

   /* ADC Configuration */
   RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
   ADC123_COMMON->CCR |= (1 << ADC_CCR_CKMODE_Pos);
   ADC1->CR &= ~ADC_CR_DEEPPWD;
   ADC1->CR |= ADC_CR_ADVREGEN;
   Delay_us(20);

   ADC1->DIFSEL &= ~ADC_DIFSEL_DIFSEL_5;
   ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF);
   ADC1->CR |= ADC_CR_ADCAL;
   while (ADC1->CR & ADC_CR_ADCAL);

   ADC1->ISR |= ADC_ISR_ADRDY;
   ADC1->CR |= ADC_CR_ADEN;
   while(!(ADC1->ISR & ADC_ISR_ADRDY));

   ADC1->SQR1 |= (5 << ADC_SQR1_SQ1_Pos);
   ADC1->SMPR1 = (7 << ADC_SMPR1_SMP5_Pos);      // Set 640.5 cycles

   ADC1->CFGR &= ~(ADC_CFGR_CONT | ADC_CFGR_EXTEN | ADC_CFGR_RES);

   /* Enable ADC interrupt */
   ADC1->IER |= ADC_IER_EOCIE;
   NVIC->ISER[0] = (1 << (ADC1_2_IRQn & 0x1F));
   __enable_irq();
}

/* -----------------------------------------------------------------------------
 * function : ADC1_2_IRQHandler
 * purpose  : ADC interrupt handler for sample collection
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.1
 * date     : 171124
 * -------------------------------------------------------------------------- */
void ADC1_2_IRQHandler(void) {
   if (ADC1->ISR & ADC_ISR_EOC) {
      adc_samples[sample_index++] = ADC1->DR;
      if (sample_index >= SAMPLE_SIZE) {
         sample_index = 0;
         eoc_flag = 1;
      }
      ADC1->ISR |= ADC_ISR_EOC;                   // Clear EOC flag
   }
}

/* -----------------------------------------------------------------------------
 * function : Process_Samples
 * purpose  : Calculate min, max, and average values from ADC samples
 * inputs   : None
 * outputs  : None
 * authors  : Jordan Reichhardt, Kalon Bienz
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void Process_Samples(void) {
   uint32_t sum = 0;
   min_val = 0xFFFF;
   max_val = 0;

   for (int i = 0; i < SAMPLE_SIZE; i++) {
      uint16_t val = adc_samples[i];
      if (val < min_val) min_val = val;
      if (val > max_val) max_val = val;
      sum += val;
   }
   avg_val = sum / SAMPLE_SIZE;

   /* Convert to current using I = V/R */
   min_current = (min_val * V_REF / ADC_MAX_VALUE) / R_SENSE;
   max_current = (max_val * V_REF / ADC_MAX_VALUE) / R_SENSE;
   avg_current = (avg_val * V_REF / ADC_MAX_VALUE) / R_SENSE;
}

/* -----------------------------------------------------------------------------
 * function : Convert_ADC_To_Voltage
 * purpose  : Convert ADC value to voltage string
 * inputs   : adc_val - ADC value to convert
 *            buffer - String buffer for result
 * outputs  : None
 * authors  : Jordan Reichhardt, Kalon Bienz
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void Convert_ADC_To_Voltage(uint16_t adc_val, char* buffer) {
   uint32_t voltage_mv = (3300 * adc_val) / ADC_MAX_VALUE;
   buffer[0] = '0' + (voltage_mv / 1000) % 10;
   buffer[1] = '.';
   buffer[2] = '0' + (voltage_mv / 100) % 10;
   buffer[3] = '0' + (voltage_mv / 10) % 10;
   buffer[4] = '0' + (voltage_mv % 10);
   buffer[5] = '\0';
}

/* -----------------------------------------------------------------------------
 * function : SysTick_Init
 * purpose  : Initialize SysTick timer for microsecond delays
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void SysTick_Init(void) {
   SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk |    // Enable SysTick Timer
                     SysTick_CTRL_CLKSOURCE_Msk);  // Select CPU clock
   SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk);  // Disable interrupt
}

/* -----------------------------------------------------------------------------
 * function : Delay_us
 * purpose  : Microsecond delay using SysTick timer
 * inputs   : time_us - Delay time in microseconds
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void Delay_us(const uint32_t time_us) {
   SysTick->LOAD = (uint32_t)((time_us * (SystemCoreClock / 1000000)) - 1);
   SysTick->VAL = 0;
   SysTick->CTRL &= ~(SysTick_CTRL_COUNTFLAG_Msk);
   while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
}

/* -----------------------------------------------------------------------------
 * function : SystemClock_Config
 * purpose  : Configure system clock to 2 MHz using MSI
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void SystemClock_Config(void) {
   RCC_OscInitTypeDef RCC_OscInitStruct = {0};
   RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

   if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
      Error_Handler();
   }

   /* Configure MSI to 2 MHz */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
   RCC_OscInitStruct.MSIState = RCC_MSI_ON;
   RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
   RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
      Error_Handler();
   }

   /* Configure system clock */
   RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                 RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
      Error_Handler();
   }
}

/* -----------------------------------------------------------------------------
 * function : Setup_Coil
 * purpose  : Configure GPIO for coil control output
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.1
 * date     : 171124
 * -------------------------------------------------------------------------- */
void Setup_Coil(void) {
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;          // Enable GPIOB clock

   GPIOB->MODER &= ~(GPIO_MODER_MODE6);           // Clear mode bits
   GPIOB->MODER |= (GPIO_MODER_MODE6_0);          // Set to output
   GPIOB->OTYPER &= ~(GPIO_OTYPER_OT6);          // Push-pull
   GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6);          // No pull-up/down
   GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6;       // High speed
}

/* -----------------------------------------------------------------------------
 * function : Setup_Button
 * purpose  : Configure GPIO for button input
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.1
 * date     : 171124
 * -------------------------------------------------------------------------- */
void Setup_Button(void) {
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;          // Enable GPIOC clock

   GPIOC->BRR = GPIO_PIN_13;                      // Reset pin PC13
   GPIOC->MODER &= ~(GPIO_MODER_MODE13);          // Set to input
   GPIOC->OTYPER &= ~(GPIO_OTYPER_OT13);          // Push-pull
   GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13);          // Clear PUPDR
   GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_1;          // Set pull-down
}

/* -----------------------------------------------------------------------------
 * function : Error_Handler
 * purpose  : Handle system errors
 * inputs   : None
 * outputs  : None
 * authors  : Kalon Bienz, Jordan Reichhardt
 * version  : 1.0
 * date     : 171124
 * -------------------------------------------------------------------------- */
void Error_Handler(void) {
   __disable_irq();
   while (1) {}
}

/* -----------------------------------------------------------------------------
 * function : Convert_To_Current_String
 * purpose  : Convert current value to string
 * inputs   : current - Current value to convert
 *            buffer - String buffer for result
 * outputs  : None
 * authors  : Jordan Reichhardt, Kalon Bienz
 * version  : 1.2
 * date     : 171124
 * -------------------------------------------------------------------------- */
void Convert_To_Current_String(float current, char* buffer) {
   int current_ma = (int)(current * 1000);
   buffer[0] = '0' + (current_ma / 1000) % 10;
   buffer[1] = '.';
   buffer[2] = '0' + (current_ma / 100) % 10;
   buffer[3] = '0' + (current_ma / 10) % 10;
   buffer[4] = '0' + current_ma % 10;
   buffer[5] = '\0';
}
