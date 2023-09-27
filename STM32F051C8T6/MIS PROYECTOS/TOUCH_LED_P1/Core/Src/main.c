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
#include "touchsensing.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define detect_type TSL_STATEID_TOUCH
#define release_type TSL_STATEID_RELEASE
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TSC_HandleTypeDef htsc;

/* USER CODE BEGIN PV */
tsl_user_status_t touch_status = TSL_USER_STATUS_BUSY;
uint8_t mem1=0,mem2=0,mem3=0;
uint8_t suma1=0,suma2=0,suma3=0;
uint8_t m1=0,m2=0,m3=0,m4=0,m5=0,m6=0,m7=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TSC_Init(void);
/* USER CODE BEGIN PFP */

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
  MX_TSC_Init();
  MX_TOUCHSENSING_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  touch_status = tsl_user_Exec();//esta linea es necesaria en todo

	  if((MyTKeys[0].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[0].p_Data->StateId == TSL_STATEID_DETECT) &&
		 (MyTKeys[1].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[1].p_Data->StateId == TSL_STATEID_PROX  ) &&
		 (MyTKeys[2].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[2].p_Data->StateId == TSL_STATEID_PROX  ) &&
		 !m1){
		  m1 = 1;
	  }

	  if((MyTKeys[0].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[0].p_Data->StateId == TSL_STATEID_DETECT) &&
		 (MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT) &&
		 (MyTKeys[2].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[2].p_Data->StateId == TSL_STATEID_PROX  ) &&
		 m1){
		  m2 = 1;
	  }

	  if((MyTKeys[0].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[0].p_Data->StateId == TSL_STATEID_PROX  ) &&
		 (MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT) &&
		 (MyTKeys[2].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[2].p_Data->StateId == TSL_STATEID_PROX  ) &&
		 m1 && m2){
		  m3 = 1;
	  }

	  if((MyTKeys[0].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[0].p_Data->StateId == TSL_STATEID_PROX  ) &&
		 (MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT) &&
		 (MyTKeys[2].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[2].p_Data->StateId == TSL_STATEID_DETECT) &&
		 m1 && m2 && m3){
		  m4 = 1;
	  }

	  if((MyTKeys[0].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[0].p_Data->StateId == TSL_STATEID_PROX  ) &&
		 (MyTKeys[1].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[1].p_Data->StateId == TSL_STATEID_PROX  ) &&
		 (MyTKeys[2].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[2].p_Data->StateId == TSL_STATEID_DETECT) &&
		 m1 && m2 && m3 && m4){
		  m5 = 1;
	  }


	  if((MyTKeys[0].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[0].p_Data->StateId == TSL_STATEID_PROX) &&
		 (MyTKeys[1].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[1].p_Data->StateId == TSL_STATEID_PROX) &&
		 (MyTKeys[2].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[2].p_Data->StateId == TSL_STATEID_PROX) &&
		 m1 && m2 && m3 && m4 && m5){
		  m6 = 1;
	  }


	  if(m1 && m2 && m3 && m4 && m5 && m6){
		  m7=!m7;
		  m1=0;m2=0;m3=0;m4=0;m5=0;m6=0;
	  }

	  if(m7){
		  HAL_GPIO_WritePin(led4_GPIO_Port, led4_Pin, SET);
	  }else{
		  HAL_GPIO_WritePin(led4_GPIO_Port, led4_Pin, RESET);
	  }





	  //Para encender y apagar tocando 1 vez
	  if((MyTKeys[0].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[0].p_Data->StateId == TSL_STATEID_DETECT)  && !mem1){mem1 = 1;}
	  if((MyTKeys[0].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[0].p_Data->StateId == TSL_STATEID_PROX  )  &&  mem1){suma1++;mem1=0;}

	  if((MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT)  && !mem2){mem2 = 1;}
	  if((MyTKeys[1].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[1].p_Data->StateId == TSL_STATEID_PROX  )  &&  mem2){suma2++;mem2=0;}

	  if((MyTKeys[2].p_Data->StateId == TSL_STATEID_DETECT  || MyTKeys[2].p_Data->StateId == TSL_STATEID_DETECT)  && !mem3){mem3 = 1;}
	  if((MyTKeys[2].p_Data->StateId == TSL_STATEID_RELEASE || MyTKeys[2].p_Data->StateId == TSL_STATEID_PROX  )  &&  mem3){suma3++;mem3=0;}


	  if(suma1%2) HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, SET);
	  else		  HAL_GPIO_WritePin(led1_GPIO_Port, led1_Pin, RESET);

	  if(suma2%2) HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, SET);
	  else		  HAL_GPIO_WritePin(led2_GPIO_Port, led2_Pin, RESET);

	  if(suma3%2) HAL_GPIO_WritePin(led3_GPIO_Port, led3_Pin, SET);
	  else		  HAL_GPIO_WritePin(led3_GPIO_Port, led3_Pin, RESET);







//    //PRUEBA 1 ENCENDER APAGAR
//	  if(MyTKeys[0].p_Data->StateId == TSL_STATEID_DETECT){
//		  HAL_GPIO_WritePin(ledBoard_GPIO_Port, ledBoard_Pin, SET);
//	  }
//
//	  if(MyTKeys[1].p_Data->StateId == TSL_STATEID_DETECT){
//	  	  HAL_GPIO_WritePin(ledBoard_GPIO_Port, ledBoard_Pin, RESET);
//	  }

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
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
}

/**
  * @brief TSC Initialization Function
  * @param None
  * @retval None
  */
static void MX_TSC_Init(void)
{

  /* USER CODE BEGIN TSC_Init 0 */

  /* USER CODE END TSC_Init 0 */

  /* USER CODE BEGIN TSC_Init 1 */

  /* USER CODE END TSC_Init 1 */

  /** Configure the TSC peripheral
  */
  htsc.Instance = TSC;
  htsc.Init.CTPulseHighLength = TSC_CTPH_2CYCLES;
  htsc.Init.CTPulseLowLength = TSC_CTPL_2CYCLES;
  htsc.Init.SpreadSpectrum = DISABLE;
  htsc.Init.SpreadSpectrumDeviation = 1;
  htsc.Init.SpreadSpectrumPrescaler = TSC_SS_PRESC_DIV1;
  htsc.Init.PulseGeneratorPrescaler = TSC_PG_PRESC_DIV4;
  htsc.Init.MaxCountValue = TSC_MCV_8191;
  htsc.Init.IODefaultMode = TSC_IODEF_OUT_PP_LOW;
  htsc.Init.SynchroPinPolarity = TSC_SYNC_POLARITY_FALLING;
  htsc.Init.AcquisitionMode = TSC_ACQ_MODE_NORMAL;
  htsc.Init.MaxCountInterrupt = DISABLE;
  htsc.Init.ChannelIOs = TSC_GROUP6_IO2|TSC_GROUP6_IO3|TSC_GROUP6_IO4;
  htsc.Init.ShieldIOs = 0;
  htsc.Init.SamplingIOs = TSC_GROUP6_IO1;
  if (HAL_TSC_Init(&htsc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TSC_Init 2 */

  /* USER CODE END TSC_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, led1_Pin|led2_Pin|led3_Pin|led4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : led1_Pin led2_Pin led3_Pin led4_Pin */
  GPIO_InitStruct.Pin = led1_Pin|led2_Pin|led3_Pin|led4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
