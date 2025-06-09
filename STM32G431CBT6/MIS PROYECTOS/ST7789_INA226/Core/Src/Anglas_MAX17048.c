/*
 * Anglas_MAX17048.c
 *
 *  Created on: Dec 16, 2024
 *      Author: PC
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Anglas_MAX17048.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

uint16_t MAX17048_Init(void){//esta funcion solo pide la version no inicializa, no se necesita inicializar
	uint16_t dato;
	uint8_t datos[3];

	datos[0] = MAX17048_VERSION_ADDR;//leo la version
	HAL_I2C_Master_Transmit (&hi2c1, MAX17048_ADDR_WRITE, (uint8_t*)datos, 1, 100);

	HAL_I2C_Master_Receive(&hi2c1, MAX17048_ADDR_READ, (uint8_t*)datos, 2, 100);
	dato = ((datos[0]<<8)|datos[1]);

	return dato;//me devuelve 0x12 o 18 en decimal y supongo que sera el año 2018, no es importante
}

void MAX17048_QuickStart(void){//el datasheet nos dice que
	uint8_t datos[3];
	datos[0] = MAX17048_MODE_ADDR;//apunto al registro MODE, donde alli se configura los bits de Quick-Start, EnSleep y HibStat
	//pag 11 datasheet: 0bx(Quick-Start)(EnSleep)(HibStat) xxxx xxxx xxxx
	//Quick-Start:ON
	//EnSleep:OFF
	//HibStat:solo lectura
	datos[1] = 0b01000000;//0bx100 xxxx (MSB—ADDRESS 0x06)
	datos[2] = 0b00000000;//0bxxxx xxxx (LSB—ADDRESS 0x07)

	HAL_I2C_Master_Transmit (&hi2c1, MAX17048_ADDR_WRITE, (uint8_t*)datos, 3, 100);
}
/*
 * Pag.12 del datasheet Figure 10
 * CONFIG REGISTER FORMAT: 16bits (8bits 8bits)  (datos[0] datos[1])
 * RCOMP: bit[16-9]
 * 	• RCOMP es un valor de 8 bits que se puede ajustar para optimizar el rendimiento del IC para diferentes químicas de litio o
 * 	  diferentes temperaturas de funcionamiento. Póngase en contacto con Maxim para obtener instrucciones de optimización. El valor
 * 	  POR de RCOMP es 0x97.
 * SLEEP: bit7
 * ALSC : bit6
 * ALRT : bit5
 * ATHD : bit[4-0]
 *
 * para leer o establecer el umbral ATHD(1-32 % de bateria) solo se tiene que modificar los bits 0-4 del LSB
 *
 * */

/*Esto esta para probar conectando una fuente variable de 3.3V a 4.2V, aun no hice pruebas, pero si establece el umbral
 * pero no probe si el pin alert se pone a 0V cuando se da la alerta y tambien ver como resetear el bit ALERT porque cuando
 * se da la alerta este bit se pone a 1 y se queda ahi*/
void MAX17048_SetAlertThreshold(uint8_t threshold){//colocar el umbral de % de bateria para activar el pin ALRT
	uint16_t ATHD;
	uint8_t MSB;
	//uint8_t LSB;
    uint8_t datos[3];

    threshold = 32 - threshold;//aqui hago la resta que dice el datasheet

    datos[0] = MAX17048_CONFIG_ADDR;//apunto al registro CONFIG
    HAL_I2C_Master_Transmit (&hi2c1, MAX17048_ADDR_WRITE, (uint8_t*)datos, 1, 100);

    //Leo el valor que esta almacenado de ATHD: me da en dato[0] y dato[1], y en dato[1] es donde esta ATHD
    //devuelve 16bits en 2 de 8bits (segun datasheet devolvera 0x1C que es 4% de bateria).
    //Este 0x1C devuelve cuando se alimenta por primera vez el uC, luego de esto si se resetea el micro con el pin reset
    //ya no devuelve 0x1C sino el valor seteado
    HAL_I2C_Master_Receive(&hi2c1, MAX17048_ADDR_READ, (uint8_t*)datos, 2, 100);

    MSB = datos[0];//aqui esta los 7 bits de RCOMP, 0x97 de fabrica
	//LSB = datos[1]; //comento porque no necesito el LSB

	//aqui escribo los valores de estos bits y el umbral de % de bateria

    //aqui ver si debo mandar ALRT en OFF o no, ya que creo que cuando se da la alerta este pin se pone a 1
    //y se debe resetear por software
    ATHD = (SLEEP_OFF + ALSC_OFF + ALRT_OFF) + threshold;

    datos[0] = MAX17048_CONFIG_ADDR;//apunto al registro CONFIG
    datos[1] = MSB;//0x97
    datos[2] = ATHD;
    HAL_I2C_Master_Transmit (&hi2c1, MAX17048_ADDR_WRITE, (uint8_t*)datos, 3, 100);
}


float MAX17048_GetVoltage(void){
	uint16_t dato;
	float voltage;
	uint8_t datos[3];

	datos[0] = MAX17048_VCELL_ADDR;//apunto al registro VCELL
	HAL_I2C_Master_Transmit (&hi2c1, MAX17048_ADDR_WRITE, (uint8_t*)datos, 1, 100);

	//Leo el registro VCELL que me devuelve el valor del ADC 16bits en 2 bytes
	HAL_I2C_Master_Receive(&hi2c1, MAX17048_ADDR_READ, (uint8_t*)datos, 2, 100);
	dato = ((datos[0]<<8)|datos[1]);//devuelve el valor del ADC de 16bits(0-65535)

	voltage = dato * 0.000078125;//formula pag.10 datasheet: 78.125μV/cell

	return voltage;
}

float MAX17048_GetSoc(void){
	uint16_t dato;
	float soc;
	uint8_t datos[3];

	datos[0] = MAX17048_SOC_ADDR;//apunto al registro SOC
	HAL_I2C_Master_Transmit (&hi2c1, MAX17048_ADDR_WRITE, (uint8_t*)datos, 1, 100);

	//Leo el registro SOC que me devuelve el valor de % de bateria
	HAL_I2C_Master_Receive(&hi2c1, MAX17048_ADDR_READ, (uint8_t*)datos, 2, 100);
	dato = ((datos[0]<<8)|datos[1]);//devuelve el valor del ADC de 16bits(0-65535)

	soc = (float)dato / 256.0;//formula pag.10 datasheet: 1%/256

	return soc;
}

uint8_t MAX17048_GetAlert(void){
	uint8_t ALRT;
	//uint8_t MSB;
	uint8_t LSB;
    uint8_t datos[3];

    datos[0] = MAX17048_CONFIG_ADDR;//apunto al registro CONFIG
    HAL_I2C_Master_Transmit (&hi2c1, MAX17048_ADDR_WRITE, (uint8_t*)datos, 1, 100);

    //Leo el valor que esta almacenado de ATHD: me da en dato[0] y dato[1], y en dato[1] es donde esta ATHD
    //devuelve 16bits en 2 de 8bits (segun datasheet devolvera 0x1C que es 4% de bateria).
    //Este 0x1C devuelve cuando se alimenta por primera vez el uC, luego de esto si se resetea el micro con el pin reset
    //ya no devuelve 0x1C sino el valor seteado
    HAL_I2C_Master_Receive(&hi2c1, MAX17048_ADDR_READ, (uint8_t*)datos, 2, 100);

    //MSB = datos[0];//aqui esta los 7 bits de RCOMP, 0x97 de fabrica
	LSB = datos[1]; //el bit ALRT es el bit 5 y solo debemos extraer ese bit

	ALRT = (LSB & 0b00100000) >> 5;//extraigo solo el bit 5 haciendo una mascara y luego dezplazando

    return ALRT;
}

void MAX17048_Reset(void){
	uint8_t datos[3];
	datos[0] = MAX17048_MODE_ADDR;//apunto al registro MODE, donde alli se configura los bits de Quick-Start, EnSleep y HibStat
	datos[1] = 0x40;
	datos[2] = 0x00;
	HAL_I2C_Master_Transmit (&hi2c1, MAX17048_ADDR_WRITE, (uint8_t*)datos, 3, 100);
}

