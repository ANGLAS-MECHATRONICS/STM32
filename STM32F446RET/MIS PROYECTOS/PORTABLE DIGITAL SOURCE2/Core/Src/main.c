/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stdio.h"
#include "OLED_SSD1306.h"
#include "IMAGENES.h"
#include "ADS1115.h"
#include "ENCODER_ROT.h"
#include "FUNCION_MAP.h"
#include "MCP4725.h"

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

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */

float adc0;
char buff[16];
float out_voltage=0;
float in_voltage;
float corriente;
float potencia;
static float encoder;
float ENCO=0;
float PWM=0;
float difference;
uint8_t mem=0,suma=0;//variables para el pulsador del encoder
uint16_t contMillis=0;

float Vdac,VoutMath;
const float Vref=1.263;
const float R1=46.61;//47k
const float R2=4.69;//4.7k
const float R3=4.63;//4.7k
const float hysteresis = 0.2;
const uint8_t muestras=3;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */
void medirCorriente(void);
void medirPotencia(void);
void medirVoltBateria(void);
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
  MX_I2C1_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  HAL_TIM_Base_Start_IT(&htim6);


  OLED_Print_Text(2,104,1,"OFF");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1){
	  //HAL_ADC_Start(&hadc1);
	  //HAL_ADC_PollForConversion(&hadc1, 100);
	  //adc0 = HAL_ADC_GetValue(&hadc1) * 3.3/4096;
	  //HAL_ADC_Stop(&hadc1);
	  //sprintf(buff,"%2.2fV",adc0);
	  //OLED_Print_Text(2,0,2,buff);

      if(!HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) && !mem){mem = 1;}
      if( HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) &&  mem){mem = 0; suma++;}
      if(suma==1){
          suma=2;  //cambio el contador para que solo haga una vez todo lo que esta dentro del if
          HAL_GPIO_WritePin(EN_XL6009_GPIO_Port, EN_XL6009_Pin, SET);
          OLED_Print_Text(2,104,1,"ON ");
      }
      if(suma==3){
          suma=0; //cambio el contador para que solo haga una vez todo lo que esta dentro del if
          HAL_GPIO_WritePin(EN_XL6009_GPIO_Port, EN_XL6009_Pin, RESET);
          OLED_Print_Text(2,104,1,"OFF");
      }

      sprintf(buff,"SET:%4.0f",PWM);
      OLED_Print_Text(1,70,1,buff);
      MCP4725_Out(encoder+ENCO);
      HAL_Delay(5);


      Vdac = encoder*3.338/4096;
      //sprintf(buff,"%1.3f",Vdac);
      //OLED_Print_Text(2,70,1,buff);

      VoutMath = Vref+(((Vref/R2)+((Vref-Vdac)/R3))*R1);
      sprintf(buff,"%2.2f",VoutMath);
      OLED_Print_Text(3,70,1,buff);

	  out_voltage   = ADS1115_Read(0)*0.00117127;
	  //sprintf(buff,"%2.2fV ",out_voltage);
	  //OLED_Print_Text(0,0,2,buff);

	  ////////////////////////////////////////////////////////////////////////////////
	  if(VoutMath > out_voltage){
		  difference = VoutMath - out_voltage;
		  if(difference>=0.001 && difference<=0.150){
			  ENCO=ENCO-1;
		  }else if(difference>0.150 && difference<=0.250){
			  ENCO=ENCO-2;
		  }else if(difference>0.250 && difference<=0.350){
			  ENCO=ENCO-3;
		  }else if(difference>0.350 && difference<=0.450){
			  ENCO=ENCO-4;
		  }else if(difference>0.450 && difference<=0.550){
			  ENCO=ENCO-5;
		  }else if(difference>0.550 && difference<=0.650){
			  ENCO=ENCO-6;
		  }
		  PWM = encoder+ENCO;
	  }

	  if(VoutMath < out_voltage){
		  difference = out_voltage - VoutMath;
		  if(difference>=0.001 && difference<=0.150){
			  ENCO=ENCO+1;
		  }else if(difference>0.150 && difference<=0.250){
			  ENCO=ENCO+2;
		  }else if(difference>0.250 && difference<=0.350){
			  ENCO=ENCO+3;
		  }else if(difference>0.350 && difference<=0.450){
			  ENCO=ENCO+4;
		  }else if(difference>0.450 && difference<=0.550){
			  ENCO=ENCO+5;
		  }else if(difference>0.550 && difference<=0.650){
			  ENCO=ENCO+6;
		  }
		  PWM = encoder+ENCO;
	  }
	  /////////////////////////////////////////////////////////////////////////////////////

	  if(contMillis>=90){//cada 120ms imprimo en el oled

		  difference = VoutMath - out_voltage;

		  if(difference>0){
			  if(difference>=0.01 && difference<=0.03){
				  sprintf(buff,"%2.2fV ",VoutMath);
				  OLED_Print_Text(0,0,2,buff);
			  }else if(difference>0.03 && difference<=0.06){
				  sprintf(buff,"%2.2fV ",VoutMath-0.01);
				  OLED_Print_Text(0,0,2,buff);
			  }else if(difference>0.06 && difference<=0.10){
				  sprintf(buff,"%2.2fV ",VoutMath-0.02);
				  OLED_Print_Text(0,0,2,buff);
			  }else if(difference>0.10 && difference<=0.15){
				  sprintf(buff,"%2.2fV ",VoutMath-0.03);
				  OLED_Print_Text(0,0,2,buff);
			  }else if(difference>0.15){
				  sprintf(buff,"%2.2fV ",out_voltage);
				  OLED_Print_Text(0,0,2,buff);
			  }

		  }else{
			  difference = difference*-1;
			  if(difference>=0.01 && difference<=0.03){
				  sprintf(buff,"%2.2fV ",VoutMath);
				  OLED_Print_Text(0,0,2,buff);
			  }else if(difference>0.03 && difference<=0.06){
				  sprintf(buff,"%2.2fV ",VoutMath+0.01);
				  OLED_Print_Text(0,0,2,buff);
			  }else if(difference>0.06 && difference<=0.10){
				  sprintf(buff,"%2.2fV ",VoutMath+0.02);
				  OLED_Print_Text(0,0,2,buff);
			  }else if(difference>0.10 && difference<=0.15){
				  sprintf(buff,"%2.2fV ",VoutMath+0.03);
				  OLED_Print_Text(0,0,2,buff);
			  }else if(difference>0.15){
				  sprintf(buff,"%2.2fV ",out_voltage);
				  OLED_Print_Text(0,0,2,buff);
			  }
		  }

		  //out_voltage   = ADS1115_Read(0)*0.00117127;
		  //sprintf(buff,"%2.2fV ",out_voltage);
		  //OLED_Print_Text(0,0,2,buff);

		  medirCorriente();
		  //medirPotencia();
		  medirVoltBateria();
		  //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		  contMillis=0;
		}

	  //HAL_Delay(50);

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 64;
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

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 64000-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 5-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EN_XL6009_GPIO_Port, EN_XL6009_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : DT_Pin CLK_Pin */
  GPIO_InitStruct.Pin = DT_Pin|CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : EN_XL6009_Pin */
  GPIO_InitStruct.Pin = EN_XL6009_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EN_XL6009_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SW_Pin */
  GPIO_InitStruct.Pin = SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {//Leemos el encoder cada 5ms
		encoder = Encoder_Run();
		contMillis++;
	}
}

void medirCorriente(void){
  corriente = ADS1115_Read(1)*0.078125;
  if(corriente>=0){
	  sprintf(buff,"%4.0fmA",corriente);
	  OLED_Print_Text(2,0,2,buff);
  }else{
	  OLED_Print_Text(2,0,2,"   0mA");
  }
}

void medirPotencia(void){
  potencia  = out_voltage*corriente/1000;
  if(potencia>=0){
	  sprintf(buff,"%2.2fW",potencia);
	  OLED_Print_Text(2,70,2,buff);
  }else{
	  OLED_Print_Text(2,70,2,"0.00W ");
  }
}

void medirVoltBateria(void){
  in_voltage   = ADS1115_Read(2)*0.00049449;
  sprintf(buff,"VIN:%2.1fV",in_voltage);
  OLED_Print_Text(0,70,1,buff);
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
