/*
 * AD5290.c
 *
 *  Created on: Jan 12, 2023
 *      Author: Giovanny Anglas
 *      Modificado:
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "AD5290.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void  AD5290_Write(uint8_t value){ //0 - 128 (128 valores), 0=124ohmios y 128=46.8kohmios para 50kOhmios
	uint8_t buf_tx[1];
	buf_tx[0] = value;
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, RESET);
	HAL_SPI_Transmit(&hspi1, buf_tx, 1, 100);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, SET);
}

void  AD5290_Init(uint8_t value){
	//enviar 2 veces ya que el spi el 1er envio lo envia erroneo y apartir del 2do dato a enviar lo hace correctamente
	AD5290_Write(value);
	HAL_Delay(1);
	AD5290_Write(value);
}

