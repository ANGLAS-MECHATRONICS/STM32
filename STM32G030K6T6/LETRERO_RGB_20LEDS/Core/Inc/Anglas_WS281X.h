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
#define MAX_LEDS 6
#define BRILLO_MAX  0
#define BRILLO_MIN 255
#define BRILLO_OFF 255
// Definiciones de los colores
#define ROJO   	 BRILLO_MAX, BRILLO_OFF, BRILLO_OFF
#define VERDE 	 BRILLO_OFF, BRILLO_MAX, BRILLO_OFF
#define AZUL   	 BRILLO_OFF, BRILLO_OFF, BRILLO_MAX
#define AMARILLO BRILLO_MAX, BRILLO_MAX, BRILLO_OFF
#define CIAN     BRILLO_OFF, BRILLO_MAX, BRILLO_MAX
#define MAGENTA  BRILLO_MAX, BRILLO_OFF, BRILLO_MAX
#define BLANCO   BRILLO_MAX, BRILLO_MAX, BRILLO_MAX

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void Set_LED(uint16_t LEDnum, uint8_t Red, uint8_t Green, uint8_t Blue);
void WS2811_Send(void);
void WS2811_Init(void);//inicia apagado todos los leds
void WS2812_Send(void);
void WS2812_Init(void);//inicia apagado todos los leds

uint8_t rainbow_effect_left(void);
uint8_t rainbow_effect_right(void);
uint8_t strip0_loop0_eff0(void);


void fade_effect(uint8_t tiempo, uint8_t r, uint8_t g, uint8_t b);//OK
void blink_effect(uint8_t veces, uint16_t tiempo, uint8_t r, uint8_t g, uint8_t b);//OK

void ControlLeds(uint8_t led, uint8_t state, uint8_t r, uint8_t g, uint8_t b);//OK
void Encender_1_Led_4(uint8_t num_led, uint8_t indice, uint8_t r, uint8_t g, uint8_t b);//OK
void ArmarIzquierda(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);//OK

void Encender_1_Led_2(uint8_t num_led, uint8_t r, uint8_t g, uint8_t b);
void VolumenAbrir(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);
void Barrido1(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);

void Barrido1(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);
void AbrirApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);
void CerrarApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);
void Girar(uint8_t veces, uint16_t tiempo, uint8_t r, uint8_t g, uint8_t b);

#endif /* INC_ANGLAS_WS281X_H_ */
