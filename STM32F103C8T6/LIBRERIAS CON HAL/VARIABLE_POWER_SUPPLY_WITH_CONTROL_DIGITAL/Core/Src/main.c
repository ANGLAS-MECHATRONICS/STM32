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
#include "stdio.h" //para sprintf
#include "Anglas_OLED_SSD1306_DMA.h"
#include "Anglas_IMAGENES.h"
#include "Anglas_ENCODER_ROT.h"
#include "Anglas_MCP4725.h"
#include "Anglas_INA226.h"
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
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;

TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
extern uint16_t encoder;
extern uint16_t paso_Encoder;

uint16_t dacOut;
char buff[50];
float power,current,voltage;
float Vbat;
uint8_t powerSupply=0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */
void medirVoltage(void);
void medirCorriente(void);
void medirPotencia(void);
void control_Button_Encoder(void);
void Control_Estabilizar(void);
void calculate_value_dac_12bits(void);
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
  MX_I2C1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim4);

  OLED_Init_DMA();
  //OLED_Imagen_Small_DMA(2,0, AM_INTRO, 128, 64);
  //OLED_Print_Text_DMA(1,0,1,"Designed by G. Anglas");
  //OLED_Print_Text_DMA(0,110,1,"100");

  INA226_Init(3.2768,100,AVG_64,T_Vbus_1_1ms,T_Vshunt_1_1ms,MODE_SHUNT_BUS_CONTINUOUS);
  INA226_Mode_pinAlert(SHUNT_VOLTAGE_OVER);


  OLED_Print_Text_DMA(3,100,2,"OFF");
  OLED_Print_Text_DMA(5,104,1,"1.0");
  OLED_Print_Text_DMA(2,0,3,"0.0V ");
  OLED_Print_Text_DMA(6,25,2,"   0mA");
  OLED_Print_Text_DMA(7,96,1,"0.0W ");

  uint32_t startTime = HAL_GetTick(); // Tiempo inicial



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  dacOut = encoder;
	  sprintf(buff,"%4i",dacOut);
	  OLED_Print_Text_DMA(6,96,1,buff);
	  MCP4725_Out(dacOut);

	  control_Button_Encoder();
	  voltage = INA226_Vbus();


	  //MOSTRAR EN EL DISPLAY CADA 700ms
	  if( HAL_GetTick() - startTime >= 700){
    	  medirVoltage();
		  medirCorriente();
		  medirPotencia();
		  startTime = HAL_GetTick();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 36000-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 10-1;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EN_XL6019_GPIO_Port, EN_XL6019_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : EN_XL6019_Pin */
  GPIO_InitStruct.Pin = EN_XL6019_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EN_XL6019_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SW_Pin DT_Pin CLK_Pin */
  GPIO_InitStruct.Pin = SW_Pin|DT_Pin|CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void control_Button_Encoder(void){
	static uint8_t mem=0,suma=1;//suma=1 para iniciar en pasos de  100
	//Control del pulsador del ENCODER (SW)
    if(HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == 0 && mem == 0){mem = 1;}
    if(HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == 1 && mem == 1){suma++; mem = 0;}
    if(suma==1){suma=2;paso_Encoder = 100;OLED_Print_Text_DMA(5,104,1,"100");}
    if(suma==3){suma=4;paso_Encoder =  10;OLED_Print_Text_DMA(5,104,1," 10");}
    if(suma==5){suma=0;paso_Encoder =   1;OLED_Print_Text_DMA(5,104,1,"  1");}


    // Mantener presionado el SW por 2 segundos para encender o apagar el XL6019
    if (HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == 0) {
        uint32_t startTime = HAL_GetTick();  //Guardar el tiempo en que se presionó el botón
        // Esperar mientras el botón está presionado
        while (HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == 0) {
            if (HAL_GetTick() - startTime >= 1200) {  // Si han pasado 1.2 segundos
            	//para no modificar paso_Encoder agrego estas condiciones
            	if(suma==2) suma=0;
            	else if(suma==4) suma=2;
            	else if(suma==0) suma=4;

            	powerSupply = !powerSupply;  // Alternar el estado de powerSupply
                if(powerSupply == 1) OLED_Print_Text_DMA(3,100,2,"ON ");
                else				 OLED_Print_Text_DMA(3,100,2,"OFF");
                HAL_GPIO_TogglePin(EN_XL6019_GPIO_Port, EN_XL6019_Pin);
                break;  // Salir del bucle
            }
        }
    }
}

void medirVoltage(void){
	voltage = INA226_Vbus();
	if(voltage <= 9.99999) {
    	sprintf(buff,"%1.1fV ",voltage);
    	OLED_Print_Text_DMA(2,0,3,buff);
    	//cuado cambia de 9.9v a 10v se ve afectado estas variables y por momentos se borran
		sprintf(buff,"%2.1fV",Vbat);
		OLED_Print_Text_DMA(2,98,1,buff);
		if(powerSupply==2) OLED_Print_Text_DMA(3,100,2,"ON ");
    }else{
    	sprintf(buff,"%2.1fV",voltage);
    	OLED_Print_Text_DMA(2,0,3,buff);
    	//cuado cambia de 9.9v a 10v se ve afectado estas variables y por momentos se borran
		sprintf(buff,"%2.1fV",Vbat);
		OLED_Print_Text_DMA(2,98,1,buff);
		if(powerSupply==2) OLED_Print_Text_DMA(3,100,2,"ON ");
    }
}

void medirCorriente(void){
	current = INA226_Current();
	if(current>=0){
		sprintf(buff,"%4.0fmA",current);
		OLED_Print_Text_DMA(6,25,2,buff);
	}else{
		OLED_Print_Text_DMA(6,25,2,"   0mA");
	}
}

void medirPotencia(void){
	power = INA226_Power();
    if(power>=0){
	    sprintf(buff,"%2.1fW ",power);
	    OLED_Print_Text_DMA(7,96,1,buff);
    }else{
	    OLED_Print_Text_DMA(7,96,1,"0.0W ");
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
