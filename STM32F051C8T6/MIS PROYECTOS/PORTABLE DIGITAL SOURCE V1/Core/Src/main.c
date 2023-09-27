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
#include "string.h"
#include "math.h"

#include "Anglas_OLED_SSD1306.h"
#include "Anglas_IMAGENES.h"
#include "Anglas_ENCODER_ROT.h"
#include "Anglas_INA226.h"

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

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim6;

/* USER CODE BEGIN PV */
char buff[16];
float encoder;
float Vdac;
float power,current,voltage,shunt,Vshunt;
float Vbat;

float Vdac,VoutMath,difference,PWM,ENCO=0;
float VoutMathAnterior=0.0, VoutMathDiferenciaUP=0.0, VoutMathDiferenciaLOW=0.0, VoutMathCorregido=0.0;
const float Vref=1.25;
const float R1=560;//560k
const float R2=39;//39k
const float R3=56;//56k DAC

float adcVbat=0,VbatADC,previous;

uint16_t contMillis=0;
uint8_t muestras=50;
float rango=0.08;
float step=0.5;
uint16_t cont=0;
uint16_t contButton=0;
uint8_t memButton=0;

uint8_t mem=0,suma=0;//variables para el pulsador del encoder
float point=1820.02;

extern int16_t valor_Encoder;
extern int16_t paso_Encoder;

uint8_t powerSupply=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);
static void MX_DAC1_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */
void medirCorriente(void);
void medirPotencia(void);
void medirVoltBateria(void);
void Control_Estabilizar(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef struct GlobalData{
	unsigned char test[10];
	unsigned int data;
}GlobalData;

GlobalData data;
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
  MX_DAC1_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */


  //ee_writeToRam(0, sizeof(float), &point);
  //ee_commit();

  ee_read(0, sizeof(float), (uint8_t*)&valor_Encoder);//leo el valor de la eeprom

  HAL_TIM_Base_Start_IT(&htim6);
  OLED_Init();
  INA226_Init(3000,25,AVG_128,T_Vbus_588us,T_Vshunt_588us,MODE_SHUNT_BUS_CONTINUOUS);
  OLED_Print_Text(7,104,1,"0.1");//eliminar

  //OLED_Print_Text(2,104,1,"OFF");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//	  if(!HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin)){
//		  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);
//	  }else{
//		  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);
//	  }
//
//	  if(!HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin)){
//		  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
//	  }else{
//		  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
//	  }

	  ///////////////////////////// ICONO DE CARGA BATERIA////////////////////////////////////////////////////
	  //Cuando se conecta el cargador
	  if(HAL_GPIO_ReadPin(stateCharger_GPIO_Port, stateCharger_Pin)==1){
		  OLED_Print_Text(1,104,1,"ON ");
	  }else{
		  OLED_Print_Text(1,104,1,"OFF");
	  }
	  ///////////////////////////// BUTTON ENCODER - SW ///////////////////////////////////////////////////////
      //Lo que hace es lo mismo que un toggle, solo con mi logica de programacio y sin delay
      if(HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == 0 && mem == 0){
          mem = 1;}
      //Cuando se presiona solo una vez el contButton aumenta de 1 a 3 como maximo, por eso verifico esto
      if(HAL_GPIO_ReadPin(SW_GPIO_Port, SW_Pin) == 1 && mem == 1){
          suma = suma + 1;
          mem = 0;
      }

      if(suma==1){
          suma=2;  //cambio el contador para que solo haga una vez todo lo que esta dentro del if
          paso_Encoder = 120;
          OLED_Print_Text(7,104,1,"1.0");
      }

      if(suma==3){
          suma=0; //cambio el contador para que solo haga una vez todo lo que esta dentro del if
          paso_Encoder = 12;
          OLED_Print_Text(7,104,1,"0.1");
      }

      //Mantener presionado el SW por cierto tiempo
      if(HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == 0){
    	  contButton++;
    	  if(contButton>15){// 15seg aprox
    		  contButton=0;
    		  powerSupply++;
    	  }
      }
      //Garantiza que el contButton siempre inicie de 0
      if(HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == 1 && contButton>0){contButton=0;}

      if(powerSupply==1){
    	  powerSupply=2;
    	  OLED_Print_Text(6,104,1,"ON ");
    	  HAL_GPIO_WritePin(EN_XL6019_GPIO_Port, EN_XL6019_Pin, SET);
    	  ee_writeToRam(0, sizeof(float), (uint8_t*)&valor_Encoder);//escribo en al eeprom
    	  ee_commit();
      }

      if(powerSupply==3){
    	  powerSupply=0;
    	  OLED_Print_Text(6,104,1,"OFF");
    	  HAL_GPIO_WritePin(EN_XL6019_GPIO_Port, EN_XL6019_Pin, RESET);
    	  ENCO=0;//ver si eliminar esta linea
      }



      ///////////////////////////// MEDIR VOLTAJE-CORRIENTE-POTENCIA//////////////////////////////////////////
	  //Medimos el voltaje del dac para aplicar en la formula de VoutMath
      Vdac = encoder*3.26/4096.0;
      medirCorriente();
      //medirPotencia();

      //medimos el voltaje de salida
      voltage = INA226_Vbus();

      ///////////////////////////// MEDIR VOUTMATH ///////////////////////////////////////////////////////////
      /* La formula es la siguiente:
       * VoutMath = Vref+(((Vref/R2)+((Vref-Vdac)/R3))*R1);
       * Para  R1=560k, R2=39k, R3=56k y Vref=1.25, se reduce a:
       * VoutMath = 31.7 - 10*Vdac
       * Vdac = 3.17 - VoutMath*0.1
       *
       * Formula para corregier la salida: (0.0009*VoutMath*VoutMath - 0.0799*VoutMath + 1.5091)
      */

      VoutMath = 31.7 - 10*Vdac;
      VoutMath = VoutMath - (0.0009*VoutMath*VoutMath - 0.0799*VoutMath + 1.5091);//formula para corregir el Vouth (excel)
      VoutMath = (round(VoutMath * 100)/100);




      if(VoutMath > VoutMathAnterior){
    	  VoutMathAnterior = VoutMath - VoutMathAnterior;
    	  VoutMathDiferenciaUP = VoutMathAnterior;
    	  sprintf(buff,"%2.2f",VoutMathAnterior);
    	  OLED_Print_Text(3,104,1,buff);
      }

      if(VoutMath < VoutMathAnterior){
		  VoutMathAnterior = VoutMathAnterior - VoutMath;
		  VoutMathDiferenciaLOW = VoutMathAnterior;
		  sprintf(buff,"%2.2f",VoutMathAnterior);
		  OLED_Print_Text(3,104,1,buff);
		}
      VoutMathAnterior = VoutMath;

      if(VoutMathDiferenciaUP>0.09 && VoutMathDiferenciaUP<=0.100001){
    	  VoutMath = VoutMath + 0.00;
    	  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);
      }else if(VoutMathDiferenciaUP>0.100001 && VoutMathDiferenciaUP<=0.11){
    	  VoutMath = VoutMath - 0.01;
    	  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);
      }

      sprintf(buff,"SET:%2.2fV ",VoutMath);
      OLED_Print_Text(0,0,2,buff);







      sprintf(buff,"%4.0f",encoder);
      OLED_Print_Text(4,104,1,buff);

      sprintf(buff,"%2.2f",Vdac);
      OLED_Print_Text(5,104,1,buff);

      Control_Estabilizar();

      ///////////////////////////// MEDIR VBAT 12VDC - ADC ///////////////////////////////////////////////////////////
      //Leo 20 muestras del adc para tener una lectura mas precisa
	  HAL_ADC_Start(&hadc);
	  adcVbat=0;
      for(uint8_t i=0; i<muestras; i++) adcVbat += HAL_ADC_GetValue(&hadc);
      adcVbat /= muestras;
      Vbat = adcVbat*(3.38/4096.0)*4.309;//(3.3/4096.0)*4.26 ------ 4.26aprox sale del divisor resistivo V*10k/(10K+33K) -> V = 4.3

      ///////////////////////////// MILLIS ///////////////////////////////////////////////////////////
      if(contMillis>=90){//cada (Xms * 5) imprimo en el oled
    	  sprintf(buff,"%2.1fV",Vbat); OLED_Print_Text(0,96,1,buff);
		  sprintf(buff,"%2.1fV ",voltage); OLED_Print_Text(2,0,3,buff);
    	  contMillis=0;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
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
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
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
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
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
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
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
  htim6.Init.Prescaler = 48000-1;
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
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
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

  /*Configure GPIO pin : ALERT_Pin */
  GPIO_InitStruct.Pin = ALERT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ALERT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CLK_Pin DT_Pin SW_Pin */
  GPIO_InitStruct.Pin = CLK_Pin|DT_Pin|SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM6) {//Leemos el encoder cada 5ms
		encoder = Encoder_Run();
		contMillis++;
	}
}


void medirCorriente(void){
	current = INA226_Current();
	if(current>=0){
		sprintf(buff,"%4.0fmA",current);
		OLED_Print_Text(6,0,2,buff);
	}else{
		OLED_Print_Text(6,0,2,"   0mA");
	}
}

void medirPotencia(void){
	power = INA226_Power();
    if(power>=0){
	    sprintf(buff,"%2.2fW",power);
	    OLED_Print_Text(3,70,1,buff);
	    //OLED_Print_Text(2,70,2,buff);
    }else{
	    OLED_Print_Text(3,70,1,"0.00W ");
	    //OLED_Print_Text(2,70,2,"0.00W ");
    }
}

void medirVoltBateria(void){
	HAL_ADC_Start(&hadc);
    //adcSample = HAL_ADC_GetValue(&hadc1);
	Vbat = HAL_ADC_GetValue(&hadc)*(3.26/4096.0)*4.55;
	sprintf(buff,"VIN:%2.2f",Vbat);
	OLED_Print_Text(0,70,1,buff);
}

void Control_Estabilizar(void){

    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
    if(VoutMath > voltage){
    	difference = VoutMath - voltage;
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
    	PWM = encoder+ENCO;
    	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, PWM);
    }

    //if(VoutMath > voltage) ENCO=0;


    if(VoutMath < voltage){
    	difference = voltage - VoutMath;
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
    	PWM = encoder+ENCO;
    	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, PWM);
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
