/*
 * Anglas_WS281X.h
 *
 *  Created on: Jun 9, 2025
 *      Author: Giovanny Anglas
 */

#ifndef INC_ANGLAS_WS281X_H_
#define INC_ANGLAS_WS281X_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_ch1;
/* Exported constant ---------------------------------------------------------*/
#define MAX_LED 6
#define BRILLO_MAX  0
#define BRILLO_MIN 255
#define BRILLO_OFF 255

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void Set_LED(int LEDnum, int Red, int Green, int Blue);
void WS2811_Send(void);
void WS2811_Init(void);//inicia apagado todos los leds
void WS2812_Send(void);
void WS2812_Init(void);//inicia apagado todos los leds

uint8_t rainbow_effect_left(void);
uint8_t rainbow_effect_right(void);

#endif /* INC_ANGLAS_WS281X_H_ */
