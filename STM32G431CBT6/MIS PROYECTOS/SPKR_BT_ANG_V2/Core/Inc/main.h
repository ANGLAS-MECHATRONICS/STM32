/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SW3_Pin GPIO_PIN_1
#define SW3_GPIO_Port GPIOA
#define IN_TS3A5018_Pin GPIO_PIN_3
#define IN_TS3A5018_GPIO_Port GPIOA
#define BLK_Pin GPIO_PIN_6
#define BLK_GPIO_Port GPIOA
#define DC_Pin GPIO_PIN_0
#define DC_GPIO_Port GPIOB
#define RST_Pin GPIO_PIN_1
#define RST_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_2
#define CS_GPIO_Port GPIOB
#define ALERT_INA_Pin GPIO_PIN_10
#define ALERT_INA_GPIO_Port GPIOB
#define LINK_Pin GPIO_PIN_11
#define LINK_GPIO_Port GPIOB
#define CS_SD_Pin GPIO_PIN_12
#define CS_SD_GPIO_Port GPIOB
#define IN_CHARGER_Pin GPIO_PIN_8
#define IN_CHARGER_GPIO_Port GPIOA
#define STDBY_Pin GPIO_PIN_11
#define STDBY_GPIO_Port GPIOA
#define CHRGB_Pin GPIO_PIN_12
#define CHRGB_GPIO_Port GPIOA
#define IN_DRAIN_LATCH_Pin GPIO_PIN_6
#define IN_DRAIN_LATCH_GPIO_Port GPIOB
#define ON_OFF_3V7_Pin GPIO_PIN_7
#define ON_OFF_3V7_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
