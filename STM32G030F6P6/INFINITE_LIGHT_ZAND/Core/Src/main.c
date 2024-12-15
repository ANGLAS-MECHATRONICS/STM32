/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;

/* USER CODE BEGIN PV */
uint16_t duty=0;
uint16_t maxDutyRGB=0;
uint16_t maxDutyLED=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM14_Init(void);
/* USER CODE BEGIN PFP */
void led_B(uint16_t duty);
void led_R(uint16_t duty);
void led_G(uint16_t duty);
void led_L(uint16_t duty);
void fade_R(uint16_t time);
void fade_B(uint16_t time);
void fade_G(uint16_t time);
void fade_L(uint16_t time);

void Fade_Colors(uint8_t R, uint8_t G, uint8_t B,  uint16_t time);
void Rotate_RGB(uint16_t duty, uint16_t veces, uint16_t time);
void Rotate_All(uint16_t duty, uint16_t veces, uint16_t time, uint16_t heart);
void Fade_All(uint16_t time);
void Fade_TwoColor(uint8_t color, uint16_t time);
void Fade_All_TwoColor(uint16_t time);
void Heart(uint8_t veces, uint16_t tiempo);
void Rojos(uint16_t time);
void Rojos_Inverted(uint16_t time);


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
  MX_TIM3_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_Base_Start(&htim14);

  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  maxDutyRGB = htim3.Instance->ARR;

  HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);
  maxDutyLED = htim14.Instance->ARR;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

	  Fade_All_TwoColor(1);//25seg
	  for(uint16_t i=5 ; i<=25 ; i+=1){Rotate_RGB(999, 8, i);}//13seg
	  for(uint16_t i=25; i<=50 ; i+=1){Rotate_RGB(999, 2, i);}//6seg
	  Heart(3,2);
	  Fade_All(1);//25seg
	  for(uint16_t i=50; i<=100; i+=1){Rotate_RGB(999, 1, i);}//12seg
	  Rojos_Inverted(3);
	  Rojos(3);
	  for(uint16_t i=400; i<=1200; i+=400){Rotate_All(999, 1, i,250);}//20seg*/


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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 8-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 8-1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 1000-1;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */
  HAL_TIM_MspPostInit(&htim14);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void led_B(uint16_t duty){
	htim3.Instance->CCR1 = duty;
}

void led_R(uint16_t duty){
	htim3.Instance->CCR2 = duty;
}

void led_G(uint16_t duty){
	htim3.Instance->CCR3 = duty;
}

void led_L(uint16_t duty){
	htim14.Instance->CCR1 = duty;
}

void fade_R(uint16_t time){
	for(uint16_t i = 0; i < maxDutyRGB; i++){led_R(i);HAL_Delay(time);}
    for(uint16_t i = maxDutyRGB; i > 0; i--){led_R(i);HAL_Delay(time);}
}

void fade_B(uint16_t time){
	for(uint16_t i = 0; i < maxDutyRGB; i++){led_B(i);HAL_Delay(time);}
    for(uint16_t i = maxDutyRGB; i > 0; i--){led_B(i);HAL_Delay(time);}
}

void fade_G(uint16_t time){
	for(uint16_t i = 0; i < maxDutyRGB; i++){led_G(i);HAL_Delay(time);}
    for(uint16_t i = maxDutyRGB; i > 0; i--){led_G(i);HAL_Delay(time);}
}

void fade_L(uint16_t time){
	for(uint16_t i = 0; i < maxDutyLED; i++){led_L(i);HAL_Delay(time);}
    for(uint16_t i = maxDutyLED; i > 0; i--){led_L(i);HAL_Delay(time);}
}

void Fade_Colors(uint8_t R, uint8_t G, uint8_t B,  uint16_t time){
	for(uint16_t i = 0; i < maxDutyRGB; i++){
		if(R) led_R(i);
		if(G) led_G(i);
		if(B) led_B(i);
		HAL_Delay(time);
	}
    for(uint16_t i = maxDutyRGB; i > 0; i--){
		if(R) led_R(i);
		if(G) led_G(i);
		if(B) led_B(i);
		HAL_Delay(time);
    }
    led_R(0);led_B(0);led_G(0);//apago todo
}

void Rotate_RGB(uint16_t duty, uint16_t veces, uint16_t time){
	 for(uint16_t i = 0; i < veces; i++){
		led_R(duty);led_B(0)   ;led_G(0)   ;HAL_Delay(time);
		led_R(0)   ;led_B(duty);led_G(0)   ;HAL_Delay(time);
		led_R(0)   ;led_B(0)   ;led_G(duty);HAL_Delay(time);
	 }
	 led_R(0);led_B(0);led_G(0);//apago todo

}

void Rotate_All(uint16_t duty, uint16_t veces, uint16_t time, uint16_t heart){
	 for(uint16_t i = 0; i < veces; i++){
		led_R(duty);led_B(0)   ;led_G(0)   ; led_L(heart);HAL_Delay(time);//Rojo
		led_R(0)   ;led_B(duty);led_G(0)   ; led_L(0)    ;HAL_Delay(time);//Azul
		led_R(0)   ;led_B(0)   ;led_G(duty); led_L(0)    ;HAL_Delay(time);//Verde
		led_R(duty);led_B(duty);led_G(0)   ; led_L(0)    ;HAL_Delay(time);//Violeta
		led_R(0)   ;led_B(duty);led_G(duty); led_L(0)    ;HAL_Delay(time);//Celeste
		led_R(duty);led_B(0)   ;led_G(duty); led_L(0)    ;HAL_Delay(time);//Amarillo
		led_R(duty);led_B(duty);led_G(duty); led_L(0)    ;HAL_Delay(time);//Blanco
	 }
	 led_R(0);led_B(0);led_G(0);led_L(0);//apago todo
}


void Fade_All(uint16_t time){
    Fade_Colors(1,0,0,time);
	Fade_Colors(0,0,1,time);
	Fade_Colors(0,1,0,time);
    Fade_Colors(1,0,1,time);
	Fade_Colors(0,1,1,time);
	Fade_Colors(1,1,0,time);
	Fade_Colors(1,1,1,time);
}

void Fade_TwoColor(uint8_t color, uint16_t time){
	maxDutyRGB = maxDutyRGB - 1;

	switch(color){
		case 1: for(uint16_t i = 0; i < maxDutyRGB; i++){led_G(maxDutyRGB-i);led_B(i);HAL_Delay(time);}
				for(uint16_t i = maxDutyRGB; i > 0; i--){led_G(maxDutyRGB-i);led_B(i);HAL_Delay(time);}
			break;

		case 2: for(uint16_t i = 0; i < maxDutyRGB; i++){led_G(maxDutyRGB-i);led_R(i);HAL_Delay(time);}
				for(uint16_t i = maxDutyRGB; i > 0; i--){led_G(maxDutyRGB-i);led_R(i);HAL_Delay(time);}
				for(uint16_t i = 0; i < maxDutyRGB; i++){led_G(maxDutyRGB-i);led_R(i);HAL_Delay(time);}
			break;
		case 3: for(uint16_t i = 0; i < maxDutyRGB; i++){led_R(maxDutyRGB-i);led_G(i);HAL_Delay(time);}
				for(uint16_t i = maxDutyRGB; i > 0; i--){led_R(maxDutyRGB-i);led_G(i);HAL_Delay(time);}
			break;

		case 4: for(uint16_t i = 0; i < maxDutyRGB; i++){led_R(maxDutyRGB-i);led_B(i);HAL_Delay(time);}
				for(uint16_t i = maxDutyRGB; i > 0; i--){led_R(maxDutyRGB-i);led_B(i);HAL_Delay(time);}
				for(uint16_t i = 0; i < maxDutyRGB; i++){led_R(maxDutyRGB-i);led_B(i);HAL_Delay(time);}
		    break;

		case 5: for(uint16_t i = 0; i < maxDutyRGB; i++){led_B(maxDutyRGB-i);led_R(i);HAL_Delay(time);}
				for(uint16_t i = maxDutyRGB; i > 0; i--){led_B(maxDutyRGB-i);led_R(i);HAL_Delay(time);}
				for(uint16_t i = 0; i < maxDutyRGB; i++){led_B(maxDutyRGB-i);led_G(i);HAL_Delay(time);}
			break;

	}
    led_R(0);led_B(0);led_G(0);led_L(0);//apago todo
}

void Fade_All_TwoColor(uint16_t time){
	Fade_TwoColor(1,time);
	Fade_TwoColor(2,time);
	Fade_TwoColor(3,time);
	Fade_TwoColor(4,time);
	Fade_TwoColor(5,time);
}

void Heart(uint8_t veces, uint16_t tiempo){
	for(uint8_t i=0; i<veces; i++){
		fade_L(tiempo);
	}
	led_L(0);
}


void Rojos_Inverted(uint16_t time){
	maxDutyRGB = maxDutyRGB - 1;

	for(uint16_t i = 0; i < maxDutyRGB; i++){led_L(maxDutyRGB-i);led_R(i);HAL_Delay(time);}
	for(uint16_t i = maxDutyRGB; i > 0; i--){led_L(maxDutyRGB-i);led_R(i);HAL_Delay(time);}

	led_R(0);led_B(0);led_G(0);led_L(0);//apago todo

}

void Rojos(uint16_t time){
	maxDutyRGB = maxDutyRGB - 1;

	for(uint16_t i = 0; i < maxDutyRGB; i++){led_L(i);led_R(i);HAL_Delay(time);}
	for(uint16_t i = maxDutyRGB; i > 0; i--){led_L(i);led_R(i);HAL_Delay(time);}

	led_R(0);led_B(0);led_G(0);led_L(0);//apago todo

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
