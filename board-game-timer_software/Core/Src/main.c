/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "TM1637.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
tm1637_t tm1637 = {
    .clk_port = TM1637_CLK_GPIO_Port,
    .dio_port = TM1637_DIO_GPIO_Port,
    .clk_pin  = TM1637_CLK_Pin,
    .dio_pin  = TM1637_DIO_Pin,
    .brightness = 2,
	.colon_on = 1
};

typedef enum state {
	RUNNING, PAUSED
} state_t;

state_t current_state = PAUSED;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t current_seconds, set_seconds = 0;
int16_t current_encoder_pos, last_encoder_pos = 0, encoder_difference, accum = 0;

// Flags
volatile uint8_t is_tm1637_on = 1;
volatile uint8_t update_flag = 0;
volatile uint8_t blink_flag = 0;
volatile uint8_t encoder_btn_flag = 0;
volatile uint8_t big_btn_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void set_time(int8_t steps)
{
	set_seconds += steps * 5;
	if (set_seconds < 0) set_seconds = 0;

	current_seconds = set_seconds;
	tm1637_update_time(&tm1637, set_seconds);
}
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
  MX_TIM14_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
 // encoder_init(&encoder);
  tm1637_init(&tm1637, 0);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (encoder_btn_flag) {
      encoder_btn_flag = 0;
      is_tm1637_on = 1;
	  HAL_TIM_Base_Stop_IT(&htim14);
	  if (current_state == RUNNING) {
	    current_state = PAUSED;
	    last_encoder_pos = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
	    accum = 0;

	  } else {
		HAL_TIM_Base_Start_IT(&htim14);
	  	current_state = RUNNING;
	  	tm1637_on(&tm1637, 2);
	  }
	} else if (big_btn_flag) {
	  big_btn_flag = 0;
	  HAL_TIM_Base_Stop_IT(&htim14);
	  current_seconds = set_seconds;
	  tm1637_on(&tm1637, 2);
	  tm1637_update_time(&tm1637, current_seconds);
	  HAL_TIM_Base_Start_IT(&htim14);
	  current_state = RUNNING;
	}

    if (current_state == RUNNING && update_flag) {
	  update_flag = 0;
	  if (current_seconds) current_seconds--;
	  tm1637_update_time(&tm1637, current_seconds);
	}

	if (current_state == PAUSED && blink_flag) {
	  blink_flag = 0;
	  if (is_tm1637_on == 0) {
	    is_tm1637_on = 1;
	    tm1637_update_time(&tm1637, current_seconds);
	    tm1637_on(&tm1637, 2);
	  } else {
	  	is_tm1637_on = 0;
	  	tm1637_off(&tm1637);
	  }
	}

	if (current_state == PAUSED) {
	  current_encoder_pos = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);
	  encoder_difference = current_encoder_pos - last_encoder_pos;
	  last_encoder_pos = current_encoder_pos;
	  // Check for spike, strange behavior
	  if (encoder_difference > 50 || encoder_difference < -50)
		  encoder_difference = 0;
	    accum += encoder_difference;
	    while (accum >= 2)
	    {
	        set_time(+1);   // +5 seconds
	        accum -= 2;
	    }
	    while (accum <= -2)
	    {
	    	if (current_seconds > 4) {
	        set_time(-1);   // -5 seconds
	    	}
	    	accum += 2;
	    }
	}
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{
  if (gpio_pin == ENCODER_Button_Pin) encoder_btn_flag = 1;
  if (gpio_pin == big_Button_Pin) big_btn_flag = 1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM14) update_flag = 1;
  //if (htim->Instance == TIM16) blink_flag = 1;
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
