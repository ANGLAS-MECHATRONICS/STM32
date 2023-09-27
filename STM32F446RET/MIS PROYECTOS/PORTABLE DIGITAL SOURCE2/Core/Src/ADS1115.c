/*
 * ADS1115.c
 *
 *  Created on: Dec 20, 2022
 *      Author: Giovanny Anglas
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ADS1115.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

int16_t ADS1115_Read(uint8_t canal){
    int16_t dato;
    uint8_t MSB;

	uint8_t datos[3];

	datos[0]  = ADS1115_REG_CONFIG;//apunto al registro de configuracion

    switch(canal){
        //  OS  CH PGA MODE
        //   1 100 000  1
        case 0: MSB = 0b11000101; break;  //AN0  PGA 2.048V  VOLTAJE
        case 1: MSB = 0b11011111; break;  //AN1  PGA 0.256V  CORRIENTE
        case 2: MSB = 0b11100101; break;  //AN2  PGA 2.048V  BATERIA
        case 3: MSB = 0b11110101; break;} //AN3  PGA 6.144V

    //PGA: +-6.144V, AIN+ = AINX and AIN- = GND, MODO CONVERSION CONTINUA
    datos[1]  = MSB;//MSB del registro de connfiguracion

    //128 SPS, Traditional comparator, active low, nonlatching, dis. ALERT/RDY
    datos[2]  = 0b11100011;//LSB del registro de connfiguracion

    HAL_I2C_Master_Transmit (&hi2c1, ADS1115_I2C_ADDRESS_WRITE, (uint8_t*)datos, 3, 100);//mando la direccion 0x48 + el bit de escritura

    HAL_Delay(8);  //tiempo minimo para volver a comunicarse

    //Escribo o direcciono al registro de conversion
    datos[0] = ADS1115_REG_CONVER;//apunto al registro de conversion

    HAL_I2C_Master_Transmit (&hi2c1, ADS1115_I2C_ADDRESS_WRITE, (uint8_t*)datos, 1, 100);//mando la direccion 0x48 + el bit de escritura


    //Ya que estoy dentro del registro de conversion aqui LEO

    HAL_I2C_Master_Receive(&hi2c1, ADS1115_I2C_ADDRESS_READ, (uint8_t*)datos, 2, 100);//mando la direccion 0x48 + el bit de lectura


    dato = ((datos[0]<<8)|datos[1]);

    HAL_Delay(8);  //tiempo minimo para volver a comunicarse
    return dato;
}


