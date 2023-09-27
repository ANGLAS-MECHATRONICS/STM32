/*
 * MCP4725.c
 *
 *  Created on: Jan 19, 2023
 *      Author: Giovanny Anglas
 *      Modificado:
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "MCP4725.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void MCP4725_Out_EEPROM(uint16_t out){
	uint8_t datos[3];

	//mando comando de escritura 010(C2=0, C1=1, C0=1) "Write DAC Register & EEPROM" para cargar bits de configuración y código de datos en el registro DAC y eeprom
	//mando 00=XX + Modo Normal 00 (PD1=0, PD0=0) + 0=X
	datos[0]=0b01100000;

	//mando el dato en formato de 12bits ejemplo: numero = 0b111100001100
	datos[1]=((uint8_t)(out >> 4));

	//mando los 4 bits restantes(del bit3 al bit0) "1100", y armo un byte(8bits) rellenando con ceros lo demas.
	datos[2]=( (uint8_t)((out & 0b1111)<<4) ); //byte armado = "11000000"

	HAL_I2C_Master_Transmit (&hi2c1, MCP4725_I2C_ADDRESS_WRITE, (uint8_t*)datos, 3, 100);//mando la direccion 0x48 + el bit de escritura
}

void MCP4725_Out(uint16_t out){
	uint8_t datos[3];

	//mando comando de escritura 010(C2=0, C1=1, C0=0) "Write DAC Register" para cargar bits de configuración y código de datos en el registro DAC
	//mando 00=XX + Modo Normal 00 (PD1=0, PD0=0) + 0=X
	datos[0]=0b01000000;

	//mando el dato en formato de 12bits ejemplo: numero = 0b111100001100
	datos[1]=((uint8_t)(out >> 4));

	//mando los 4 bits restantes(del bit3 al bit0) "1100", y armo un byte(8bits) rellenando con ceros lo demas.
	datos[2]=( (uint8_t)((out & 0b1111)<<4) ); //byte armado = "11000000"

	HAL_I2C_Master_Transmit (&hi2c1, MCP4725_I2C_ADDRESS_WRITE, (uint8_t*)datos, 3, 100);//mando la direccion 0x48 + el bit de escritura
}

uint16_t MCP4725_Read(void){
	//Pag.20 del datasheet del MCP4725
	uint8_t datos[5];//byte2,byte3,byte4,byte5,byte6;
	uint16_t dato;

	HAL_I2C_Master_Receive(&hi2c1, MCP4725_I2C_ADDRESS_READ, (uint8_t*)datos, 5, 100);//mando la direccion 0x48 + el bit de lectura

	dato = ( ( (uint16_t)datos[3] << 8 ) | datos[4] ); //armo el dato de 12bits EEPROM Data
	return dato;
}

