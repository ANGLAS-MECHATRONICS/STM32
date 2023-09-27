/*
 * ENCODER_ROT.c
 *
 *  Created on: Oct 22, 2022
 *      Author: Giovanny Anglas
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ENCODER_ROT.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
uint8_t bit_anterior = 0, bit_actual = 0, bit_armado = 0;
float val_Min_Encoder = 0;
float val_Max_Encoder = 4095;
float valor_Encoder   = 2040;//valor inicial del encoder 3100

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
float Encoder_Run(void){
    //FUNCION PARA LEER EL ENCODER ROTATIVOS, me base de aqui: https://www.youtube.com/watch?v=6c5nL2tcCs0&t=607s
    bit_actual = ((HAL_GPIO_ReadPin(DT_GPIO_Port, DT_Pin) << 1) | HAL_GPIO_ReadPin(CLK_GPIO_Port, CLK_Pin));                  //convierto a 'uint8_t' el valor ya que al compilar me daba un warning "la conversión implícita pierde precisión entera: 'int' a 'uint8_t'"
    bit_armado = ((bit_anterior << 2) | bit_actual);
    bit_anterior = bit_actual;

    //Lectura de encoder rotativo de 20 posiciones
    if(bit_armado==14) valor_Encoder-=10;//horario
    if(bit_armado==4)  valor_Encoder+=10;//antihorario

    //Lectura de encoder rotativo de 30 posiciones
    //if( (bit_armado==14)||(bit_armado==7) ) valor_Encoder+=10;//horario
    //if( (bit_armado==4) ||(bit_armado==2) ) valor_Encoder-=10;//antihorario

    if(valor_Encoder <= val_Min_Encoder) valor_Encoder = val_Min_Encoder;
    if(valor_Encoder >= val_Max_Encoder) valor_Encoder = val_Max_Encoder;

    return valor_Encoder;
}


