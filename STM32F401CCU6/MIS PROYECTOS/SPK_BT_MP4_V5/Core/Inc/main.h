/* USER CODE BEGIN Header */
/*
  * Author: VadRov
  * Copyright (C) 2022, VadRov, all rights reserved.
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
  *
 *  https://www.youtube.com/c/VadRov
 *  https://zen.yandex.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LIGHTER_Pin LL_GPIO_PIN_13
#define LIGHTER_GPIO_Port GPIOC
#define A4988_EN_2_Pin LL_GPIO_PIN_1
#define A4988_EN_2_GPIO_Port GPIOA
#define LCD_DC_Pin LL_GPIO_PIN_2
#define LCD_DC_GPIO_Port GPIOA
#define LCD_RES_Pin LL_GPIO_PIN_3
#define LCD_RES_GPIO_Port GPIOA
#define LCD_CS_Pin LL_GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOA
#define LCD_SCL_Pin LL_GPIO_PIN_5
#define LCD_SCL_GPIO_Port GPIOA
#define LCD_BLK_Pin LL_GPIO_PIN_6
#define LCD_BLK_GPIO_Port GPIOA
#define LCD_SDA_Pin LL_GPIO_PIN_7
#define LCD_SDA_GPIO_Port GPIOA
#define ON_OFF_3V7_1_Pin LL_GPIO_PIN_0
#define ON_OFF_3V7_1_GPIO_Port GPIOB
#define IN_DRAIN_LATCH1_Pin LL_GPIO_PIN_1
#define IN_DRAIN_LATCH1_GPIO_Port GPIOB
#define A4988_SLEEP_2_Pin LL_GPIO_PIN_2
#define A4988_SLEEP_2_GPIO_Port GPIOB
#define IN_TS3A5018_Pin LL_GPIO_PIN_10
#define IN_TS3A5018_GPIO_Port GPIOB
#define SD_CS_Pin LL_GPIO_PIN_12
#define SD_CS_GPIO_Port GPIOB
#define SD_CLK_Pin LL_GPIO_PIN_13
#define SD_CLK_GPIO_Port GPIOB
#define SD_MISO_Pin LL_GPIO_PIN_14
#define SD_MISO_GPIO_Port GPIOB
#define SD_MOSI_Pin LL_GPIO_PIN_15
#define SD_MOSI_GPIO_Port GPIOB
#define A4988_STEP_2_Pin LL_GPIO_PIN_11
#define A4988_STEP_2_GPIO_Port GPIOA
#define A4988_DIR_2_Pin LL_GPIO_PIN_12
#define A4988_DIR_2_GPIO_Port GPIOA
#define EN_TS3A5018_Pin LL_GPIO_PIN_6
#define EN_TS3A5018_GPIO_Port GPIOB
#define KEY_RIGHT_Pin LL_GPIO_PIN_7
#define KEY_RIGHT_GPIO_Port GPIOB
#define KEY_DOWN_Pin LL_GPIO_PIN_8
#define KEY_DOWN_GPIO_Port GPIOB
#define KEY_UP_Pin LL_GPIO_PIN_9
#define KEY_UP_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
