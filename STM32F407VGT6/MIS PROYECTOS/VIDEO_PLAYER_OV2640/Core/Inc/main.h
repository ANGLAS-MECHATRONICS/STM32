/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_gpio.h"

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
#define PWDN_CAM_Pin LL_GPIO_PIN_2
#define PWDN_CAM_GPIO_Port GPIOE
#define RST_CAM_Pin LL_GPIO_PIN_3
#define RST_CAM_GPIO_Port GPIOE
#define LCD_SCL_Pin LL_GPIO_PIN_5
#define LCD_SCL_GPIO_Port GPIOA
#define LCD_SDA_Pin LL_GPIO_PIN_7
#define LCD_SDA_GPIO_Port GPIOA
#define LCD_RES_Pin LL_GPIO_PIN_4
#define LCD_RES_GPIO_Port GPIOC
#define LCD_DC_Pin LL_GPIO_PIN_5
#define LCD_DC_GPIO_Port GPIOC
#define LCD_CS_Pin LL_GPIO_PIN_0
#define LCD_CS_GPIO_Port GPIOB
#define KEY_UP_Pin LL_GPIO_PIN_13
#define KEY_UP_GPIO_Port GPIOE
#define KEY_DOWN_Pin LL_GPIO_PIN_14
#define KEY_DOWN_GPIO_Port GPIOE
#define KEY_RIGHT_Pin LL_GPIO_PIN_15
#define KEY_RIGHT_GPIO_Port GPIOE
#define SD_CS_Pin LL_GPIO_PIN_12
#define SD_CS_GPIO_Port GPIOB
#define SD_CLK_Pin LL_GPIO_PIN_13
#define SD_CLK_GPIO_Port GPIOB
#define SD_MISO_Pin LL_GPIO_PIN_14
#define SD_MISO_GPIO_Port GPIOB
#define SD_MOSI_Pin LL_GPIO_PIN_15
#define SD_MOSI_GPIO_Port GPIOB
#define LCD_BLK_Pin LL_GPIO_PIN_6
#define LCD_BLK_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
