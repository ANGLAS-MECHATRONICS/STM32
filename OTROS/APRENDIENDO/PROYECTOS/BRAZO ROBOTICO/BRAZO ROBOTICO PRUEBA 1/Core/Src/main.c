/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
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
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint16_t adcSamples[7];
char adc1Buffer[20];
char adc2Buffer[20];
char adc3Buffer[20];
char adc4Buffer[20];
char adc5Buffer[20];
char adc6Buffer[20];
char adc7Buffer[20];
uint8_t stringSize[7];

uint8_t i=0;
uint16_t servos[7];
uint16_t tiempo=25;
uint16_t contServos=0;
uint8_t estado;
uint8_t contGrabar=0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
//////////////////////// Function map/////////////////////////////////////////////////////////////
double map(float valor, float entradaMin, float entradaMax, float salidaMin, float salidaMax){
	   return ((((valor-entradaMin)*(salidaMax-salidaMin))/(entradaMax-entradaMin))+salidaMin);}
//////////////////// Function to move the SERVO1_BASE a desired position ////////////////////////////////
void control_base(uint8_t ang1, uint8_t ang2, uint8_t vel){
	 if(ang2 >= ang1){
	    for(uint8_t angulo = ang1; angulo < ang2; angulo++){
        __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,angulo);
        HAL_Delay(vel);}}
	 else{
		for(uint8_t angulo = ang1; angulo > ang2; angulo--){
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,angulo);
		HAL_Delay(vel);}}}
//////////////////// Function to move the SERVO1_WRIST a desired position ////////////////////////////////
void control_wrist(uint8_t ang1, uint8_t ang2, uint8_t vel){
	 if(ang2 >= ang1){
	    for(uint8_t angulo = ang1; angulo < ang2; angulo++){
        __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,angulo);
        HAL_Delay(vel);}}
	 else{
		for(uint8_t angulo = ang1; angulo > ang2; angulo--){
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,angulo);
		HAL_Delay(vel);}}}
//////////////////// Function to move the SERVO7_GRIPPER a desired position ////////////////////////////////
void control_gripper(uint8_t ang1, uint8_t ang2, uint8_t vel){
	 if(ang2 >= ang1){
	    for(uint8_t angulo = ang1; angulo < ang2; angulo++){
        __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,angulo);
        HAL_Delay(vel);}}
	 else{
		for(uint8_t angulo = ang1; angulo > ang2; angulo--){
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,angulo);
		HAL_Delay(vel);}}}
///////////////////FUNCION PARA MOVER LOS SERVOS/////////////////////////////////////////////
void todosServos(uint8_t ang1,uint8_t ang2, uint8_t ang3, uint8_t ang4,
		         uint8_t ang5,uint8_t ang6, uint8_t ang7, uint8_t tiempo){

	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,ang1);
	HAL_Delay(tiempo);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,ang2);
    HAL_Delay(tiempo);
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,ang3);
    HAL_Delay(tiempo);
    __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,ang4);
    HAL_Delay(tiempo);
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,ang5);
    HAL_Delay(tiempo);
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,ang6);
    HAL_Delay(tiempo);
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,ang7);
    HAL_Delay(tiempo);}

//todosServos(56,85,77,79,81,82,58,tiempo);
//todosServos(66,95,87,89,91,92,68,tiempo);

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
  MX_ADC1_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcSamples, 7);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);//PB4 - base
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);//PB5 -
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);//PB0 -
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);//PB1 -

  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);//PB6 -
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);//PB7 - wrist
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);//PB8 - gripper


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  //Initial position
  /*
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,58);//base
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,84);
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,76);
  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,78);
  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,80);
  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,91);//wrist
  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,50);//gripper

  HAL_Delay(2000);

  control_base(58, 88, 20);
  HAL_Delay(400);
  control_wrist(91, 115, 10);
  HAL_Delay(400);
  control_gripper(50, 83, 15);
  HAL_Delay(400);
  control_gripper(83, 50, 15);
  HAL_Delay(400);
  control_wrist(115, 44, 10);
  HAL_Delay(400);
  control_wrist(44, 91, 10);*/

  /////////////////////////////////////////////////


  //while(1);






  while (1)
  {


	  servos[0] = map(adcSamples[0], 0, 4095, 44, 115);                        //mapeo la lectura del pot1
	  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,servos[0]); 				    //escribo los grados del servo 0-180
	  stringSize[0] = sprintf(adc1Buffer,"todosServos(%u,",servos[0]);			//convierto a cadena para mandar al UART

	  servos[1] = map(adcSamples[1], 0, 4095, 115, 44);                        //mapeo la lectura del pot1
	  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,servos[1]); 				    //escribo los grados del servo 0-180
	  stringSize[1] = sprintf(adc2Buffer,"%u,",servos[1]);			//convierto a cadena para mandar al UART

  	  servos[2] = map(adcSamples[2], 0, 4095, 115, 44);                        //mapeo la lectura del pot1
  	  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,servos[2]); 				    //escribo los grados del servo 0-180
  	  stringSize[2] = sprintf(adc3Buffer,"%u,",servos[2]);			//convierto a cadena para mandar al UART

  	  servos[3] = map(adcSamples[3], 0, 4095, 115, 44);                        //mapeo la lectura del pot1
  	  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,servos[3]); 				    //escribo los grados del servo 0-180
  	  stringSize[3] = sprintf(adc4Buffer,"%u,",servos[3]);			//convierto a cadena para mandar al UART

  	  servos[4] = map(adcSamples[4], 0, 4095, 115, 44);                        //mapeo la lectura del pot1
  	  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1,servos[4]); 				    //escribo los grados del servo 0-180
  	  stringSize[4] = sprintf(adc5Buffer,"%u,",servos[4]);			//convierto a cadena para mandar al UART

  	  servos[5] = map(adcSamples[5], 0, 4095, 115, 44);                        //mapeo la lectura del pot1
  	  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,servos[5]); 				    //escribo los grados del servo 0-180
  	  stringSize[5] = sprintf(adc6Buffer,"%u,",servos[5]);			//convierto a cadena para mandar al UART

  	  servos[6] = map(adcSamples[6], 0, 4095, 58, 84);                        //mapeo la lectura del pot1
  	  __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,servos[6]); 				    //escribo los grados del servo 0-180
  	  stringSize[6] = sprintf(adc7Buffer,"%u,tiempo);\r\n",servos[6]);			//convierto a cadena para mandar al UART

      HAL_Delay(10);

  	  contServos++;

  	  if(contServos==10){
  		HAL_UART_Transmit(&huart1, (uint8_t *)adc1Buffer, stringSize[0], 100);    	//mando al puerto serial
  	  	HAL_UART_Transmit(&huart1, (uint8_t *)adc2Buffer, stringSize[1], 100);    	//mando al puerto serial
  	  	HAL_UART_Transmit(&huart1, (uint8_t *)adc3Buffer, stringSize[2], 100);    	//mando al puerto serial
  	  	HAL_UART_Transmit(&huart1, (uint8_t *)adc4Buffer, stringSize[3], 100);    	//mando al puerto serial
  	  	HAL_UART_Transmit(&huart1, (uint8_t *)adc5Buffer, stringSize[4], 100);    	//mando al puerto serial
  	    HAL_UART_Transmit(&huart1, (uint8_t *)adc6Buffer, stringSize[5], 100);    	//mando al puerto serial
  	    HAL_UART_Transmit(&huart1, (uint8_t *)adc7Buffer, stringSize[6], 100);    	//mando al puerto serial
        contServos=0;}

  	  /*
  	  estado = HAL_GPIO_ReadPin(boton1_GPIO_Port, boton1_Pin);
  	  if(estado==0){
  		  HAL_Delay(180);
  		  contGrabar=1;}

  	  if(contGrabar==1){
  		HAL_UART_Transmit(&huart1, (uint8_t *)adc1Buffer, stringSize[0], 100);    	//mando al puerto serial
  	  	HAL_UART_Transmit(&huart1, (uint8_t *)adc2Buffer, stringSize[1], 100);    	//mando al puerto serial
  	  	HAL_UART_Transmit(&huart1, (uint8_t *)adc3Buffer, stringSize[2], 100);    	//mando al puerto serial
  	  	HAL_UART_Transmit(&huart1, (uint8_t *)adc4Buffer, stringSize[3], 100);    	//mando al puerto serial
  	  	HAL_UART_Transmit(&huart1, (uint8_t *)adc5Buffer, stringSize[4], 100);    	//mando al puerto serial
  	    HAL_UART_Transmit(&huart1, (uint8_t *)adc6Buffer, stringSize[5], 100);    	//mando al puerto serial
  	    HAL_UART_Transmit(&huart1, (uint8_t *)adc7Buffer, stringSize[6], 100);    	//mando al puerto serial
  	    contGrabar=0;}*/




	  /*for(i=0;i<7;i++){
		  servos[i] = map(adcSamples[i], 0, 4095, 44, 115);
		  __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,servos[i]);
		  stringSize[i] = sprintf(adc1Buffer,"ADC%u : %u  ",i,servos[i]);
		  HAL_UART_Transmit(&huart1, (uint8_t *)adc1Buffer, stringSize[i], 100);
		  if(i==6){
			  stringSize[i] = sprintf(adc1Buffer,"servo= %u\r\n",servos[i]);
			  HAL_UART_Transmit(&huart1, (uint8_t *)adc1Buffer, stringSize[i], 100);}}*/





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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 7;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = ADC_REGULAR_RANK_7;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  htim3.Init.Prescaler = 1400;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000;
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
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 1400;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : led1_Pin */
  GPIO_InitStruct.Pin = led1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(led1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : boton1_Pin */
  GPIO_InitStruct.Pin = boton1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(boton1_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
