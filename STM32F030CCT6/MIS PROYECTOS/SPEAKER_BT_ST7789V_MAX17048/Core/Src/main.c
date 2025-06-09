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
#include "stdio.h"
#include "Anglas_ST7789.h"
#include "fonts.h"
#include "math.h"
#include "bitmap.h"

#include "Anglas_MAX17048.h"

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

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim16;

/* USER CODE BEGIN PV */

float soc,voltage;
char buff[32];
uint16_t freq_BLK = 5000;//5khz

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM16_Init(void);
/* USER CODE BEGIN PFP */
void control_BTN1(void);
void PWM_set_Freq_DutyCycle(uint16_t freq, uint8_t duty);
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
  MX_SPI1_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

  /******************** PARA USAR LOS DOS SYS_WKUP **********************************/

  // Verifica si el sistema reanudó desde Standby Mode
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); // Limpia la bandera

      //Esto hace cuando se presiona el pulsador del SYS_WKUP1
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 1) {//FUNCION 1
    	  //AQUI TODO EL CODIGO DE LA FUNCION 1
    	  if (HAL_GPIO_ReadPin(DRAIN_LATCH_GPIO_Port, DRAIN_LATCH_Pin) == 0){
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 1);//Encender parlante
			  HAL_Delay(4000);//tiempo para apagar luego de encender o viceversa
    	  }else{
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 0);//Apagar parlante
			  MAX17048_SetAlertThreshold(1);//al colocar pone a 0 el pin alert y esto hace que consuma unos mA menos
			  HAL_Delay(4000);//tiempo para apagar luego de encender o viceversa

    	  }



      }

      //Esto hace cuando se presiona el pulsador del SYS_WKUP2
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == 1) {//FUNCION 2
    	  //verifica si esta encendido lo mantiene asi y si esta apagado lo mantiene apagado
    	  if (HAL_GPIO_ReadPin(DRAIN_LATCH_GPIO_Port, DRAIN_LATCH_Pin) == 0){
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 0);
    	  }else{
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 1);
    	  }

    	  PWM_set_Freq_DutyCycle(freq_BLK,0);//iniciar pantalla negra
    	  ST7789_Init();
    	  ST7789_rotation(2);
    	  PWM_set_Freq_DutyCycle(freq_BLK,100);
    	  ST7789_DrawImage(0, 0, 240, 135, img2);

    	  MAX17048_Init();
    	  MAX17048_QuickStart();
    	  MAX17048_SetAlertThreshold(1);
    	  HAL_Delay(5);
    	  voltage = MAX17048_GetVoltage();
    	  sprintf(buff,"VOLTAGE: %1.2fV",voltage);
    	  ST7789_print(40, 50, RGB565(0, 255, 0) , RGB565(0, 0, 0) , 1, &Font_11x18, 1, buff);

    	  soc = MAX17048_GetSoc();
    	  sprintf(buff,"%3i%%",(uint8_t)soc);
    	  ST7789_print(140, 5, RGB565(0, 0, 255) , RGB565(255, 255, 255) , 1, &Font_16x28, 1, buff);
    	  HAL_Delay(4000);
    	  PWM_set_Freq_DutyCycle(freq_BLK,0);//iniciar pantalla negra
    	  HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);
    	  ST7789_SleepModeEnter();
      }

      HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);// Desactiva el Wake-Up Pin1
      HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);// Desactiva el Wake-Up Pin2
  }

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);//AHORA RECIEN VAMOS A INGRESAR AL STANDBY MODE
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);//habilitamos el Wake-Up Pin1
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);//habilitamos el Wake-Up Pin2
    HAL_PWR_EnterSTANDBYMode();//FINALMENTE INGRESAMOS AL STANDBY MODE

  /********************************************/



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {


	  //voltage = MAX17048_GetVoltage();
	  //sprintf(buff,"VOLTAGE: %1.2fV",voltage);
	  //ST7789_print(40, 20, RGB565(0, 255, 0) , RGB565(0, 0, 0) , 1, &Font_11x18, 1, buff);



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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 48-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DC_Pin|RST_Pin|ON_OFF_3V7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : DC_Pin RST_Pin CS_Pin */
  GPIO_InitStruct.Pin = DC_Pin|RST_Pin|CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : ON_OFF_3V7_Pin */
  GPIO_InitStruct.Pin = ON_OFF_3V7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ON_OFF_3V7_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DRAIN_LATCH_Pin ALERT_Pin */
  GPIO_InitStruct.Pin = DRAIN_LATCH_Pin|ALERT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/*
void control_BTN1(void){
    //Mantener presionado el SW2 por cierto tiempo, para encender o apagar el XL6019
    if(HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == 1){//RESISTENCIA PULL DOWN EXTERNA
  	  contBTN1++;
  	  if(contBTN1>300000){// 1.5seg aprox
  		contBTN1=0;
  		powerSupply++;
  	  }
    }

    //Garantiza que el contSW2 siempre inicie de 0
    if(HAL_GPIO_ReadPin(BTN1_GPIO_Port, BTN1_Pin) == 0 && contBTN1>0){contBTN1=0;}

    //Salida de voltaje de XL6019 ON
    if(powerSupply==1){
  	  powerSupply=2;
  	  estado=1;
  	  //HAL_GPIO_WritePin(EN_3V7_GPIO_Port, EN_3V7_Pin, RESET);
  	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, SET);
    }

    //Salida de voltaje de XL6019 OFF
    if(powerSupply==3){
  	  powerSupply=0;
  	  estado=0;
  	  //HAL_GPIO_WritePin(EN_3V7_GPIO_Port, EN_3V7_Pin, SET);
  	  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, RESET);
    }
}
*/

void PWM_set_Freq_DutyCycle(uint16_t freq, uint8_t duty){
	/*
	 * La frecuencia a la que opera el uC es de 48000000Hz
	 *
	 * El prescaler PSC-16bits divide a esta frecuencia
	 * 48000000/48 = 1000000Hz = 1Mhz
	 *
	 * El CounterPeriod solo es un contador de 0 - 65535 y cuando lo establecemos en un valor digamos 10000
	 * entonces va contar de 0  a 10000 y luego se desbordara es decir reiniciara y empezara de cero otra vez hasta 10000
	 * Ahora el CounterPeriod sirve para cambiar el periodo y con esto la frecuencia T = 1 / freq
	 * 		__HAL_TIM_GET_AUTORELOAD(&htim16): Esta función devuelve el valor actual del registro ARR (Auto-Reload Register)
	 * 		del temporizador, que representa el Counter Period.
	 * */
	uint16_t valor_CCR;
	uint32_t CounterPeriod;//en microsegundos
	uint32_t freqOsc = HAL_RCC_GetHCLKFreq();//obtiene la frecuencia del OSC en Hz (48000000)
	uint16_t prescaler = 48;//PSC-16bits

	if(duty > 100) duty = 100;
	CounterPeriod = freqOsc/(freq*prescaler);//aqui nos da el periodo en microsegundos
	//y este periodo(CounterPeriod) necesita la funcion __HAL_TIM_SET_AUTORELOAD

    HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);//inicia la generacion de la señal PWM
    //modificamos el valor del CounterPeriod y con esto modificamos la frecuencia de salida
	__HAL_TIM_SET_AUTORELOAD(&htim16,CounterPeriod);//Si quiero cambiar la frecuencia modifico: __HAL_TIM_SET_AUTORELOAD(&htim16, 1000000/frecuencia que quiero) 1000000porque es microsegundos
	//CounterPeriod = __HAL_TIM_GET_AUTORELOAD(&htim16)
	//visto con un analizador logico en un 50% de duty me da 49.5% y cuando le sumo 2 me da 50.05%
	valor_CCR = (CounterPeriod + 2)* duty/100.0;//divido entre 100 porque quiero de 0 a 100%, cambiar el rango si se desea
    __HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, valor_CCR);
    //HAL_Delay(500);
    //HAL_TIM_PWM_Stop(&htim16, TIM_CHANNEL_1);//aqui detengo la generacion del PWM, pero para mi caso necesito que siempre ete activado
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
