/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LASER_GREEN_Pin GPIO_PIN_7
#define LASER_GREEN_GPIO_Port GPIOB
#define LASER_BLUE_Pin GPIO_PIN_14
#define LASER_BLUE_GPIO_Port GPIOC
#define BUTTON_Pin GPIO_PIN_0
#define BUTTON_GPIO_Port GPIOA
#define ON_OFF_3V7_2_Pin GPIO_PIN_1
#define ON_OFF_3V7_2_GPIO_Port GPIOA
#define IN_DRAIN_LATCH2_Pin GPIO_PIN_2
#define IN_DRAIN_LATCH2_GPIO_Port GPIOA
#define LASER_RED_Pin GPIO_PIN_4
#define LASER_RED_GPIO_Port GPIOA
#define A4988_SLEEP_Pin GPIO_PIN_5
#define A4988_SLEEP_GPIO_Port GPIOA
#define A4988_STEP_Pin GPIO_PIN_11
#define A4988_STEP_GPIO_Port GPIOA
#define A4988_DIR_Pin GPIO_PIN_12
#define A4988_DIR_GPIO_Port GPIOA
#define A4988_EN_Pin GPIO_PIN_3
#define A4988_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
