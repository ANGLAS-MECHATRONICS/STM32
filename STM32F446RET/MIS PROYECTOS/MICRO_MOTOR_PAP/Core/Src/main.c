/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STEP_MOTOR 20   // Pasos por vuelta en modo Full Step

#define FULL_STEP  1    // Full step      MS1=0 MS2=0 MS3=0   → STEP_MOTOR * 1   = 20 pasos/vuelta
#define HALF_STEP  2    // Half step      MS1=1 MS2=0 MS3=0   → STEP_MOTOR * 2   = 40 pasos/vuelta
#define QUAR_STEP  4    // Quarter step   MS1=0 MS2=1 MS3=0   → STEP_MOTOR * 4   = 80 pasos/vuelta
#define EIGH_STEP  8    // Eighth step    MS1=1 MS2=1 MS3=0   → STEP_MOTOR * 8   = 160 pasos/vuelta
#define SIXT_STEP 16    // Sixteenth step MS1=1 MS2=1 MS3=1   → STEP_MOTOR * 16  = 320 pasos/vuelta

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void delay_us(uint32_t time);// funciona desde 2us en adelante lom probe hasta 5 segundos (5000000)
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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t N_VUELTAS=30;
  uint32_t tiempo_ms=1;
  uint32_t tiempo_us=750;

  while (1)
  {
	  //El motor necesita 20 pasos para una vuelta en full step, en 1/16=320pasos, 1/8 = 160
	  HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, RESET);//habilito el motor
	  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, SET);//sentido horario
	  for(int i = 0; i < STEP_MOTOR * FULL_STEP * N_VUELTAS; i++){
		  HAL_GPIO_WritePin(STEP_GPIO_Port, STEP_Pin, SET);
		  //HAL_Delay(tiempo_ms);
		  delay_us(tiempo_us);
		  HAL_GPIO_WritePin(STEP_GPIO_Port, STEP_Pin, RESET);
		  //HAL_Delay(tiempo_ms);
		  delay_us(tiempo_us);
	  }
	  HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, SET);//deshabilito el motor
	  HAL_Delay(2000);



	  HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, RESET);//habilito el motor
	  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, RESET);//sentido antihorario
	  for(int i = 0; i < STEP_MOTOR * FULL_STEP * N_VUELTAS; i++){
		  HAL_GPIO_WritePin(STEP_GPIO_Port, STEP_Pin, SET);
		  //HAL_Delay(tiempo_ms);
		  delay_us(tiempo_us);
		  HAL_GPIO_WritePin(STEP_GPIO_Port, STEP_Pin, RESET);
		  //HAL_Delay(tiempo_ms);
		  delay_us(tiempo_us);
	  }
	  HAL_GPIO_WritePin(EN_GPIO_Port, EN_Pin, SET);//deshabilito el motor
	  HAL_Delay(2000);



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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 10;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, EN_Pin|STEP_Pin|DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : EN_Pin STEP_Pin DIR_Pin */
  GPIO_InitStruct.Pin = EN_Pin|STEP_Pin|DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void delay_us(uint32_t time){//esta funcion funciona mejor cambie uint16_t  a uint32_t y funciono mejor
	uint32_t delay = time *(HAL_RCC_GetHCLKFreq() / 1000000);
	for(int  i = 0; i < delay; i+=14){
		__ASM("NOP");
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
