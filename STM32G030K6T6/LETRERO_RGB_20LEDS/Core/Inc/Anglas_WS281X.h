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
#define MAX_LEDS 7
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
// Direccioes
#define IZQ      1
#define DER_IZQ -1
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


void Respirar(uint8_t tiempo, uint8_t r, uint8_t g, uint8_t b);//OK
void Blink(uint8_t veces, uint16_t tiempo, uint8_t r, uint8_t g, uint8_t b);//OK
void ControlLeds(uint8_t led, uint8_t state, uint8_t r, uint8_t g, uint8_t b);//OK
void Encender_1_Led_2(uint8_t num_led, uint8_t r, uint8_t g, uint8_t b);//OK
void Encender_1_Led_3(uint8_t num_led, uint8_t r, uint8_t g, uint8_t b);//OK
void Encender_1_Led_4(uint8_t num_led, uint8_t indice, uint8_t r, uint8_t g, uint8_t b);//OK
void ArmarIzquierda(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);//OK
void VolumenAbrir(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);//OK
void Barrido1(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);//BLANCO FALLA
void BarridoDer(uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);//OK
void BarridoIzq(uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);//OK
void BarridoColoresDer(uint16_t tiempo, uint16_t tiempo2);//OK
void BarridoColoresDerIzq(uint16_t tiempo, uint16_t tiempo2);//OK
void AbrirApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);//BLANCO FALLA
void CerrarApagar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);//BLANCO FALLA
void Girar(uint8_t veces, uint16_t tiempo, uint8_t r, uint8_t g, uint8_t b);//BLANCO FALLA
void VolumenCerrar(uint8_t veces, uint16_t tiempo, uint16_t tiempo2, uint8_t r, uint8_t g, uint8_t b);//BLANCO FALLA
void applyGradient(void);
void applyGradient2(void);
void applyGradient3(int dir, uint8_t r, uint8_t g, uint8_t b);

#endif /* INC_ANGLAS_WS281X_H_ */
