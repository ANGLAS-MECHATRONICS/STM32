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
#define L1(state)  HAL_GPIO_WritePin(LED1_GPIO_Port,  LED1_Pin,  state)
#define L2(state)  HAL_GPIO_WritePin(LED2_GPIO_Port,  LED2_Pin,  state)
#define L3(state)  HAL_GPIO_WritePin(LED3_GPIO_Port,  LED3_Pin,  state)
#define L4(state)  HAL_GPIO_WritePin(LED4_GPIO_Port,  LED4_Pin,  state)
#define L5(state)  HAL_GPIO_WritePin(LED5_GPIO_Port,  LED5_Pin,  state)
#define L6(state)  HAL_GPIO_WritePin(LED6_GPIO_Port,  LED6_Pin,  state)
#define L7(state)  HAL_GPIO_WritePin(LED7_GPIO_Port,  LED7_Pin,  state)
#define L8(state)  HAL_GPIO_WritePin(LED8_GPIO_Port,  LED8_Pin,  state)
#define L9(state)  HAL_GPIO_WritePin(LED9_GPIO_Port,  LED9_Pin,  state)
#define L10(state) HAL_GPIO_WritePin(LED10_GPIO_Port, LED10_Pin, state)
#define L11(state) HAL_GPIO_WritePin(LED11_GPIO_Port, LED11_Pin, state)
#define L12(state) HAL_GPIO_WritePin(LED12_GPIO_Port, LED12_Pin, state)
#define L13(state) HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, state)
#define L14(state) HAL_GPIO_WritePin(LED14_GPIO_Port, LED14_Pin, state)
#define L15(state) HAL_GPIO_WritePin(LED15_GPIO_Port, LED15_Pin, state)
#define L16(state) HAL_GPIO_WritePin(LED16_GPIO_Port, LED16_Pin, state)
#define L17(state) HAL_GPIO_WritePin(LED17_GPIO_Port, LED17_Pin, state)
#define L18(state) HAL_GPIO_WritePin(LED18_GPIO_Port, LED18_Pin, state)
#define L19(state) HAL_GPIO_WritePin(LED19_GPIO_Port, LED19_Pin, state)
#define L20(state) HAL_GPIO_WritePin(LED20_GPIO_Port, LED20_Pin, state)
#define L21(state) HAL_GPIO_WritePin(LED21_GPIO_Port, LED21_Pin, state)
#define L22(state) HAL_GPIO_WritePin(LED22_GPIO_Port, LED22_Pin, state)
#define L23(state) HAL_GPIO_WritePin(LED23_GPIO_Port, LED23_Pin, state)
#define L24(state) HAL_GPIO_WritePin(LED24_GPIO_Port, LED24_Pin, state)
#define L25(state) HAL_GPIO_WritePin(LED25_GPIO_Port, LED25_Pin, state)
//#define L26(state) HAL_GPIO_WritePin(LED26_GPIO_Port, LED26_Pin, state)
//#define L27(state) HAL_GPIO_WritePin(LED27_GPIO_Port, LED27_Pin, state)

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

/* USER CODE BEGIN PV */
//falta corregir para cantLeds par y cantLeds2 impar o algo asi
uint8_t cantLeds=11;//creo que agregar o quitar en  ControlLeds
uint8_t cantLeds2=8;//creo que agregar o quitar en  ControlLeds2
uint16_t adc;
static float vel;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
float Refresh_ADC_Value(void);

void ControlLeds(uint8_t led, uint8_t state);
void ControlLeds2(uint8_t led, uint8_t state);
void Barrido1(uint8_t veces, uint16_t tiempo, uint16_t tiempo2);
void Interlude(uint8_t veces, uint16_t tiempo);
void Encender_1_Led(uint8_t num_led);
void Encender_1_Led_2(uint8_t num_led);
void Encender_1_Led_2_1(uint8_t num_led);
void Encender_1_Led_3(uint8_t num_led);
void Encender_1_Led_4(uint8_t num_led, uint8_t indice);
void Encender_1_Led_4_1(uint8_t num_led, uint8_t indice);
void AbrirApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2);
void CerrarApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2);
void Girar(uint8_t veces, uint16_t tiempo);
void VolumenAbrir(uint8_t veces, uint16_t tiempo, uint16_t tiempo2);
void VolumenAbrir2(uint8_t veces, uint16_t tiempo, uint16_t tiempo2);
void VolumenCerrar(uint8_t veces, uint16_t tiempo);
void ArmarIzquierda(uint8_t veces, uint16_t tiempo, uint16_t tiempo2);
void ArmarIzquierda2(uint8_t veces, uint16_t tiempo, uint16_t tiempo2);
void Blink(uint8_t veces, uint16_t tiempo);

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
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADC_Start_IT(&hadc1);
  /* USER CODE END 2 */
  HAL_Delay(3000);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
	  Barrido1(2,60,2500);
	  Blink(2,1500);

	  for(uint8_t i=1;i<=cantLeds;i++){ControlLeds(i,1);}//encender todo 1
	  ArmarIzquierda2(1,30,2000);
	  for(uint8_t i=1;i<=cantLeds;i++){ControlLeds2(i+cantLeds,1);}//encender todo 2
	  ArmarIzquierda(1,30,2000);

	  Blink(2,1500);
	  Girar(2,100);
	  Blink(2,2000);


	  for(uint8_t i=1;i<=cantLeds;i++){ControlLeds(i,1);}//encender todo 1
	  VolumenAbrir2(1,30,2500);
	  for(uint8_t i=1;i<=cantLeds;i++){ControlLeds2(i+cantLeds,1);}//encender todo 2
	  VolumenAbrir(1,30,2500);

	  CerrarApagar(3,150,2000);
	  Blink(2,1500);


	  for(uint8_t i=1;i<=cantLeds;i++){ControlLeds(i,1);}//encender todo 1
	  ArmarIzquierda2(1,30,2000);
	  for(uint8_t i=1;i<=cantLeds;i++){ControlLeds2(i+cantLeds,1);}//encender todo 2
	  ArmarIzquierda(1,30,2000);

	  Blink(3,1500);
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
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_160CYCLES_5;
  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED1_Pin|LED12_Pin|LED13_Pin|LED14_Pin
                          |LED22_Pin|LED23_Pin|LED24_Pin|LED25_Pin
                          |ledBoard_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LED2_Pin|LED3_Pin|LED17_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED4_Pin|LED5_Pin|LED6_Pin|LED7_Pin
                          |LED8_Pin|LED9_Pin|LED10_Pin|LED11_Pin
                          |LED15_Pin|LED16_Pin|LED18_Pin|LED19_Pin
                          |LED20_Pin|LED21_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED1_Pin LED12_Pin LED13_Pin LED14_Pin
                           LED22_Pin LED23_Pin LED24_Pin LED25_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED12_Pin|LED13_Pin|LED14_Pin
                          |LED22_Pin|LED23_Pin|LED24_Pin|LED25_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LED2_Pin LED3_Pin LED17_Pin */
  GPIO_InitStruct.Pin = LED2_Pin|LED3_Pin|LED17_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED4_Pin LED5_Pin LED6_Pin LED7_Pin
                           LED8_Pin LED9_Pin LED10_Pin LED11_Pin
                           LED15_Pin LED16_Pin LED18_Pin LED19_Pin
                           LED20_Pin LED21_Pin */
  GPIO_InitStruct.Pin = LED4_Pin|LED5_Pin|LED6_Pin|LED7_Pin
                          |LED8_Pin|LED9_Pin|LED10_Pin|LED11_Pin
                          |LED15_Pin|LED16_Pin|LED18_Pin|LED19_Pin
                          |LED20_Pin|LED21_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ledBoard_Pin */
  GPIO_InitStruct.Pin = ledBoard_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ledBoard_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	adc = HAL_ADC_GetValue(&hadc1);
	vel = (float)adc/1023;
	HAL_ADC_Start_IT(&hadc1);
}

float Refresh_ADC_Value(void){
	adc = HAL_ADC_GetValue(&hadc1);
	vel = (float)adc/1023;
	HAL_ADC_Start_IT(&hadc1);
	return vel;
}

void ControlLeds(uint8_t led, uint8_t state){
	switch(led){
		case 1:  L1(state); break;
		case 2:  L2(state); break;
		case 3:  L3(state); break;
		case 4:  L4(state); break;
		case 5:  L5(state); break;
		case 6:  L6(state); break;
		case 7:  L7(state); break;
		case 8:  L8(state); break;
		case 9:  L9(state); break;
		case 10: L10(state);break;
		case 11: L11(state);break;
		default:break;
	}
}

void ControlLeds2(uint8_t led, uint8_t state){
	switch(led){
		case 12: L12(state);break;
		case 13: L13(state);break;
		case 14: L14(state);break;
		case 15: L15(state);break;
		case 16: L16(state);break;
		case 17: L17(state);break;
		case 18: L18(state);break;
		case 19: L19(state);break;
	}
}

void Barrido1(uint8_t veces, uint16_t tiempo, uint16_t tiempo2){
	for(uint8_t v=0;v<veces;v++){
		for(uint8_t i=1; i<=cantLeds; i++){ControlLeds(i,1);ControlLeds2(i+cantLeds,1);HAL_Delay(tiempo*Refresh_ADC_Value());}
		HAL_Delay(tiempo2*Refresh_ADC_Value());
		for(uint8_t i=1; i<=cantLeds; i++){ControlLeds(i,0);ControlLeds2(i+cantLeds,0);HAL_Delay(tiempo*Refresh_ADC_Value());}
	}
}

void Encender_1_Led(uint8_t num_led){
	for(uint8_t i=1; i<=cantLeds; i++){
		if(i == num_led){
			ControlLeds(i,1);
		}else{
			ControlLeds(i,0);
		}
	}
}

void Encender_1_Led_2(uint8_t num_led){
	for(uint8_t i=1; i<=cantLeds; i++){
		if(i<=num_led){
			ControlLeds(i,1);
		}else{
			ControlLeds(i,0);
		}
	}
}

void Encender_1_Led_2_1(uint8_t num_led){
	for(uint8_t i=1; i<=cantLeds; i++){
		if(i<=num_led){
			ControlLeds2(i+cantLeds,1);
		}else{
			ControlLeds2(i+cantLeds,0);
		}
	}
}

void Encender_1_Led_3(uint8_t num_led){
	for(uint8_t i=1; i<=cantLeds; i++){
		if(i>=num_led){
			ControlLeds(i,1);
		}else{
			ControlLeds(i,0);
		}
	}
}

void Encender_1_Led_4(uint8_t num_led, uint8_t indice){
	for(uint8_t i=cantLeds;i>=indice;i--){
		if(i == num_led){
			ControlLeds(i,1);
		}else{
			ControlLeds(i,0);
		}
	}
}

void Encender_1_Led_4_1(uint8_t num_led, uint8_t indice){
	for(uint8_t i=cantLeds;i>=indice;i--){
		if(i == num_led){
			ControlLeds2(i+cantLeds,1);
		}else{
			ControlLeds2(i+cantLeds,0);
		}
	}
}

void Interlude(uint8_t veces, uint16_t tiempo){
	for(uint8_t v=0;v<veces;v++){
		for(uint8_t i=1;i<=cantLeds;i++){
			Encender_1_Led(i);
			HAL_Delay(tiempo*Refresh_ADC_Value());
		}
		for(uint8_t i=cantLeds;i>=1;i--){
			Encender_1_Led(i);
			HAL_Delay(tiempo*Refresh_ADC_Value());
		}
	}
}

void AbrirApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2){

	for(uint8_t v=0;v<veces;v++){
		uint8_t centro,centro2,impar=1,par=0;
		if(cantLeds%2==0){//si es par
			centro = cantLeds/2;
			for(uint8_t i=centro;i>=1;i--){
				ControlLeds(i, 1);
				ControlLeds(i+impar, 1);
				impar=impar+2;
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}

			impar=1;
			HAL_Delay(tiempo2*Refresh_ADC_Value());

			for(uint8_t i=centro;i>=1;i--){
				ControlLeds(i, 0);
				ControlLeds(i+impar, 0);
				impar=impar+2;
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}

		}else{//si es impar (aqui edito porq cantLeds=15)
			centro = (cantLeds/2)+1;//7
			centro2 = (cantLeds2/2);//3
			for(uint8_t i=centro;i>=1;i--){
				//ControlLeds(i-(centro-centro2), 1);ControlLeds2(i+cantLeds,1);
				//ControlLeds(i+par-(centro-centro2-1), 1);ControlLeds2(i+cantLeds+par,1);
				ControlLeds2(i-(centro-centro2)+cantLeds, 1);      ControlLeds(i,1);
				ControlLeds2(i+par-(centro-centro2-1)+cantLeds, 1);ControlLeds(i+par,1);
				par=par+2;
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}

			par=0;
			HAL_Delay(tiempo2*Refresh_ADC_Value());

			for(uint8_t i=centro;i>=1;i--){
				//ControlLeds(i-(centro-centro2), 0);ControlLeds2(i+cantLeds,0);
				//ControlLeds(i+par-(centro-centro2-1), 0);ControlLeds2(i+cantLeds+par,0);
				ControlLeds2(i-(centro-centro2)+cantLeds, 0);      ControlLeds(i,0);
				ControlLeds2(i+par-(centro-centro2-1)+cantLeds, 0);ControlLeds(i+par,0);
				par=par+2;
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
		}
	}
}

void CerrarApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2){
	for(uint8_t v=0;v<veces;v++){
		uint8_t limite,limite2,centro;
		if(cantLeds%2==0){//si es par

			centro = cantLeds/2;//5
			limite = cantLeds-1;
			for(uint8_t i=1;i<=centro;i++){
				ControlLeds(i, 1);
				ControlLeds(i+limite, 1);
				limite=limite-2;
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
			limite = cantLeds-1;
			HAL_Delay(tiempo2*Refresh_ADC_Value());
			for(uint8_t i=1;i<=centro;i++){
				ControlLeds(i, 0);
				ControlLeds(i+limite, 0);
				limite=limite-2;
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}

		}else{//si es impar (ya esta listo para impar y cualquier cantidad de letras)
			centro = (cantLeds/2)+1;//7
			limite = cantLeds-1;limite2 = cantLeds2-1;
			for(uint8_t i=1;i<=centro;i++){
				ControlLeds(i, 1);ControlLeds2(i+cantLeds,1);
				ControlLeds(i+limite, 1);ControlLeds2(i+cantLeds+limite2,1);
				limite=limite-2;limite2=limite2-2;
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
			limite = cantLeds-1;limite2 = cantLeds2-1;
			HAL_Delay(tiempo2*Refresh_ADC_Value());
			for(uint8_t i=1;i<=centro;i++){
				ControlLeds(i, 0);ControlLeds2(i+cantLeds,0);
				ControlLeds(i+limite, 0);ControlLeds2(i+cantLeds+limite2,0);
				limite=limite-2;limite2=limite2-2;
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
		}
	}
}

void Girar(uint8_t veces, uint16_t tiempo){
	for(uint8_t v=0;v<veces;v++){
		AbrirApagar(1,tiempo*Refresh_ADC_Value(),0);
		CerrarApagar(1,tiempo*Refresh_ADC_Value(),0);
	}
}

void VolumenAbrir(uint8_t veces, uint16_t tiempo, uint16_t tiempo2){
	for(uint8_t v=0;v<veces;v++){
		uint8_t l;
		for(uint8_t i=1;i<=cantLeds;i++){
			l=i;
			HAL_Delay(tiempo*Refresh_ADC_Value());
			for(uint8_t i=l;i<=cantLeds;i++){
				Encender_1_Led_2(i);
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
			for(uint8_t i=cantLeds;i>=l;i--){
				Encender_1_Led_2(i);
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
		}
		HAL_Delay(tiempo2*Refresh_ADC_Value());
	}
}

void VolumenAbrir2(uint8_t veces, uint16_t tiempo, uint16_t tiempo2){
	for(uint8_t v=0;v<veces;v++){
		uint8_t l;
		for(uint8_t i=1;i<=cantLeds;i++){
			l=i;
			HAL_Delay(tiempo*Refresh_ADC_Value());
			for(uint8_t i=l;i<=cantLeds;i++){
				Encender_1_Led_2_1(i);
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
			for(uint8_t i=cantLeds;i>=l;i--){
				Encender_1_Led_2_1(i);
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
		}
		HAL_Delay(tiempo2*Refresh_ADC_Value());
	}
}

void VolumenCerrar(uint8_t veces, uint16_t tiempo){
	for(uint8_t v=0;v<veces;v++){
		uint8_t l;
		for(uint8_t i=1;i<=cantLeds;i++){
			l=i;
			HAL_Delay(tiempo*Refresh_ADC_Value());
			for(uint8_t i=l;i<=cantLeds;i++){
				Encender_1_Led_3(i);
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
			for(uint8_t i=cantLeds;i>=l;i--){
				Encender_1_Led_3(i);
				HAL_Delay(tiempo*Refresh_ADC_Value());
			}
		}
	}
}

void ArmarIzquierda(uint8_t veces, uint16_t tiempo, uint16_t tiempo2){
	for(uint8_t v=0;v<veces;v++){

		uint8_t l=1;
		for(uint8_t i=cantLeds;i>=1;i--){
			for(uint8_t i=cantLeds;i>=l;i--){
				Encender_1_Led_4(i,l);
				if(l<cantLeds/2){
					HAL_Delay(tiempo*Refresh_ADC_Value());
				}else if(l>=cantLeds-2){
					HAL_Delay(tiempo*Refresh_ADC_Value()*4);
				}else{
					HAL_Delay(tiempo*Refresh_ADC_Value()*2);
				}
				//HAL_Delay(tiempo);
			}

			if(l>=cantLeds){
				l=1;
			}else{
				ControlLeds(l, 1);
				l++;
			}
		}

		HAL_Delay(tiempo2*Refresh_ADC_Value());
	}
}

void ArmarIzquierda2(uint8_t veces, uint16_t tiempo, uint16_t tiempo2){
	for(uint8_t v=0;v<veces;v++){

		uint8_t l=1;
		for(uint8_t i=cantLeds;i>=1;i--){
			for(uint8_t i=cantLeds;i>=l;i--){
				Encender_1_Led_4_1(i,l);
				if(l<cantLeds/2){
					HAL_Delay(tiempo*Refresh_ADC_Value());
				}else if(l>=cantLeds-2){
					HAL_Delay(tiempo*Refresh_ADC_Value()*4);
				}else{
					HAL_Delay(tiempo*Refresh_ADC_Value()*2);
				}
				//HAL_Delay(tiempo);
			}

			if(l>=cantLeds){
				l=1;
			}else{
				ControlLeds2(l+cantLeds, 1);
				l++;
			}
		}

		HAL_Delay(tiempo2*Refresh_ADC_Value());
	}
}

void Blink(uint8_t veces, uint16_t tiempo){
	for(uint8_t v=0;v<veces;v++){
		for(uint8_t i=1;i<=cantLeds;i++){
			ControlLeds(i, 1);ControlLeds2(i+cantLeds,1);
		}
		HAL_Delay(tiempo*Refresh_ADC_Value());
		for(uint8_t i=1;i<=cantLeds;i++){
			ControlLeds(i, 0);ControlLeds2(i+cantLeds,0);
		}
		HAL_Delay(tiempo*Refresh_ADC_Value()/5);
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
