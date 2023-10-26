/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
//#include "string.h"
#include "math.h"

#include "Anglas_OLED_SSD1306.h"
#include "Anglas_IMAGENES.h"
#include "Anglas_ENCODER_ROT.h"
#include "Anglas_INA226.h"
#include "Anglas_FUNCION_MAP.h"

#include "eeprom.h"
#include "eeprom_Config.h"
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
ADC_HandleTypeDef hadc;

DAC_HandleTypeDef hdac1;
DMA_HandleTypeDef hdma_dac1_ch1;

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

/* USER CODE BEGIN PV */
char buff[16];
//float encoder;
float dac_12bits;
float power,current,voltage,shunt,Vshunt;
float Vbat,Vdac,set_Voltage_Encoder,difference,PWM,ENCO=0;
float adcVbat=0,VbatADC;
float rango=0.05;//0.08
float step=0.3;//0.5
float limitCurrent;

uint8_t flagBattery=0,sumaSW1=0,flagSW1=0,contSW1=0,mem=0,suma=0,mem1=0,suma1=0,mem2=0,suma2=0,memButton=0,powerSupply=0,muestras=50, alertCurrent;
uint8_t activate_Protec_Current=0,flag_Exceed_CurrentLimit=0,flag_Exceed_CurrentLimit2=0;
uint16_t contButton=0,timerShowIconBattery=0,timerShowAllData=0;
uint32_t dacDMA[1];

extern float valor_Encoder;
extern float paso_Encoder;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_DAC1_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM15_Init(void);
static void MX_TIM14_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM17_Init(void);
/* USER CODE BEGIN PFP */
void medirVoltage(void);
void medirCorriente(void);
void medirPotencia(void);
void Control_Estabilizar(void);
void PWM_set_Freq_DutyCycle(uint16_t freq, uint8_t duty, uint32_t tiempo);
void medirCargaBateria(void);
void cut_CurrenteLimit_ON(void);
void cut_CurrenteLimit_ON_Set(float limitCurrentSet);
void show_Exceed_CurrentLimit(void);
void update_Data_on_Display(void);
void medirVoltageBattery(void);
void calculate_value_dac_12bits(void);
void control_SW(void);
void control_SW1(void);
void control_SW2(void);
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
  MX_DAC1_Init();
  MX_ADC_Init();
  MX_TIM15_Init();
  MX_TIM14_Init();
  MX_TIM6_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */

  //Subir estas dos lineas solo la 1ra vez, luego dejarlas comentadas
  //ee_writeToRam(0, sizeof(float), (uint8_t*)&valor_Encoder);//escribo en al eeprom
  //ee_commit();

  ee_read(0, sizeof(float), (uint8_t*)&valor_Encoder);//leo el valor de la eeprom

  OLED_Init_DMA();
  OLED_Imagen_Small_DMA(2,0, AM_INTRO, 128, 64);
  OLED_Print_Text_DMA(1,0,1,"Designed by G. Anglas");
  while(HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin));

  if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin)==0){
	  activate_Protec_Current = 1;
	  HAL_Delay(1500);
  }

  HAL_TIM_Base_Start_IT(&htim14);//encoder
  HAL_TIM_Base_Start_IT(&htim6);//dac_dma
  HAL_TIM_Base_Start_IT(&htim16);//icono de bateria
  HAL_TIM_Base_Start_IT(&htim17);//todos los datos del oled

  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)dacDMA, 1, DAC_ALIGN_12B_R);

  OLED_Clear_DMA();

  INA226_Init_DMA(3.2768,25,AVG_64,T_Vbus_1_1ms,T_Vshunt_1_1ms,MODE_SHUNT_BUS_CONTINUOUS);
  INA226_Mode_pinAlert_DMA(SHUNT_VOLTAGE_OVER);
  INA226_Alert_Limit_DMA(2000);

  OLED_Print_Text_DMA(3,100,2,"OFF");
  OLED_Print_Text_DMA(5,104,1,"1.0");
  OLED_Print_Text_DMA(2,0,3,"0.0V ");
  OLED_Print_Text_DMA(6,25,2,"   0mA");
  OLED_Print_Text_DMA(7,96,1,"0.0W ");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  update_Data_on_Display();
	  control_SW();

      //Modo proteccion de sobrecorriente
      if(!activate_Protec_Current) cut_CurrenteLimit_ON();//corte automatico por limite de corriente fija establecida
      else cut_CurrenteLimit_ON_Set(2500);//corte automatico por limite de corriente 2500mA a todos los voltajes

      //Evento de corte de sobrecorriente y mensaje de alerta el en OLED
      if(flag_Exceed_CurrentLimit==1) show_Exceed_CurrentLimit();






      //Mantener presionado el SW2 por cierto tiempo, para encender o apagar el XL6019
      if(HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == 0){
    	  contButton++;
    	  if(contButton>15){// 1.5seg aprox
    		  contButton=0;
    		  powerSupply++;
    	  }
      }

      //Garantiza que el contButton siempre inicie de 0
      if(HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == 1 && contButton>0){contButton=0;}

      //Salida de voltaje de XL6019 ON
      if(powerSupply==1){
    	  powerSupply=2;
    	  OLED_Print_Text_DMA(3,100,2,"ON ");
    	  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
    	  HAL_GPIO_WritePin(EN_XL6019_GPIO_Port, EN_XL6019_Pin, SET);
    	  HAL_Delay(6);//tiempo minimo que necesita para detectar la sobrecorriente al habilitar el XL6019

          if(flag_Exceed_CurrentLimit==1){
        	  show_Exceed_CurrentLimit();
          }else{
        	  PWM_set_Freq_DutyCycle(4000,50,100);
        	  ee_writeToRam(0, sizeof(float), (uint8_t*)&valor_Encoder);//escribo en al eeprom
        	  ee_commit();
          }
      }

      //Salida de voltaje de XL6019 OFF, luego de detectar sobrecorriente
      if(powerSupply==3 && flag_Exceed_CurrentLimit2==1){
    	  powerSupply=0;
    	  flag_Exceed_CurrentLimit2=0;
      }

      //Salida de voltaje de XL6019 OFF
      if(powerSupply==3 && flag_Exceed_CurrentLimit2==0){
    	  powerSupply=0;
    	  OLED_Print_Text_DMA(3,100,2,"OFF");
    	  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
    	  HAL_GPIO_WritePin(EN_XL6019_GPIO_Port, EN_XL6019_Pin, RESET);
    	  PWM_set_Freq_DutyCycle(1000,50,100);
    	  ENCO=0;//ver si eliminar esta linea
      }

      control_SW1();

      //Mido el volaje de salida del XL6019 constantemente
      voltage = INA226_Vbus_DMA();

      calculate_value_dac_12bits();
      Control_Estabilizar();
      medirVoltageBattery();

      ///////////////////////////// MOSTRAR EN EL DISPLAY CADA CIERTO TIEMPO //////////////////////////////////////////
      if(timerShowAllData>=7){//cada x * 100ms
    	  medirVoltage();
		  medirCorriente();
		  medirPotencia();
		  timerShowAllData=0;
      }

      if(timerShowIconBattery>=1){//cada x * 1s
    	  if(HAL_GPIO_ReadPin(stateCharger_GPIO_Port, stateCharger_Pin)==0){
    		  medirCargaBateria();
    	  }else{
    		  //blink icono bateria al momento de conectar el cargador
    		  flagBattery = !flagBattery;
    		  if(flagBattery) OLED_Imagen_Small_DMA(0, 96, chargerBattery, 32, 16);
    		  else 			  OLED_Imagen_Small_DMA(0, 96, chargerBattery2, 32, 16);
    	  }

          sprintf(buff,"%2.1fV",Vbat);
          OLED_Print_Text_DMA(2,98,1,buff);

    	  timerShowIconBattery=0;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
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
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_8B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */

  /** DAC Initialization
  */
  hdac1.Instance = DAC;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

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
  hi2c1.Init.Timing = 0x20000209;
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

  /** I2C Fast mode Plus enable
  */
  __HAL_SYSCFG_FASTMODEPLUS_ENABLE(I2C_FASTMODEPLUS_I2C1);
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
  htim6.Init.Prescaler = 0;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 32000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 48000-1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 5-1;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 24-1;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 20000-1;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
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
  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
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
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */
  HAL_TIM_MspPostInit(&htim15);

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

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 48000-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 1000-1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 48000-1;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 100-1;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EN_XL6019_GPIO_Port, EN_XL6019_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, LED1_Pin|LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : stateCharger_Pin */
  GPIO_InitStruct.Pin = stateCharger_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(stateCharger_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : EN_XL6019_Pin */
  GPIO_InitStruct.Pin = EN_XL6019_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EN_XL6019_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SW1_Pin SW2_Pin */
  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin LED2_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : INT_OVER_CURRENT_Pin */
  GPIO_InitStruct.Pin = INT_OVER_CURRENT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(INT_OVER_CURRENT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CLK_Pin DT_Pin SW_Pin */
  GPIO_InitStruct.Pin = CLK_Pin|DT_Pin|SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){//interrupcion externa que viene de pinAlert del INA226
	powerSupply=3;
	flag_Exceed_CurrentLimit=1;
	flag_Exceed_CurrentLimit2=1;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM14) {//Leemos el encoder cada 5ms
		set_Voltage_Encoder = Encoder_Run();
	}

	if (htim->Instance == TIM16) {//Mostramos el icono de bateria en el oled cada 1000ms
		timerShowIconBattery++;
	}

	if (htim->Instance == TIM17) {//Mostramos el todos los datos en el oled cada 100ms
		timerShowAllData++;
	}
}

void medirVoltage(void){
	voltage = INA226_Vbus_DMA();
    if(voltage <= 9.9) {
    	OLED_Print_Text_DMA(2,80,3," ");
    	sprintf(buff,"%1.1fV ",voltage);
    	OLED_Print_Text_DMA(2,0,3,buff);
    }else{
    	sprintf(buff,"%2.1fV",voltage);
    	OLED_Print_Text_DMA(2,0,3,buff);
    }
}

void medirCorriente(void){
	current = INA226_Current_DMA();
	if(current>=0){
		sprintf(buff,"%4.0fmA",current);
		OLED_Print_Text_DMA(6,25,2,buff);
	}else{
		OLED_Print_Text_DMA(6,25,2,"   0mA");
	}
}

void medirPotencia(void){
	power = INA226_Power_DMA();
    if(power>=0){
	    sprintf(buff,"%2.1fW ",power);
	    OLED_Print_Text_DMA(7,96,1,buff);
    }else{
	    OLED_Print_Text_DMA(7,96,1,"0.0W ");
    }
}

void Control_Estabilizar(void){

    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);

    if(set_Voltage_Encoder > voltage){
    	difference = set_Voltage_Encoder - voltage;
    	if(difference>=0.001 && difference<=rango){
    		ENCO=ENCO-step;
    	}else if(difference>rango && difference<=rango*2){
    		ENCO=ENCO-step;
    	}else if(difference>rango*2 && difference<=rango*3){
    		ENCO=ENCO-step;
    	}else if(difference>rango*3 && difference<=rango*4){
    		ENCO=ENCO-step;
    	}else if(difference>rango*4 && difference<=rango*5){
    		ENCO=ENCO-step;
    	}else if(difference>rango*5 && difference<=rango*6){
    		ENCO=ENCO-step*2;
    	}else if(difference>rango*6 && difference<=rango*7){
    		ENCO=ENCO-step*3;
    	}else if(difference>rango*7 && difference<=rango*8){
    		ENCO=ENCO-step*4;
    	}
    	PWM = dac_12bits+ENCO;
    	dacDMA[0]=PWM;
    }

    if(set_Voltage_Encoder < voltage){
    	difference = voltage - set_Voltage_Encoder;
    	if(difference>=0.001 && difference<=rango){
    		ENCO=ENCO+step;
    	}else if(difference>rango && difference<=rango*2){
    		ENCO=ENCO+step;
    	}else if(difference>rango*2 && difference<=rango*3){
    		ENCO=ENCO+step;
    	}else if(difference>rango*3 && difference<=rango*4){
    		ENCO=ENCO+step;
    	}else if(difference>rango*4 && difference<=rango*5){
    		ENCO=ENCO+step;
    	}else if(difference>rango*5 && difference<=rango*6){
    		ENCO=ENCO+step*2;
    	}else if(difference>rango*6 && difference<=rango*7){
    		ENCO=ENCO+step*3;
    	}else if(difference>rango*7 && difference<=rango*8){
    		ENCO=ENCO+step*4;
    	}
    	PWM = dac_12bits+ENCO;
    	dacDMA[0]=PWM;
    }
}

void PWM_set_Freq_DutyCycle(uint16_t freq, uint8_t duty, uint32_t tiempo){
	uint16_t valor_CCR;
	uint32_t freqOsc = 48000000;//Hz
	uint16_t prescaler = 24;//24-1     PSC-16bits

	if(duty > 100) duty = 100;
	freq = freqOsc/(freq*prescaler);

    HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1);
	__HAL_TIM_SET_AUTORELOAD(&htim15,freq);//Si quiero cambiar la frecuencia modifico: __HAL_TIM_SET_AUTORELOAD(&htim15, frecuencia que quiero)
	valor_CCR = (__HAL_TIM_GET_AUTORELOAD(&htim15)+1)*duty/100;//__HAL_TIM_GET_AUTORELOAD(&htim15): esto es el valor de 10-1 osea 9, por eso le sumo 1 para volver a tener 10, y por 0.5 ya que quiero 50% del duty cycle
    __HAL_TIM_SET_COMPARE(&htim15, TIM_CHANNEL_1, valor_CCR);
    HAL_Delay(tiempo);
    HAL_TIM_PWM_Stop(&htim15, TIM_CHANNEL_1);
}

void medirCargaBateria(void){
	if(Vbat>9 && Vbat<=9.9){
		OLED_Imagen_Small_DMA(0, 96, bateria0, 32, 16);
	}else if(Vbat>9.9 && Vbat<=10.5){
		OLED_Imagen_Small_DMA(0, 96, bateria25, 32, 16);
	}else if(Vbat>10.5 && Vbat<=11.1){
		OLED_Imagen_Small_DMA(0, 96, bateria50, 32, 16);
	}else if(Vbat>11.1 && Vbat<=11.7){
		OLED_Imagen_Small_DMA(0, 96, bateria75, 32, 16);
	}else if(Vbat>11.7 && Vbat<=12.6){
		OLED_Imagen_Small_DMA(0, 96, bateria100, 32, 16);
	}
}

void cut_CurrenteLimit_ON(void){
	limitCurrent = -0.012 * pow(set_Voltage_Encoder, 4) + 0.7163 * pow(set_Voltage_Encoder, 3) - 11.064 * pow(set_Voltage_Encoder, 2) - 59 * set_Voltage_Encoder + 2541.3;
	INA226_Alert_Limit_DMA(limitCurrent);
	sprintf(buff,"Current: %4.0fmA",limitCurrent);
	OLED_Print_Text_DMA(1,0,1,buff);
}

void cut_CurrenteLimit_ON_Set(float limitCurrentSet){
	INA226_Alert_Limit_DMA(limitCurrentSet);
	sprintf(buff,"Current: %4.0fmA",limitCurrentSet);
	OLED_Print_Text_DMA(1,0,1,buff);
}

void show_Exceed_CurrentLimit(void){
	HAL_GPIO_WritePin(EN_XL6019_GPIO_Port, EN_XL6019_Pin, RESET);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);

	OLED_Clear_DMA();

    sprintf(buff,"%2.1fV ",set_Voltage_Encoder);
    OLED_Print_Text_DMA(2,45,2,buff);

	OLED_Print_Text_DMA(0,0,1,"EXCEED CURRENT LIMIT!");
	OLED_Print_Text_DMA(1,0,1,"   AT A VOLTAGE OF   ");
	sprintf(buff,"%4.0fmA",limitCurrent);
	OLED_Print_Text_DMA(4,0,3,buff);

	for(uint8_t i=0; i<3; i++){
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
		PWM_set_Freq_DutyCycle(6000,50,300);
		HAL_Delay(200);
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
		HAL_Delay(200);
	}
	HAL_Delay(1500);
	flag_Exceed_CurrentLimit=0;
	OLED_Clear_DMA();
}

void update_Data_on_Display(void){//actualizo 0.1, OFF, que no son tan relevantes
	// Pasos del encoder 1.0  o  0.1
	sprintf(buff,"%1.1f",paso_Encoder);
	OLED_Print_Text_DMA(5,104,1,buff);

	//ON OFF del XL6019
	if(powerSupply==2) OLED_Print_Text_DMA(3,100,2,"ON ");
	if(powerSupply==0) OLED_Print_Text_DMA(3,100,2,"OFF");

    //Ver el PID
    sprintf(buff,"%4.0f",PWM);
    OLED_Print_Text_DMA(6,96,1,buff);

    //Muestro el voltaje seteado(voltaje de salida deseado)
    sprintf(buff,"Voltage: %2.1fV ",set_Voltage_Encoder);
    OLED_Print_Text_DMA(0,0,1,buff);
}

void medirVoltageBattery(void){
	HAL_ADC_Start(&hadc);
	adcVbat=0;
    for(uint8_t i=0; i<muestras; i++) adcVbat += HAL_ADC_GetValue(&hadc);//Leo 20 muestras del adc para tener una lectura mas precisa
    adcVbat /= muestras;
    Vbat = adcVbat*0.05768;//(3.3/2^8)*4.3 ------ divisor resistivo V*10k/(10K+33K) -> V = 4.3
}

void calculate_value_dac_12bits(void){
	/* Calculo el valor "dac_12bits"(0-4095) a partir del set_Voltage_Encoder(0-30V) y Vdac(0-3.3V)
	 *
	 * La formula es la siguiente:
     * set_Voltage_Encoder = Vref+(((Vref/R2)+((Vref-Vdac)/R3))*R1);
     *
     * Para  R1=560k, R2=39k, R3=56k y Vref=1.25, se reduce a:
     * set_Voltage_Encoder = 31.7 - 10*Vdac
     * Vdac = 3.17 - set_Voltage_Encoder*0.1   // set_Voltage_Encoder = valor de dac_12bits (0.0-30.0V);
     *
     * Vdac = dac_12bits*3.26/4096.0;
     * dac_12bits = Vdac*4096.0/3.26
     *
     * Formula para corregier la salida: (8.0E-06 * dac_12bits * dac_12bits + 0.002 * dac_12bits + 1.1844);
    */

    //Calculo el valor "dac_12bits"(0-4095) a partir del set_Voltage_Encoder(0-30V) y Vdac(0-3.3V)
    Vdac = 3.1677 - set_Voltage_Encoder*0.09825;//coloque R1=560k pero para mejorar los calculos utilizo el valor de 570k y obtuve esta formula
    dac_12bits = Vdac * 4096.0/3.26;//teniendo el Voltaje de salida del dac, calculo "dac_12bits" osea un valor de 0-4095
    dac_12bits = dac_12bits - (8.0E-06 * dac_12bits * dac_12bits + 0.002 * dac_12bits + 1.1844);//corrigo el valor del dac_12bits para acercarse lo mas posible al voltaje seteado(set_Voltage_Encoder)
}

void control_SW(void){
	//Control del pulsador del ENCODER (SW)
    if(HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == 0 && mem == 0){mem = 1;}
    if(HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == 1 && mem == 1){suma++; mem = 0;}
    if(suma==1){suma=2;paso_Encoder = 0.1;OLED_Print_Text_DMA(5,104,1,"0.1");}
    if(suma==3){suma=0;paso_Encoder = 1.0;OLED_Print_Text_DMA(5,104,1,"1.0");}
}

void control_SW1(void){
    //Control del pulsador SW1 - VOUT FIJOS 3.3, 5, 9, 12, 15 y 24
    //solo funciona cuando esta desactivada la salida de voltaje (powerSupply==0)
    if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 0 && mem1 == 0 && powerSupply==0){mem1 = 1;}
    if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 0 && mem1 == 1 && powerSupply==0){
  	  contSW1++;

  	  if(contSW1>25){//2.5seg aprox presionado
  		  HAL_TIM_Base_Stop_IT(&htim14);//desactivo la lectura del encoder
  		  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);
  		  OLED_Clear_DMA();
			  OLED_Print_Text_DMA(0,27,1," 1.8V 2500mA");
			  OLED_Print_Text_DMA(1,27,1," 3.3V 2250mA");
			  OLED_Print_Text_DMA(2,27,1," 5.0V 2000mA");
			  OLED_Print_Text_DMA(3,27,1," 9.0V 1250mA");
			  OLED_Print_Text_DMA(4,27,1,"12.0V 1000mA");
			  OLED_Print_Text_DMA(5,27,1,"15.0V  750mA");
			  OLED_Print_Text_DMA(6,27,1,"24.0V  500mA");
			  OLED_Print_Text_DMA(7,27,1,"30.0V  250mA");

			  mem1 = 0;
			  contSW1=0;
			  flagSW1 = 1;

  		  for(uint8_t i=0; i<3; i++){
  			  PWM_set_Freq_DutyCycle(2000,50,100);
  			  HAL_Delay(100);
  		  }

  		  HAL_Delay(1000);//tiempo para soltar el pulsador luego de entrar a esta pantalla o de escuchar el buzzer

			  while(flagSW1 == 1){
				  if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 0 && mem1 == 0){mem1 = 1;}
				  if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 1 && mem1 == 1 && contSW1<=10){sumaSW1++; mem1 = 0; contSW1=0;}
				  if(sumaSW1==1) {sumaSW1=0; OLED_Clear_DMA(); flagSW1=0;}
			  }
			  HAL_TIM_Base_Start_IT(&htim14);//activo nuevamente la lectura del encoder
			  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);
  	  }
    }

    if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 1 && mem1 == 1 && contSW1<=10){suma1++; mem1 = 0; contSW1=0;}//aqui no se si importa el powerSupply==0, pero funciona!

    //Valores de voltajes fijos establecidos
    if(suma1==1) {suma1=2; valor_Encoder = 3.3; }
    if(suma1==3) {suma1=4; valor_Encoder = 5.0; }
    if(suma1==5) {suma1=6; valor_Encoder = 9.0; }
    if(suma1==7) {suma1=8; valor_Encoder = 12.0;}
    if(suma1==9) {suma1=10;valor_Encoder = 15.0;}
    if(suma1==11){suma1=0; valor_Encoder = 24.0;}
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
