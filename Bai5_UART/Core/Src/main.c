/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "uart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DISPLAY_MODE 0
#define ADJUST_MODE 1

#define ALARM_MODE 2
#define ADJUST_RS232_Mode 3
#define ADJUST_RS232_Mode_ERROR 4

uint8_t counter_blink=0;
uint8_t blink_flag =0;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t ring_buffer[RING_BUFFER_SIZE];
uint16_t head = 0; // Points to the next write position
uint16_t tail = 0; // Points to the current read position
uint8_t receive_buffer = 0;
uint8_t data_available_flag = 0;


uint8_t count_led_debug = 0;

uint8_t current_mode = DISPLAY_MODE;
uint8_t adjust_part = 0;
uint8_t alarm_hours = 0;
uint8_t alarm_minutes = 0;

uint8_t alarm_flag = 0;

uint8_t request_RS232_count = 0;

// Constants and global variables
#define MAX_ATTEMPTS 3
#define TIMEOUT 100        // Timeout for 10 seconds (assuming a 50ms increment per tick)
#define ERROR_DISPLAY_TIME 60 // 3 seconds (assuming a 50ms increment per tick)

uint8_t attempt_count = 0;
uint16_t timeout_counter = TIMEOUT-1;
uint8_t temp_value = 0;
uint8_t digit_count = 0;
uint8_t request_sent = 0;
uint8_t error_display_counter = 0;
uint8_t in_error_display = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void buttonBai5();
void test_Uart();

void displayTime();
void updateTime();

void adjustTimeRS232(void);
void adjustTime();
void setAlarm();
void checkAlarm();
void ProcessRingBufferData();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
      /* USER CODE END WHILE */
//      if (data_available_flag) {
//          data_available_flag = 0;  // Clear the flag
//
//          // Process all data in the ring buffer
//          while (tail != head) {
//              uint8_t data = ring_buffer[tail];
//              tail = (tail + 1) % RING_BUFFER_SIZE;
//
//              // Process the data (e.g., echo it back or handle it in some other way)
//              HAL_UART_Transmit(&huart1, &data, 1, 10);
//          }
//      }

    	// Inside the main loop



      while (!flag_timer2);
      flag_timer2 = 0;
      button_Scan();
      buttonBai5();
  	if (data_available_flag) {
  	    ProcessRingBufferData();  // Process all available data in the ring buffer
  	    data_available_flag = 0;   // Reset the flag after processing
  	}
      switch (current_mode) {
          case DISPLAY_MODE:
        	  lcd_StrCenter(0, 2, "DISPLAY MODE", WHITE, BLACK, 16, 1);
              ds3231_ReadTime();
              displayTime();
              checkAlarm();
              adjust_part =0;
              break;

          case ADJUST_MODE:
        	  lcd_StrCenter(0, 2, "ADJUST MANUALLY Mode", WHITE, BLACK, 16, 1);
              adjustTime();
              break;

          case ALARM_MODE:
        	  lcd_StrCenter(0, 2, "ALARM Mode", WHITE, BLACK, 16, 1);
              setAlarm();
              adjust_part =0;
              break;
          case ADJUST_RS232_Mode:
        	  lcd_StrCenter(0, 2, "ADJUST RS232 Mode", WHITE, BLACK, 16, 1);
//        	  lcd_ShowStr(20, 30, "Traffic light", WHITE, Black, 24, 0);
        	  adjustTimeRS232();
        	  break;
          case ADJUST_RS232_Mode_ERROR:
        	    lcd_StrCenter(0, 2, "ERROR: No response", RED, BLACK, 16, 1);
      	        error_display_counter++;
      	        if (error_display_counter >= ERROR_DISPLAY_TIME) {
      	            current_mode = DISPLAY_MODE;  // Reset to normal mode
      	            lcd_Clear(BLACK);
      	            in_error_display = 0;
      	            error_display_counter = 0;
      	          attempt_count = 0;
      	          timeout_counter = TIMEOUT;
      	        }
        	  break;
          default:
              break;
      }
      /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */


}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	  HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
	  timer_init();
	  led7_init();
	  button_init();
	  lcd_init();
	  uart_init_rs232();
	  ds3231_init();
	  setTimer2(50);
	  lcd_Clear(BLACK);
	  updateTime();
}



void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

void test_button(){
	for(int i = 0; i < 16; i++){
		if(button_count[i] == 1){
			led7_SetDigit(i/10, 2, 0);
			led7_SetDigit(i%10, 3, 0);
		}
	}
}

void test_Uart(){
	if(button_count[12] == 1){
		uart_Rs232SendNum(ds3231_hours);
		uart_Rs232SendString(":");
		uart_Rs232SendNum(ds3231_min);
		uart_Rs232SendString(":");
		uart_Rs232SendNum(ds3231_sec);
		uart_Rs232SendString("\n");
	}
}
void updateTime(){
	ds3231_Write(ADDRESS_YEAR, 23);
	ds3231_Write(ADDRESS_MONTH, 10);
	ds3231_Write(ADDRESS_DATE, 20);
	ds3231_Write(ADDRESS_DAY, 6);
	ds3231_Write(ADDRESS_HOUR, 1);
	ds3231_Write(ADDRESS_MIN, 2);
	ds3231_Write(ADDRESS_SEC, 30);
}
void buttonBai5(){
    if (button_count[0] == 1) {
        current_mode = (current_mode +1) % 4;
       lcd_Clear(BLACK);
       request_RS232_count =0;
    }
}

uint8_t isButtonUp()
{
    if (button_count[3] == 1)
        return 1;
    else
        return 0;
}
uint8_t isButtonDown()
{
    if (button_count[7] == 1)
        return 1;
    else
        return 0;
}
void displayTime(){

	lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(20, 130, ds3231_day, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(110, 130, ds3231_month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(150, 130, ds3231_year, 2, YELLOW, BLACK, 24);
}

void adjustTime() {
//    lcd_ShowStr(10, 10, "Adjust Time", RED, BLACK, 24);
	counter_blink = (counter_blink + 1)%10;
    // Increment the selected part of time
    if (isButtonUp()) {
        if (adjust_part == 0){
        	ds3231_hours = (ds3231_hours + 1) % 24;

        }
        else if (adjust_part == 1) ds3231_min = (ds3231_min + 1) % 60;
        else if (adjust_part == 2) ds3231_sec = (ds3231_sec + 1) % 60;
    }

    // Save part and move to the next part
    if (isButtonDown()) {
        adjust_part = (adjust_part + 1) % 3;  // Rotate through hours, minutes, seconds

        if (adjust_part == 0) {
        	ds3231_Write(ADDRESS_HOUR, ds3231_hours);
			ds3231_Write(ADDRESS_MIN, ds3231_min);
			ds3231_Write(ADDRESS_SEC, ds3231_sec);
            current_mode = DISPLAY_MODE;
            lcd_Clear(BLACK);
        }
    }

    // Display and blink selected part

    if (adjust_part == 0) {
    	if(counter_blink ==0 &&blink_flag ==0)
    	{
    		displayTime();
    		lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
    		blink_flag =1;
    	}
    	else if (counter_blink ==0 &&blink_flag ==1){
    		displayTime();
    		lcd_ShowIntNum(70, 100, ds3231_hours, 2, BLACK, BLACK, 24);
    		blink_flag =0;
    	}
    }
    else if (adjust_part == 1) {
    	if(counter_blink ==0 &&blink_flag ==0)
    	{
    		displayTime();
    		lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
    		blink_flag =1;
    	}

    	else if (counter_blink ==0 &&blink_flag ==1){
    		displayTime();
    		lcd_ShowIntNum(110, 100, ds3231_min, 2, BLACK, BLACK, 24);
    		blink_flag =0;
    	}
    }

    else if (adjust_part == 2) {
    	if(counter_blink ==0 &&blink_flag ==0)
    	{
    		displayTime();
    		lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
    		blink_flag =1;
    	}
    	else if(counter_blink ==0 &&blink_flag ==1){
    		displayTime();
    		lcd_ShowIntNum(150, 100, ds3231_sec, 2, BLACK, BLACK, 24);
    		blink_flag =0;
    	}
    }
}
static uint8_t alarm_active = 2;  // Flag to indicate if the alarm is active


void setAlarm() {
//    lcd_ShowStr(10, 10, "Set Alarm", YELLOW, BLACK, 24);

    // Adjust alarm time
    if (isButtonUp()) {
        if (adjust_part == 0) alarm_hours = (alarm_hours + 1) % 24;
        else if (adjust_part == 1) alarm_minutes = (alarm_minutes + 1) % 60;


    }

    if (isButtonDown()) {
        adjust_part = (adjust_part + 1) % 2;  // Move to next part

        if (adjust_part == 0) {
            current_mode = DISPLAY_MODE; // Return to view mode
            lcd_Clear(BLACK);
            alarm_active = 1;
        }
    }

    // Display alarm time
    lcd_ShowIntNum(70, 100, alarm_hours, 2, YELLOW, BLACK, 24);
    lcd_ShowIntNum(110, 100, alarm_minutes, 2, YELLOW, BLACK, 24);

}
uint8_t flash_counter = 0;
void checkAlarm() {
    ds3231_ReadTime();

    // Check if it's time for the alarm to trigger
    if (ds3231_hours == alarm_hours && ds3231_min == alarm_minutes) {
        flash_counter++;  // Increment the flash counter every time checkAlarm is called

        // Flash red screen every 5 seconds
        if (flash_counter >= 20) {
            lcd_Fill(0, 0, 240, 320, RED);  // Flash red screen
            flash_counter = 0;  // Reset the counter after flashing
            if(flash_counter == 0){
            	lcd_Clear(BLACK);
            }
        }
    } else {
        flash_counter = 0;  // Reset counter if not in alarm time
    }
}

void resendRequest() {
    switch (adjust_part) {
        case 0: HAL_UART_Transmit(&huart1, (uint8_t *)"Request Hour\n", 13, 10); break;
        case 1: HAL_UART_Transmit(&huart1, (uint8_t *)"Request Min\n", 12, 10); break;
        case 2: HAL_UART_Transmit(&huart1, (uint8_t *)"Request Sec\n", 12, 10); break;
    }
}




void adjustTimeRS232(void){
	counter_blink = (counter_blink + 1)%10;
	timeout_counter = (timeout_counter + 1)%TIMEOUT;



	    if (isButtonDown()) {
	        adjust_part = (adjust_part + 1) % 3;  // Rotate through hours, minutes, seconds

	        if (adjust_part == 0) {
	        	ds3231_Write(ADDRESS_HOUR, ds3231_hours);
				ds3231_Write(ADDRESS_MIN, ds3231_min);
				ds3231_Write(ADDRESS_SEC, ds3231_sec);
	            current_mode = DISPLAY_MODE;
	            lcd_Clear(BLACK);
	        }
	    }

	    if (data_available_flag) {
	        data_available_flag = 0;
	        ProcessRingBufferData();
	    }

	    if (timeout_counter ==0) {  // 5 seconds elapsed
	        if (attempt_count < MAX_ATTEMPTS) {
	            resendRequest();
	            timeout_counter = 0;  // Reset counter after resend
	            attempt_count++;

	        }
	        else{
	            current_mode = ADJUST_RS232_Mode_ERROR;
	            lcd_Clear(BLACK);
	            error_display_counter = 0;
	        }

	    }

	    //Blink with 0.5 second
	    if (adjust_part == 0) {
	    	if(counter_blink ==0 &&blink_flag ==0)
	    	{
	    		displayTime();
	    		lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
	    		blink_flag =1;
	    	}
	    	else if (counter_blink ==0 &&blink_flag ==1){
	    		displayTime();
	    		lcd_ShowIntNum(70, 100, ds3231_hours, 2, BLACK, BLACK, 24);
	    		blink_flag =0;
	    	}
	    }
	    else if (adjust_part == 1) {
	    	if(counter_blink ==0 &&blink_flag ==0)
	    	{
	    		displayTime();
	    		lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
	    		blink_flag =1;
	    	}

	    	else if (counter_blink ==0 &&blink_flag ==1){
	    		displayTime();
	    		lcd_ShowIntNum(110, 100, ds3231_min, 2, BLACK, BLACK, 24);
	    		blink_flag =0;
	    	}
	    }

	    else if (adjust_part == 2) {
	    	if(counter_blink ==0 &&blink_flag ==0)
	    	{
	    		displayTime();
	    		lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
	    		blink_flag =1;
	    	}
	    	else if(counter_blink ==0 &&blink_flag ==1){
	    		displayTime();
	    		lcd_ShowIntNum(150, 100, ds3231_sec, 2, BLACK, BLACK, 24);
	    		blink_flag =0;
	    	}
	    }
}
void ProcessRingBufferData() {
    while (tail != head) {
        uint8_t data = ring_buffer[tail];
        tail = (tail + 1) % RING_BUFFER_SIZE;

        if (data >= '0' && data <= '9') {
            temp_value = temp_value * 10 + (data - '0');
            digit_count++;

            if (digit_count == 2 || (adjust_part == 0 && temp_value > 2)) {
                switch (adjust_part) {
                    case 0:
                        ds3231_hours = temp_value % 24;
                        HAL_UART_Transmit(&huart1, (uint8_t *)"Hour Set\n", 9, 10);
                        break;
                    case 1:
                        ds3231_min = temp_value % 60;
                        HAL_UART_Transmit(&huart1, (uint8_t *)"Minute Set\n", 11, 10);
                        break;
                    case 2:
                        ds3231_sec = temp_value % 60;
                        HAL_UART_Transmit(&huart1, (uint8_t *)"Second Set\n", 11, 10);
                        break;
                }

                // Reset temporary variables on valid data
                attempt_count = 0;
                timeout_counter = 0;
                request_sent = 0;
                temp_value = 0;
                digit_count = 0;

                // Move to the next part
                adjust_part = (adjust_part + 1) % 3;

                // If finished adjusting all parts, save and exit
                if (adjust_part == 0) {
                    ds3231_Write(ADDRESS_HOUR, ds3231_hours);
                    ds3231_Write(ADDRESS_MIN, ds3231_min);
                    ds3231_Write(ADDRESS_SEC, ds3231_sec);
                    current_mode = DISPLAY_MODE;
                    lcd_Clear(BLACK);
                    HAL_UART_Transmit(&huart1, (uint8_t *)"Time Adjusted\n", 14, 10);
                }
            }
        }
    }
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
