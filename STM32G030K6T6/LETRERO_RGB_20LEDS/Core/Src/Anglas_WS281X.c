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
uint8_t LED_Data[MAX_LEDS][4];//para leds
uint8_t LED_Mod[MAX_LEDS][4];//para brillo
uint8_t datasentflag = 0;
uint16_t pwmData[(24*MAX_LEDS)+240];//para ws2811
//uint16_t pwmData[(24*MAX_LED)+50];//para ws2812
//la condicion de RESET en WS2811 me pide >280uS y como mi señal es de 800kHz = 1.25uS
//le doy 240 x 1.25uS = 300uS (en el analizador logico me sale 305uS , asi que todo bien)
uint16_t effStep = 0;//variable solo para los efectos
uint16_t effStart = 0;//variable solo para los efectos

/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
	HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);
	datasentflag=1;
}

void Set_LED(uint16_t LEDnum, uint8_t Red, uint8_t Green, uint8_t Blue){
	LED_Data[LEDnum][0] = LEDnum;
	LED_Data[LEDnum][1] = Red;
	LED_Data[LEDnum][2] = Green;
	LED_Data[LEDnum][3] = Blue;
}

void WS2811_Send(void){
	uint32_t indx=0;
	uint32_t color;

	for(uint8_t i= 0; i<MAX_LEDS; i++){
		color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));
		for(int8_t i=23; i>=0; i--){
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
  for(uint8_t i=0; i<MAX_LEDS; i++) Set_LED(i, 255, 255, 255);
  WS2811_Send();
  HAL_Delay(10);//bajar el tiempo si es necesario

}


void WS2812_Send(void){
	uint32_t indx=0;
	uint32_t color;

	for(uint8_t i= 0; i<MAX_LEDS; i++){
		color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));
		for(int8_t i=23; i>=0; i--){
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
  for(uint8_t i=0; i<MAX_LEDS; i++) Set_LED(i, BRILLO_OFF, BRILLO_OFF, BRILLO_OFF);
  WS2812_Send();
  HAL_Delay(2);//bajar el tiempo si es necesario

}

///////////////////////////////// EFECTOS //////////////////////////////////////////////////
// Strip ID: 0 - Effect: Rainbow - LEDS: 8
// Steps: 13 - Delay: 54
// Colors: 3 (255.0.0, 0.255.0, 0.0.255)
// Options: rainbowlen=8, toLeft=true,
//  if(millis() - strip_0.effStart < 54 * (strip_0.effStep)) return 0x00;

/*uint8_t rainbow_effect_left(void) {
  float factor1, factor2;
  uint16_t ind;
  for(uint16_t j=0;j<MAX_LED;j++) {
    ind = effStep + j * 1.625;
    switch((int)((ind % 13) / 4.333333333333333)) {
      case 0: factor1 = 1.0 - ((float)(ind % 13 - 0 * 4.333333333333333) / 4.333333333333333);
              factor2 = (float)((int)(ind - 0) % 13) / 4.333333333333333;
              Set_LED(j, BRILLO_MAX * factor1 + BRILLO_MIN * factor2, BRILLO_MIN * factor1 + BRILLO_MAX * factor2, BRILLO_MIN * factor1 + BRILLO_MIN * factor2);
              WS2811_Send();
              break;
      case 1: factor1 = 1.0 - ((float)(ind % 13 - 1 * 4.333333333333333) / 4.333333333333333);
              factor2 = (float)((int)(ind - 4.333333333333333) % 13) / 4.333333333333333;
              Set_LED(j, BRILLO_MIN * factor1 + BRILLO_MIN * factor2, BRILLO_MAX * factor1 + BRILLO_MIN * factor2, BRILLO_MIN * factor1 + BRILLO_MAX * factor2);
              WS2811_Send();
              break;
      case 2: factor1 = 1.0 - ((float)(ind % 13 - 2 * 4.333333333333333) / 4.333333333333333);
              factor2 = (float)((int)(ind - 8.666666666666666) % 13) / 4.333333333333333;
              Set_LED(j, BRILLO_MIN * factor1 + BRILLO_MAX * factor2, BRILLO_MIN * factor1 + BRILLO_MIN * factor2, BRILLO_MAX * factor1 + BRILLO_MIN * factor2);
              WS2811_Send();
              break;
    }
  }
  if(effStep >= 13) {effStep=0; return 0x03; }
  else effStep++;
  return 0x01;
}*/

#define FACTOR_ESCALA 1000   // Factor para manejar números enteros en lugar de floats
#define MAX_MODULO 13000     // 13 * FACTOR_ESCALA (escalamos para la parte de ind % 13)
#define DIV_FACTOR 4333      // 4.333333 * FACTOR_ESCALA

uint8_t rainbow_effect_left(void) {
    int factor1, factor2;
    uint16_t ind;
    int modInd, stepFactor;

    for (uint16_t j = 0; j < MAX_LEDS; j++) {
        // Escalamos ind a enteros, multiplicamos por 1000 para evitar flotantes
        ind = effStep * FACTOR_ESCALA + j * 1625;  // (1.625 * FACTOR_ESCALA)
        modInd = ind % MAX_MODULO;  // Equivale a ind % 13
        stepFactor = modInd / DIV_FACTOR;  // División con el factor de escala

        // Calculamos el valor de factor1 y factor2
        factor1 = FACTOR_ESCALA - stepFactor;  // factor1 = 1.0 - stepFactor
        factor2 = stepFactor;  // factor2 = stepFactor

        // Dependiendo de la posición, asignamos colores
        switch (modInd / DIV_FACTOR) {
            case 0:
                Set_LED(j, BRILLO_MAX * factor1 / FACTOR_ESCALA + BRILLO_MIN * factor2 / FACTOR_ESCALA,
                        BRILLO_MIN * factor1 / FACTOR_ESCALA + BRILLO_MAX * factor2 / FACTOR_ESCALA,
                        BRILLO_MIN * factor1 / FACTOR_ESCALA + BRILLO_MIN * factor2 / FACTOR_ESCALA);
                break;
            case 1:
                Set_LED(j, BRILLO_MIN * factor1 / FACTOR_ESCALA + BRILLO_MIN * factor2 / FACTOR_ESCALA,
                        BRILLO_MAX * factor1 / FACTOR_ESCALA + BRILLO_MIN * factor2 / FACTOR_ESCALA,
                        BRILLO_MIN * factor1 / FACTOR_ESCALA + BRILLO_MAX * factor2 / FACTOR_ESCALA);
                break;
            case 2:
                Set_LED(j, BRILLO_MIN * factor1 / FACTOR_ESCALA + BRILLO_MAX * factor2 / FACTOR_ESCALA,
                        BRILLO_MIN * factor1 / FACTOR_ESCALA + BRILLO_MIN * factor2 / FACTOR_ESCALA,
                        BRILLO_MAX * factor1 / FACTOR_ESCALA + BRILLO_MIN * factor2 / FACTOR_ESCALA);
                break;
        }

        WS2811_Send();
    }

    if (effStep >= 13) {
        effStep = 0;
        return 0x03;
    } else {
        effStep++;
        return 0x01;
    }
}











uint8_t rainbow_effect_right(void) {
    // Strip ID: 0 - Effect: Rainbow - LEDS: 8
    // Steps: 14 - Delay: 30
    // Colors: 3 (255.0.0, 0.255.0, 0.0.255)
    // Options: rainbowlen=8, toLeft=false,
//  if(millis() - strip_0.effStart < 30 * (strip_0.effStep)) return 0x00;
  float factor1, factor2;
  uint16_t ind;
  for(uint16_t j=0;j<MAX_LEDS;j++) {
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



uint8_t strip0_loop0_eff0(void) {//funciona pero consume mucha flash
    // Strip ID: 0 - Effect: Fade - LEDS: 6
    // Steps: 83 - Delay: 12
    // Colors: 2 (0.0.0, 255.255.255)
    // Options: duration=996, every=1,
  //if(millis() - strip_0.effStart < 12 * (strip_0.effStep)) return 0x00;
  uint8_t r,g,b;
  double e;
  e = (effStep * 5) / (double)500;
  r = ( e ) * 255 + 0 * ( 1.0 - e );
  g = ( e ) * 255 + 255 * ( 1.0 - e );
  b = ( e ) * 255 + 255 * ( 1.0 - e );
  for(uint16_t j=0;j<6;j++) {
    if((j % 1) == 0){
    	Set_LED(j, r, g, b);
    	WS2811_Send();
    }else{
    	Set_LED(j, 0, 0, 0);
    	WS2811_Send();
    }
  }
  if(effStep >= 200) {effStep=0; return 0x03; }
  else effStep++;
  return 0x01;
}

//for(uint8_t i=5;i>=0;i--); aqui cuando compara 0>=0 todo bien y luego decrementa a 255, 255>=0(TRUE) y nunca sale del for:5 4 3 2 1 0 255 254 253... 5 4 3 2 1 0 255 254.......
//for(int8_t  i=5;i>=0;i--); aqui si hace todo bien y devuelve 5 4 3 2 1 0 (±128)
//for(int16_t  i=5;i>=0;i--); necesitamos hasta 255 por eso int16_t (±32768 )

/*void fade_effect(uint8_t tiempo){
	for(int16_t brillo = BRILLO_MIN; brillo >= BRILLO_MAX; brillo--){//int porque tiene que compara 0
		for(uint8_t i=0; i<MAX_LED; i++) Set_LED(i, brillo, BRILLO_OFF, BRILLO_OFF);
		WS2811_Send();
		HAL_Delay(tiempo);
	}
	HAL_Delay(300);

	for(int16_t brillo = BRILLO_MAX; brillo <= BRILLO_MIN; brillo++) {
	    for(uint8_t i=0; i<MAX_LED; i++) Set_LED(i, brillo, BRILLO_OFF, BRILLO_OFF);
	    WS2811_Send();
	    HAL_Delay(tiempo);
	}
	HAL_Delay(300);
}*/

void fade_effect(uint8_t tiempo, uint8_t r, uint8_t g, uint8_t b) {

    for(int16_t brillo = BRILLO_MIN; brillo >= BRILLO_MAX; brillo--) { // De mayor a menor brillo
        for(uint8_t i = 0; i < MAX_LEDS; i++) {
            // Usar los parámetros de color para cada LED
            //Set_LED(i, r * brillo / 255, g * brillo / 255, b * brillo / 255);
            //Set_LED(i, r * (255 - brillo) / 255, g * (255 - brillo) / 255, b * (255 - brillo) / 255);

            if(r==BRILLO_MAX && g==BRILLO_MIN && b==BRILLO_MIN ){//ROJO
            	Set_LED(i,brillo,BRILLO_OFF,BRILLO_OFF);
            }else if(r==BRILLO_MIN && g==BRILLO_MAX && b==BRILLO_MIN ){//VERDE
            	Set_LED(i,BRILLO_OFF,brillo,BRILLO_OFF);
            }else if(r==BRILLO_MIN && g==BRILLO_MIN && b==BRILLO_MAX ){//AZUL
            	Set_LED(i,BRILLO_OFF,BRILLO_OFF,brillo);
            }else if(r==BRILLO_MAX && g==BRILLO_MIN && b==BRILLO_MAX ){//VIOLETA
            	Set_LED(i,brillo,BRILLO_OFF,brillo);
            }else if(r==BRILLO_MAX && g==BRILLO_MAX && b==BRILLO_MIN ){//AMARILLO
            	Set_LED(i,brillo,brillo,BRILLO_OFF);
            }else if(r==BRILLO_MIN && g==BRILLO_MAX && b==BRILLO_MAX ){//CELESTE
            	Set_LED(i,BRILLO_OFF,brillo,brillo);
            }else if(r==BRILLO_MAX && g==BRILLO_MAX && b==BRILLO_MAX ){//BLANCO
            	Set_LED(i,brillo,brillo,brillo);
            }
        }
        WS2811_Send();
        HAL_Delay(tiempo);
    }
    HAL_Delay(900);

    for(int16_t brillo = BRILLO_MAX; brillo <= BRILLO_MIN; brillo++) { // De menor a mayor brillo
        for(uint8_t i = 0; i < MAX_LEDS; i++) {
            // Usar los parámetros de color para cada LED
            //Set_LED(i, r * brillo / 255, g * brillo / 255, b * brillo / 255);
            //Set_LED(i, r * (255 - brillo) / 255, g * (255 - brillo) / 255, b * (255 - brillo) / 255);
            if(r==BRILLO_MAX && g==BRILLO_MIN && b==BRILLO_MIN ){//ROJO
            	Set_LED(i,brillo,BRILLO_OFF,BRILLO_OFF);
            }else if(r==BRILLO_MIN && g==BRILLO_MAX && b==BRILLO_MIN ){//VERDE
            	Set_LED(i,BRILLO_OFF,brillo,BRILLO_OFF);
            }else if(r==BRILLO_MIN && g==BRILLO_MIN && b==BRILLO_MAX ){//AZUL
            	Set_LED(i,BRILLO_OFF,BRILLO_OFF,brillo);
            }else if(r==BRILLO_MAX && g==BRILLO_MIN && b==BRILLO_MAX ){//MAGENTA
            	Set_LED(i,brillo,BRILLO_OFF,brillo);
            }else if(r==BRILLO_MAX && g==BRILLO_MAX && b==BRILLO_MIN ){//AMARILLO
            	Set_LED(i,brillo,brillo,BRILLO_OFF);
            }else if(r==BRILLO_MIN && g==BRILLO_MAX && b==BRILLO_MAX ){//CIAN
            	Set_LED(i,BRILLO_OFF,brillo,brillo);
            }else if(r==BRILLO_MAX && g==BRILLO_MAX && b==BRILLO_MAX ){//BLANCO
            	Set_LED(i,brillo,brillo,brillo);
            }
        }
        WS2811_Send();
        HAL_Delay(tiempo);
    }
    HAL_Delay(100);
}

void Blink(uint8_t veces, uint16_t tiempo, uint8_t r, uint8_t g, uint8_t b) {

	for(uint8_t v=0;v<veces;v++){
		for(uint8_t i=0; i<MAX_LEDS; i++) Set_LED(i, r, g, b);
		WS2811_Send();
		HAL_Delay(tiempo);
		//HAL_Delay(tiempo*Refresh_ADC_Value());
		for(uint8_t i=0; i<MAX_LEDS; i++) Set_LED(i, BRILLO_OFF, BRILLO_OFF, BRILLO_OFF);
		WS2811_Send();
		HAL_Delay(tiempo/5);
		//HAL_Delay((tiempo*Refresh_ADC_Value())/5);
	}

}

void Encender_1_Led_3(uint8_t num_led, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t i=1; i<=MAX_LEDS; i++){
		if(i>=num_led){
			ControlLeds(i,1,r,g,b);
		}else{
			ControlLeds(i,0,r,g,b);
		}
	}
}

void VolumenCerrar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t v=0;v<veces;v++){
		uint8_t l;
		for(uint8_t i=MAX_LEDS;i>=1;i--){
			l=i;
			HAL_Delay(tiempo);
			//HAL_Delay(tiempo*Refresh_ADC_Value());
			for(uint8_t i=l;i<=MAX_LEDS;i++){
				Encender_1_Led_3(i,r,g,b);
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}
			for(uint8_t i=MAX_LEDS;i>=l;i--){
				Encender_1_Led_3(i,r,g,b);
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}
		}
		HAL_Delay(tiempo2);
		//HAL_Delay(tiempo2*Refresh_ADC_Value());
	}
}

























void ControlLeds(uint8_t led, uint8_t state, uint8_t r, uint8_t g, uint8_t b){
	if(state == 1){
		Set_LED(led-1, r, g, b);
		WS2811_Send();
	}else{
		Set_LED(led-1, BRILLO_OFF, BRILLO_OFF, BRILLO_OFF);
		WS2811_Send();
	}
}

void Encender_1_Led_2(uint8_t num_led, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t i=1; i<=MAX_LEDS; i++){
		if(i<=num_led){
			ControlLeds(i,1,r,g,b);
		}else{
			ControlLeds(i,0,r,g,b);
		}
	}
}

void Encender_1_Led_4(uint8_t num_led, uint8_t indice, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t i=MAX_LEDS;i>=indice;i--){
		if(i == num_led){
			ControlLeds(i,1,r,g,b);
		}else{
			ControlLeds(i,0,r,g,b);
		}
	}
}

void ArmarIzquierda(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t v=0;v<veces;v++){

		uint8_t l=1;
		for(uint8_t i=MAX_LEDS;i>=1;i--){
			for(uint8_t i=MAX_LEDS;i>=l;i--){
				Encender_1_Led_4(i,l,r,g,b);
				if(l<MAX_LEDS/2){
					HAL_Delay(tiempo);
					//HAL_Delay(tiempo*Refresh_ADC_Value());
				}else if(l>=MAX_LEDS-2){
					HAL_Delay(tiempo*4);
					//HAL_Delay(tiempo*Refresh_ADC_Value()*4);
				}else{
					HAL_Delay(tiempo*2);
					//HAL_Delay(tiempo*Refresh_ADC_Value()*2);
				}
			}

			if(l>=MAX_LEDS){
				l=1;
			}else{
				ControlLeds(l, 1,r,g,b);
				l++;
			}
		}
		HAL_Delay(tiempo2);
		//HAL_Delay(tiempo2*Refresh_ADC_Value());
	}
}


void VolumenAbrir(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t v=0;v<veces;v++){
		uint8_t l;
		for(uint8_t i=1;i<=MAX_LEDS;i++){
			l=i;
			HAL_Delay(tiempo);
			//HAL_Delay(tiempo*Refresh_ADC_Value());
			for(uint8_t i=l;i<=MAX_LEDS;i++){
				Encender_1_Led_2(i,r,g,b);
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}
			for(uint8_t i=MAX_LEDS;i>=l;i--){
				Encender_1_Led_2(i,r,g,b);
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}
		}
		HAL_Delay(tiempo2);
		//HAL_Delay(tiempo2*Refresh_ADC_Value());
	}
}

void Barrido1(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t v=0;v<veces;v++){
		for(uint8_t i=1; i<=MAX_LEDS; i++){
			ControlLeds(i,1,r,g,b);
			HAL_Delay(tiempo);
			//HAL_Delay(tiempo*Refresh_ADC_Value());
		}

		HAL_Delay(tiempo2);
		//HAL_Delay(tiempo2*Refresh_ADC_Value());

		for(uint8_t i=1; i<=MAX_LEDS; i++){
			ControlLeds(i,0,r,g,b);
			HAL_Delay(tiempo);
			//HAL_Delay(tiempo*Refresh_ADC_Value());
		}
	}
}


void AbrirApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b){

	for(uint8_t v=0;v<veces;v++){
		uint8_t centro,impar=1,par=0;
		if(MAX_LEDS%2==0){//si es par
			centro = MAX_LEDS/2;
			for(uint8_t i=centro;i>=1;i--){
				ControlLeds(i, 1,r,g,b);
				ControlLeds(i+impar, 1,r,g,b);
				impar=impar+2;
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}

			impar=1;
			HAL_Delay(tiempo2);
			//HAL_Delay(tiempo2*Refresh_ADC_Value());

			for(uint8_t i=centro;i>=1;i--){
				ControlLeds(i, 0,r,g,b);
				ControlLeds(i+impar, 0,r,g,b);
				impar=impar+2;
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}

		}else{//si es impar
			centro = (MAX_LEDS/2)+1;
			for(uint8_t i=centro;i>=1;i--){
				ControlLeds(i, 1,r,g,b);
				ControlLeds(i+par, 1,r,g,b);
				par=par+2;
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}

			par=0;
			HAL_Delay(tiempo2);
			//HAL_Delay(tiempo2*Refresh_ADC_Value());

			for(uint8_t i=centro;i>=1;i--){
				ControlLeds(i, 0,r,g,b);
				ControlLeds(i+par, 0,r,g,b);
				par=par+2;
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}
		}
	}
}

void CerrarApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t v=0;v<veces;v++){
		uint8_t limite,centro;
		if(MAX_LEDS%2==0){//si es par

			centro = MAX_LEDS/2;//5
			limite = MAX_LEDS-1;
			for(uint8_t i=1;i<=centro;i++){
				ControlLeds(i, 1,r,g,b);
				ControlLeds(i+limite, 1,r,g,b);
				limite=limite-2;
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}
			limite = MAX_LEDS-1;
			HAL_Delay(tiempo2);
			//HAL_Delay(tiempo2*Refresh_ADC_Value());
			for(uint8_t i=1;i<=centro;i++){
				ControlLeds(i, 0,r,g,b);
				ControlLeds(i+limite, 0,r,g,b);
				limite=limite-2;
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}

		}else{//si es impar
			centro = (MAX_LEDS/2)+1;//5
			limite = MAX_LEDS-1;
			for(uint8_t i=1;i<=centro;i++){
				ControlLeds(i, 1,r,g,b);
				ControlLeds(i+limite, 1,r,g,b);
				limite=limite-2;
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}
			limite = MAX_LEDS-1;
			HAL_Delay(tiempo2);
			//HAL_Delay(tiempo2*Refresh_ADC_Value());
			for(uint8_t i=1;i<=centro;i++){
				ControlLeds(i, 0,r,g,b);
				ControlLeds(i+limite, 0,r,g,b);
				limite=limite-2;
				HAL_Delay(tiempo);
				//HAL_Delay(tiempo*Refresh_ADC_Value());
			}
		}
	}
}


void Girar(uint8_t veces, uint16_t tiempo, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t v=0;v<veces;v++){
		AbrirApagar(1,tiempo,0,r,g,b);
		//AbrirApagar(1,tiempo*Refresh_ADC_Value(),0);
		CerrarApagar(1,tiempo,0,r,g,b);
		//CerrarApagar(1,tiempo*Refresh_ADC_Value(),0);
	}
}
