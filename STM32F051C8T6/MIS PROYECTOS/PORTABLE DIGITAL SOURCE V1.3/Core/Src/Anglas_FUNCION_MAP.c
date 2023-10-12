/*
 * Anglas_FUNCION_MAP.c
 *
 *  Created on: Oct 12, 2023
 *      Author: PC
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Anglas_FUNCION_MAP.h"

/* Exported functions --------------------------------------------------------*/

// Me base de este blog : https://electgpl.blogspot.com/2015/08/despues-de-trabajar-con-ccs-y-arduino.html
double map(float valor, float entradaMin, float entradaMax, float salidaMin, float salidaMax){
    return ((((valor-entradaMin)*(salidaMax-salidaMin))/(entradaMax-entradaMin))+salidaMin);
}

