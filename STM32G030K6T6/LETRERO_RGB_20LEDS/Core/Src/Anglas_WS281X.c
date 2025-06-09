/*
 * Anglas_WS281X.c
 *
 *  Created on: Jun 9, 2025
 *      Author: Giovanny Anglas
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Anglas_WS281X.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
uint8_t LED_Data[MAX_LED][4];//para leds
uint8_t LED_Mod[MAX_LED][4];//para brillo
uint8_t datasentflag = 0;
uint16_t pwmData[(24*MAX_LED)+240];//para ws2811
//uint16_t pwmData[(24*MAX_LED)+50];//para ws2812
//la condicion de RESET en WS2811 me pide >280uS y como mi señal es de 800kHz = 1.25uS
//le doy 240 x 1.25uS = 300uS (en el analizador logico me sale 305uS , asi que todo bien)
uint16_t effStep = 0;//variable solo para los efectos

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	datasentflag=1;
}

void Set_LED(int LEDnum, int Red, int Green, int Blue){
	LED_Data[LEDnum][0] = LEDnum;
	LED_Data[LEDnum][1] = Red;
	LED_Data[LEDnum][2] = Green;
	LED_Data[LEDnum][3] = Blue;
}

void WS2811_Send(void){
	uint32_t indx=0;
	uint32_t color;

	for(int i= 0; i<MAX_LED; i++){
		color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));
		for(int i=23; i>=0; i--){
			if (color&(1<<i)) pwmData[indx] = 56;// 875ns es el 70% -> 80 * 70%
			else 			  pwmData[indx] = 24;// 375ns es el 30% -> 80 * 30%
			indx++;
		}
	}

	for(int i=0; i<240; i++){
		pwmData[indx] = 0;
		indx++;
	}

	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
	while (!datasentflag){};
	datasentflag = 0;
}

void WS2811_Init(void){//inicia apagado todos los leds
  for(int i=0; i<MAX_LED; i++) Set_LED(i, 255, 255, 255);
  WS2811_Send();
  HAL_Delay(10);//bajar el tiempo si es necesario

}


void WS2812_Send(void){
	uint32_t indx=0;
	uint32_t color;

	for(int i= 0; i<MAX_LED; i++){
		color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));
		for(int i=23; i>=0; i--){
			if (color&(1<<i)) pwmData[indx] = 56;// 875ns es el 70% -> 80 * 70%
			else 			  pwmData[indx] = 24;// 375ns es el 30% -> 80 * 30%
			indx++;
		}
	}

	for(int i=0; i<50; i++){
		pwmData[indx] = 0;
		indx++;
	}

	HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
	while (!datasentflag){};
	datasentflag = 0;
}

void WS2812_Init(void){//inicia apagado todos los leds
  for(int i=0; i<MAX_LED; i++) Set_LED(i, BRILLO_OFF, BRILLO_OFF, BRILLO_OFF);
  WS2812_Send();
  HAL_Delay(2);//bajar el tiempo si es necesario

}

///////////////////////////////// EFECTOS //////////////////////////////////////////////////


uint8_t rainbow_effect_left(void) {
    // Strip ID: 0 - Effect: Rainbow - LEDS: 8
    // Steps: 13 - Delay: 54
    // Colors: 3 (255.0.0, 0.255.0, 0.0.255)
    // Options: rainbowlen=8, toLeft=true,
//  if(millis() - strip_0.effStart < 54 * (strip_0.effStep)) return 0x00;

  float factor1, factor2;
  uint16_t ind;
  for(uint16_t j=0;j<MAX_LED;j++) {
    ind = effStep + j * 1.625;
    switch((int)((ind % 13) / 4.333333333333333)) {
      case 0: factor1 = 1.0 - ((float)(ind % 13 - 0 * 4.333333333333333) / 4.333333333333333);
              factor2 = (float)((int)(ind - 0) % 13) / 4.333333333333333;
              /************ chnaged here *********/
              Set_LED(j, BRILLO_MAX * factor1 + BRILLO_MIN * factor2, BRILLO_MIN * factor1 + BRILLO_MAX * factor2, BRILLO_MIN * factor1 + BRILLO_MIN * factor2);
              //Set_LED(j, 0 * factor1 + 255 * factor2, 255 * factor1 + 0 * factor2, 255 * factor1 + 255 * factor2);
              WS2811_Send();
              break;
      case 1: factor1 = 1.0 - ((float)(ind % 13 - 1 * 4.333333333333333) / 4.333333333333333);
              factor2 = (float)((int)(ind - 4.333333333333333) % 13) / 4.333333333333333;
              //Set_LED(j, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2);
              Set_LED(j, 255 * factor1 + 255 * factor2, 0 * factor1 + 255 * factor2, 255 * factor1 + 0 * factor2);
              WS2811_Send();
              break;
      case 2: factor1 = 1.0 - ((float)(ind % 13 - 2 * 4.333333333333333) / 4.333333333333333);
              factor2 = (float)((int)(ind - 8.666666666666666) % 13) / 4.333333333333333;
              Set_LED(j, BRILLO_MIN * factor1 + BRILLO_MAX * factor2, BRILLO_MIN * factor1 + BRILLO_MIN * factor2, BRILLO_MAX * factor1 + BRILLO_MIN * factor2);
              //Set_LED(j, 255 * factor1 + 0 * factor2, 255 * factor1 + 255 * factor2, 0 * factor1 + 255 * factor2);
              WS2811_Send();
              break;
    }
  }
  if(effStep >= 13) {effStep=0; return 0x03; }
  else effStep++;
  return 0x01;
}

uint8_t rainbow_effect_right() {
    // Strip ID: 0 - Effect: Rainbow - LEDS: 8
    // Steps: 14 - Delay: 30
    // Colors: 3 (255.0.0, 0.255.0, 0.0.255)
    // Options: rainbowlen=8, toLeft=false,
//  if(millis() - strip_0.effStart < 30 * (strip_0.effStep)) return 0x00;
  float factor1, factor2;
  uint16_t ind;
  for(uint16_t j=0;j<MAX_LED;j++) {
    ind = 14 - (int16_t)(effStep - j * 1.75) % 14;
    switch((int)((ind % 14) / 4.666666666666667)) {
      case 0: factor1 = 1.0 - ((float)(ind % 14 - 0 * 4.666666666666667) / 4.666666666666667);
              factor2 = (float)((int)(ind - 0) % 14) / 4.666666666666667;
              //Set_LED(j, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2);
              Set_LED(j, 0 * factor1 + 255 * factor2, 255 * factor1 + 0 * factor2, 255 * factor1 + 255 * factor2);
              WS2811_Send();
              break;
      case 1: factor1 = 1.0 - ((float)(ind % 14 - 1 * 4.666666666666667) / 4.666666666666667);
              factor2 = (float)((int)(ind - 4.666666666666667) % 14) / 4.666666666666667;
              //Set_LED(j, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2);
              Set_LED(j, 255 * factor1 + 255 * factor2, 0 * factor1 + 255 * factor2, 255 * factor1 + 0 * factor2);
              WS2811_Send();
              break;
      case 2: factor1 = 1.0 - ((float)(ind % 14 - 2 * 4.666666666666667) / 4.666666666666667);
              factor2 = (float)((int)(ind - 9.333333333333334) % 14) / 4.666666666666667;
              //Set_LED(j, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2);
              Set_LED(j, 255 * factor1 + 0 * factor2, 255 * factor1 + 255 * factor2, 0 * factor1 + 255 * factor2);
              WS2811_Send();
              break;
    }
  }
  if(effStep >= 14) {effStep = 0; return 0x03; }
  else effStep++;
  return 0x01;
}





