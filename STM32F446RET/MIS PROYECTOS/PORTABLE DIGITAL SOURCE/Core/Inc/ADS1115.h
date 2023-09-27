	/*
 * ADS1115.h
 *
 *  Created on: Dec 20, 2022
 *      Author: Giovanny Anglas
 */

#ifndef INC_ADS1115_H_
#define INC_ADS1115_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/* Exported constant ---------------------------------------------------------*/
#define ADS1115_I2C_ADDRESS_WRITE 0x90 //mando la direccion 0x48 + el bit de escritura
#define ADS1115_I2C_ADDRESS_READ  0x91 //mando la direccion 0x48 + el bit de lectura
#define ADS1115_REG_CONFIG        0x01 //registro de configuracion
#define ADS1115_REG_CONVER        0x00 //registro de conversion
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
int16_t ADS1115_Read(uint8_t canal);


#endif /* INC_ADS1115_H_ */
