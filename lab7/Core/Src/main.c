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
#include "touch.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define GRID_SIZE 10
#define GRID_WIDTH 24
#define GRID_HEIGHT 32
#define SNAKE_MAX_LENGTH 100
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    STATE_INIT,
    STATE_GAME,
    STATE_END
} GameState;

typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;


GameState gameState = STATE_INIT;
Direction snakeDirection = DIR_RIGHT;

Point snake[SNAKE_MAX_LENGTH];
int snakeLength = 3;
Point food;

uint8_t isGameOver = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
uint8_t isButtonClear();

void handleInitState();
void handleGameState();
void handleEndState();
void updateSnake();
void drawGame();
uint8_t isButtonTouched(int x1, int y1, int x2, int y2);
void generateFood();
void checkCollision();
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
  MX_TIM13_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 touch_Adjust();
 lcd_Clear(BLACK);
 while (1)
  {
		 touch_Scan();

		 switch (gameState) {
			 case STATE_INIT:
				 handleInitState();
				 break;
			 case STATE_GAME:
				 handleGameState();
				 break;
			 case STATE_END:
				 handleEndState();
				 break;
		 }


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
	  timer_init();
	  button_init();
	  lcd_init();
	  touch_init();
	  setTimer2(250);
}

uint8_t count_led_debug = 0;

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

uint8_t isButtonClear(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 60 && touch_GetX() < 180 && touch_GetY() > 10 && touch_GetY() < 60;
}

//void gameProcess(){
//	switch (game_Status) {
//		case INIT:
//                // display blue button
//			lcd_Fill(60, 10, 180, 60, GBLUE);
//			lcd_ShowStr(90, 20, "Start", RED, BLACK, 24, 1);
//			game_Status = START;
//			break;
//		case START:
//			if(isButtonClear()){
//				draw_Status = CLEAR;
//                    // clear board
//				lcd_Fill(0, 60, 240, 320, BLACK);
//                    // display green button
//				lcd_Fill(60, 10, 180, 60, GREEN);
//				lcd_ShowStr(90, 20, "CLEAR", RED, BLACK, 24, 1);
//			}
//			break;
//		case END:
//			if(!touch_IsTouched()) game_Status = INIT;
//			break;
//		default:
//			break;
//	}
//}

void handleInitState() {
    lcd_Clear(BLACK);
    lcd_ShowStr(60, 160, "SNAKE GAME", GREEN, BLACK, 24, 1);
    lcd_Fill(60, 10, 180, 60, BLUE);
    lcd_ShowStr(90, 20, "START", WHITE, BLUE, 24, 1);

    if (isButtonTouched(60, 10, 180, 60)) { // Start button
        snakeLength = 3;
        snake[0] = (Point){10, 10};
        snake[1] = (Point){9, 10};
        snake[2] = (Point){8, 10};
        snakeDirection = DIR_RIGHT;
        generateFood();
        isGameOver = 0;
        gameState = STATE_GAME;
        lcd_Clear(BLACK);
    }
}

/**
  * @brief Handle the GAME state.
  */
void handleGameState() {
    // Game logic
    if (flag_timer2 == 1) {
        flag_timer2 = 0;
        updateSnake();
        checkCollision();

        if (isGameOver) {
            gameState = STATE_END;
        } else {
            drawGame();
        }
    }


    if (isButtonTouched(40, 260, 80, 300)) snakeDirection = DIR_LEFT;  // Left
    if (isButtonTouched(90, 260, 130, 300)) snakeDirection = DIR_UP;    // Up
    if (isButtonTouched(140, 260, 180, 300)) snakeDirection = DIR_RIGHT; // Right
    if (isButtonTouched(190, 260, 230, 300)) snakeDirection = DIR_DOWN;  // Down
}

/**
  * @brief Handle the END state.
  */
void handleEndState() {
    lcd_Clear(BLACK);
    lcd_ShowStr(60, 160, "GAME OVER", RED, BLACK, 24, 1);
    lcd_Fill(60, 10, 180, 60, BLUE);
    lcd_ShowStr(90, 20, "RESTART", WHITE, BLUE, 24, 1);

    if (isButtonTouched(60, 10, 180, 60)) { // Restart button
        gameState = STATE_INIT;
    }
}

/**
  * @brief Update the snake's position.
  */
void updateSnake() {
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Update head position
    if (snakeDirection == DIR_UP) snake[0].y--;
    if (snakeDirection == DIR_DOWN) snake[0].y++;
    if (snakeDirection == DIR_LEFT) snake[0].x--;
    if (snakeDirection == DIR_RIGHT) snake[0].x++;
}

/**
  * @brief Check for collisions and handle game events.
  */
void checkCollision() {
    // Check collision with food
    if (snake[0].x == food.x && snake[0].y == food.y) {
        snakeLength++;
        generateFood();
    }

    // Check collision with walls
    if (snake[0].x < 0 || snake[0].x >= GRID_WIDTH || snake[0].y < 0 || snake[0].y >= GRID_HEIGHT) {
        isGameOver = 1;
    }

    // Check collision with itself
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            isGameOver = 1;
            break;
        }
    }
}

/**
  * @brief Draw the game elements on the LCD.
  */
void drawGame() {
    lcd_Clear(BLACK);

    // Draw snake
    for (int i = 0; i < snakeLength; i++) {
        lcd_Fill(snake[i].x * GRID_SIZE, snake[i].y * GRID_SIZE,
                 (snake[i].x + 1) * GRID_SIZE, (snake[i].y + 1) * GRID_SIZE, GREEN);
    }

    // Draw food
    lcd_Fill(food.x * GRID_SIZE, food.y * GRID_SIZE,
             (food.x + 1) * GRID_SIZE, (food.y + 1) * GRID_SIZE, RED);

    // Draw controls in a single row: L, U, R, D
    int buttonWidth = 40;
    int buttonHeight = 40;
    int yPosition = 260;
    int xStart = 40;
    int spacing = 10;

    // Left button
    lcd_Fill(xStart, yPosition, xStart + buttonWidth, yPosition + buttonHeight, BLUE);
    lcd_ShowStr(xStart + 10, yPosition + 10, "L", WHITE, BLUE, 24, 1);

    // Up button
    lcd_Fill(xStart + buttonWidth + spacing, yPosition, xStart + 2 * buttonWidth + spacing, yPosition + buttonHeight, BLUE);
    lcd_ShowStr(xStart + buttonWidth + spacing + 10, yPosition + 10, "U", WHITE, BLUE, 24, 1);

    // Right button
    lcd_Fill(xStart + 2 * (buttonWidth + spacing), yPosition, xStart + 3 * buttonWidth + 2 * spacing, yPosition + buttonHeight, BLUE);
    lcd_ShowStr(xStart + 2 * (buttonWidth + spacing) + 10, yPosition + 10, "R", WHITE, BLUE, 24, 1);

    // Down button
    lcd_Fill(xStart + 3 * (buttonWidth + spacing), yPosition, xStart + 4 * buttonWidth + 3 * spacing, yPosition + buttonHeight, BLUE);
    lcd_ShowStr(xStart + 3 * (buttonWidth + spacing) + 10, yPosition + 10, "D", WHITE, BLUE, 24, 1);
}

/**
  * @brief Generate a new food position.
  */
void generateFood() {
    food.x = rand() % GRID_WIDTH;
    food.y = rand() % GRID_HEIGHT;
}

/**
  * @brief Check if a button is touched.
  */
uint8_t isButtonTouched(int x1, int y1, int x2, int y2) {
    return touch_IsTouched() &&
           touch_GetX() > x1 && touch_GetX() < x2 &&
           touch_GetY() > y1 && touch_GetY() < y2;
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
