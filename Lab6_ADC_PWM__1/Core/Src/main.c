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
#include "adc.h"
#include "dma.h"
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
#include "sensor.h"
#include "buzzer.h"
#include "global.h"
#include "uart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// 0 = Text Mode, 1 = Graph Mode
uint8_t display_mode = 0;
uint8_t mode_changed_flag = 1; // Trigger initial screen setup

// Graph Settings
#define GRAPH_X_START 20
#define GRAPH_Y_BASE 300
#define GRAPH_WIDTH 200
#define GRAPH_HEIGHT 100 // Increased height for better visibility
#define GRAPH_MIN_VAL 500.0f
#define GRAPH_MAX_VAL 700.0f
// Graph State
static uint16_t g_curX = 0;
static uint16_t g_prevY = GRAPH_Y_BASE;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void test_Buzzer();
void test_Adc();
void checkingCondition();
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
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_TIM13_Init();
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  lcd_Clear(BLACK);
  while (1)
  {
    while (!flag_timer2)
      ;
    flag_timer2 = 0;
    button_Scan();
    test_LedDebug();
    test_Adc();
    test_Buzzer();
    checkingCondition();
    /* USER CODE END WHILE */

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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
void system_init()
{
  timer_init();
  button_init();
  lcd_init();
  sensor_init();
  buzzer_init();
  init_RTC_timer();
  uart_init_rs232();
  setTimer2(50);
}

uint8_t count_led_debug = 0;

void test_LedDebug()
{
  count_led_debug = (count_led_debug + 1) % 20;
  if (count_led_debug == 0)
  {
    HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
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

uint8_t isButtonRight()
{
  if (button_count[11] == 1)
    return 1;
  else
    return 0;
}
// Check Button 12 for Mode Switching
uint8_t isButtonModeSwitch()
{
  if (button_count[12] == 1) // Using Index 12
    return 1;
  else
    return 0;
}

void display_TextMode(void)
{
  float voltage = sensor_GetVoltage();
  float current = sensor_GetCurrent();
  float power = (voltage * current) / 1000; // Tính Công suất P = V * I

  // Logic to display text info

  lcd_ShowStr(10, 100, "Power (Watt):", RED, BLACK, 16, 0);
  lcd_ShowFloatNum(130, 100, power, 4, RED, BLACK, 16);

  lcd_ShowStr(10, 140, "Light:", RED, BLACK, 16, 0);
  lcd_ShowIntNum(130, 140, sensor_GetLight(), 4, RED, BLACK, 16);

  lcd_ShowStr(10, 160, "Potentiometer:", RED, BLACK, 16, 0);
  lcd_ShowIntNum(130, 160, sensor_GetPotentiometer(), 4, RED, BLACK, 16);

  lcd_ShowStr(10, 180, "Potentio %:", RED, BLACK, 16, 0);
  lcd_ShowIntNum(130, 180, glb_pct_Potentiometer, 4, RED, BLACK, 16);

  lcd_ShowStr(10, 200, "Temp:", RED, BLACK, 16, 0);
  lcd_ShowFloatNum(130, 200, sensor_GetTemperature(), 4, RED, BLACK, 16);

  // Date and Time
  ds3231_ReadTime();
  char time_str[9];
  sprintf(time_str, "%02d:%02d:%02d", ds3231_hours, ds3231_min, ds3231_sec);
  lcd_ShowStr(130, 220, time_str, RED, BLACK, 16, 0);

  char date_str[11];
  sprintf(date_str, "%02d/%02d/20%02d", ds3231_date, ds3231_month, ds3231_year);
  lcd_ShowStr(130, 240, date_str, RED, BLACK, 16, 0);
}

void display_GraphMode(void)
{
  float voltage = sensor_GetVoltage();
  float current = sensor_GetCurrent();

  // 1. Draw Title
  lcd_ShowStr(10, 40, "Power Zoom (mW)", WHITE, BLACK, 16, 0);

  // --- DEBUG RAW VALUES ---
  lcd_ShowStr(10, 60, "V:", GREEN, BLACK, 16, 0);
  lcd_ShowFloatNum(40, 60, voltage, 4, GREEN, BLACK, 16);
  lcd_ShowStr(100, 60, "mA:", GREEN, BLACK, 16, 0);
  lcd_ShowFloatNum(130, 60, current, 5, GREEN, BLACK, 16);
  // ------------------------

  // 2. Calculate Power
  // Using your specific formula from the provided code
  float power_mW = (voltage * current) / 100.0f;

  // Show current Power value
  lcd_ShowStr(10, 80, "P(mW/10):", YELLOW, BLACK, 16, 0);
  lcd_ShowFloatNum(80, 80, power_mW, 5, YELLOW, BLACK, 16);

  // 3. NEW: Calculate "Zoomed" Y-Offset
  float range = GRAPH_MAX_VAL - GRAPH_MIN_VAL; // Range is 200 (700 - 500)

  // Clamp the value so it doesn't draw outside the box
  float display_val = power_mW;
  if (display_val < GRAPH_MIN_VAL)
    display_val = GRAPH_MIN_VAL;
  if (display_val > GRAPH_MAX_VAL)
    display_val = GRAPH_MAX_VAL;

  // Subtract Min so we start plotting from the bottom of our range
  float val_offset = display_val - GRAPH_MIN_VAL;

  // Scale to pixels
  uint16_t pixel_height = (uint16_t)((val_offset / range) * GRAPH_HEIGHT);

  // Invert for Screen Coordinates (Y=0 is top)
  uint16_t pointY = GRAPH_Y_BASE - pixel_height;
  uint16_t pointX = GRAPH_X_START + g_curX;

  // 4. Handle Graph Wrap-around
  if (g_curX >= GRAPH_WIDTH)
  {
    // Clear graph area
    lcd_Fill(GRAPH_X_START, GRAPH_Y_BASE - GRAPH_HEIGHT, GRAPH_X_START + GRAPH_WIDTH, GRAPH_Y_BASE + 1, BLACK);

    // Draw Frame
    lcd_DrawRectangle(GRAPH_X_START, GRAPH_Y_BASE - GRAPH_HEIGHT, GRAPH_X_START + GRAPH_WIDTH, GRAPH_Y_BASE, WHITE);

    g_curX = 0;
    pointX = GRAPH_X_START;
    g_prevY = pointY;
  }

  // 5. Draw Line
  if (g_curX > 0)
  {
    // Draw line from previous point to current point
    lcd_DrawLine(pointX - 5, g_prevY, pointX, pointY, CYAN);
  }

  g_prevY = pointY;
  g_curX += 5;
}

uint8_t count_adc = 0;
uint8_t condition_met = 0;
uint8_t buzzer_volume = 0;
uint8_t timer_buzzer = 0;

void test_Adc()
{
  // --- PART 1: Button Check (Runs every 50ms) ---
  if (isButtonModeSwitch())
  {
    display_mode = !display_mode; // Toggle Mode
    mode_changed_flag = 1;        // Set flag to clear screen

    // UX Trick: Reset the counter to 0 so the screen updates
    // immediately on the next loop, rather than waiting for the second to finish.
    count_adc = 19;

    // Reset graph X if entering graph mode
    if (display_mode == 1)
    {
      g_curX = 0;
    }
  }

  // --- PART 2: Periodic Update (Runs every 1s) ---
  count_adc = (count_adc + 1) % 20;

  if (count_adc == 0)
  {
    // Always read sensors so data is fresh
    if (sensor_Read() == 0)
      return;

    // Handle Screen Clearing if mode changed
    if (mode_changed_flag)
    {
      lcd_Clear(BLACK);
      mode_changed_flag = 0;

      // If entering Graph Mode, draw axes immediately
      if (display_mode == 1)
      {
        lcd_DrawLine(GRAPH_X_START, GRAPH_Y_BASE, GRAPH_X_START + GRAPH_WIDTH, GRAPH_Y_BASE, WHITE);
        lcd_DrawLine(GRAPH_X_START, GRAPH_Y_BASE, GRAPH_X_START, GRAPH_Y_BASE - GRAPH_HEIGHT, WHITE);
      }
    }

    // Select Display Routine
    if (display_mode == 0)
    {
      display_TextMode();
    }
    else
    {
      display_GraphMode();
    }
  }
}
char *err_msg = "Caution: Potentiometer percentage exceeded 70%\r\n";
void checkingCondition()
{
  if (glb_pct_Potentiometer >= 70)
  {
    condition_met = 1;
  }
  else
  {
    condition_met = 0;
    buzzer_SetVolume(0); // turn off buzzer
    timer_buzzer = 0;
  }
  if (condition_met)
  {
    timer_buzzer++;
    if (timer_buzzer >= 20)
    { // 1 second has passed
      timer_buzzer = 0;
      if (buzzer_volume == 0)
      {
        buzzer_volume = 50; // turn on buzzer
      }
      else
      {
        buzzer_volume = 0; // turn off buzzer
      }
      buzzer_SetVolume(buzzer_volume);
      uart_Rs232SendString((uint8_t *)err_msg);
    }
    // Do something when condition is met
    // Write caution to UART
    // HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
  }
}

void init_RTC_timer()
{
  ds3231_init();
  ds3231_Write(0x00, 0);  // second
  ds3231_Write(0x01, 50); // minute
  ds3231_Write(0x02, 8);  // hour
  ds3231_Write(0x03, 4);  // day
  ds3231_Write(0x04, 19); // date
  ds3231_Write(0x05, 11); // month
  ds3231_Write(0x06, 25); // year
}

void test_Buzzer()
{
  if (isButtonUp())
  {
    buzzer_SetVolume(50);
  }

  if (isButtonDown())
  {
    buzzer_SetVolume(0);
  }

  if (isButtonRight())
  {
    buzzer_SetVolume(25);
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

#ifdef USE_FULL_ASSERT
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
