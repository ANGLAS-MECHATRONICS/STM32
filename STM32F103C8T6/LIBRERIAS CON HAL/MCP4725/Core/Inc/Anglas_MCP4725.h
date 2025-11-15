/*
 * Anglas_MCP4725.h
 *
 *  Created on: Nov 13, 2025
 *      Author: Giovanny Anglas
 */

#ifndef INC_ANGLAS_MCP4725_H_
#define INC_ANGLAS_MCP4725_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/* Exported constant ---------------------------------------------------------*/
//codigo del dispositivo 1100 + direccion 000(A2=0, A1=0, A0=0) + el bit de escritura "0"
#define MCP4725_I2C_ADDRESS_WRITE 0b11000000

//codigo del dispositivo 1100 + direccion 000(A2=0, A1=0, A0=0) + el bit de lectura "1"
#define MCP4725_I2C_ADDRESS_READ  0b11000001

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void MCP4725_Out(uint16_t out);
void MCP4725_Out_Save_EEPROM(uint16_t out);
uint16_t MCP4725_Read_EEPROM(void);

#endif /* INC_ANGLAS_MCP4725_H_ */
