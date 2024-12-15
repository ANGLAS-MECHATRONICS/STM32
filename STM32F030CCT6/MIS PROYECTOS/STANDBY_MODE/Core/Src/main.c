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

/* USER CODE BEGIN PV */
uint8_t powerSupply=0;
uint8_t gpio=0;
uint32_t contBTN1=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
uint8_t control_BTN1(void);
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

  /******************** PARA USAR SOLO 1 SYS_WKUP **********************************/
  /*
  //PRESIONAR UNA VEZ Y LISTO (EL PIN PA0 PARECE QUE TIENE RESISTENCIA PULL DOWN, NO SE PUEDE COLCOAR DESDE EL .IOC)
  //verifica la bandera SB la cual es para saber si reanudo
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET){
	  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);//limpia la bandera

	  //blink led
	  for(uint8_t i=0; i<20; i++){
		  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		  HAL_Delay(150);
	  }

	  //desactivamos el WAKE UP pin
	  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  }

  //AHORA RECIEN VAMOS A INGRESAR AL STANDBY MODE
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  for(uint8_t i=0; i<5; i++){
	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	  HAL_Delay(2000);
  }

  //HABILITAMOS el WAKE UP pin
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

  //FINALMENTE INGRESAMOS AL STANDBY MODE
  HAL_PWR_EnterSTANDBYMode();
  */


  /******************** PARA USAR LOS DOS SYS_WKUP **********************************/

  //En este uC estos pines necesitan un pulso 3.3V para ingresar al modo STANDBY
  //es por eso que se coloca una resistencia pull down externa de 1k, no se puede colocar resistencias internas porque no hay esa opcion


  // Verifica si el sistema reanudó desde Standby Mode
  if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); // Limpia la bandera

      /*YA QUE CUALQUIER DE ELLOS SE PRESIONA, INMEDIATAMENTE DETECTA SI AUN ESTA PRESIONADO Y ENTRA CUALQUIERA DE LOS IF */

      //Aqui no debe haber nada, ningun codigo, inmediatamente debe pasar a los ifs para detectar el boton presionado

      //Esto hace cuando se presiona el pulsador del SYS_WKUP1
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 1) {//FUNCION 1
    	  //AQUI TODO EL CODIGO DE LA FUNCION 1
    	  if (HAL_GPIO_ReadPin(DRAIN_LATCH_GPIO_Port, DRAIN_LATCH_Pin) == 0){
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 1);
			  HAL_Delay(4000);//tiempo para apagar luego de encender o viceversa
    	  }else{
    		  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, 0);
			  HAL_Delay(4000);//tiempo para apagar luego de encender o viceversa
    	  }

      }

      //Esto hace cuando se presiona el pulsador del SYS_WKUP2
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == 1) {//FUNCION 2
		  for (uint8_t i = 0; i < 120; i++) {
			  HAL_GPIO_TogglePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin);
			  HAL_Delay(50);
		  }
      }

      HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);// Desactiva el Wake-Up Pin1
      HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);// Desactiva el Wake-Up Pin2
  }

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);//AHORA RECIEN VAMOS A INGRESAR AL STANDBY MODE
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);//habilitamos el Wake-Up Pin1
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);//habilitamos el Wake-Up Pin2
    HAL_PWR_EnterSTANDBYMode();//FINALMENTE INGRESAMOS AL STANDBY MODE


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  HAL_GPIO_WritePin(ON_OFF_3V7_GPIO_Port, ON_OFF_3V7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : ON_OFF_3V7_Pin */
  GPIO_InitStruct.Pin = ON_OFF_3V7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ON_OFF_3V7_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DRAIN_LATCH_Pin */
  GPIO_InitStruct.Pin = DRAIN_LATCH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DRAIN_LATCH_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
