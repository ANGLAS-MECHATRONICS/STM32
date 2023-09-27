/*
 * AD5290.h
 *
 *  Created on: Jan 12, 2023
 *      Author: Giovanny Anglas
 */

#ifndef INC_AD5290_H_
#define INC_AD5290_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void  AD5290_Write(uint8_t value);
void  AD5290_Init(uint8_t value);

#endif /* INC_AD5290_H_ */
