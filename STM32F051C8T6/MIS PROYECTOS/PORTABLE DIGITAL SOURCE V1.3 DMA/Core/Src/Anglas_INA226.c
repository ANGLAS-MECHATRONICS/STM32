/*
 * Anglas_INA226.c
 *
 *  Created on: May 20, 2023
 *      Author: Giovanny Anglas
 *  		Finished on: 26/05/2023
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Anglas_INA226.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
Anglas_IN226 ina;
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void INA226_Init_DMA(const double Maximum_Expected_Current, const uint16_t resMiliohmios, uint16_t AVG, uint16_t Time_Vbus, uint16_t Time_Vshunt, uint16_t ModeOperation){
	uint8_t datos[3];
	uint8_t MSB,LSB;
	uint16_t REGISTER;

	ina.Rshunt = resMiliohmios; //esto lo hago porq necesito Rshunt para la funcion "INA226_Alert_Limit_Write"

	/*
	 * bit    15: 0 (No reinicio)
	 * bit 14-12: 100 (valor fijo segun datasheet)
	 * bit 11-9 : 100 (AVG=128, promedio de numero de muestras)
	 * bit  8-6 : 100 (T=1.1ms, tiempo de conversion de Vbus)
	 * bit  5-3 : 111 (T=8.24ms, tiempo de conversion de Vshunt)
	 * bit  2-0 : 111 (Shunt & Bus continuo, modo de operacion)
	*/
	REGISTER = NO_RESET_INA226|(AVG << 9)|(Time_Vbus << 6)|(Time_Vshunt << 3)|ModeOperation;

	MSB = (uint8_t)(REGISTER >> 8);
	LSB = (uint8_t)(REGISTER & 0xFF);

	datos[0] = INA_CONFIGURATION_REGISTER;
	datos[1] = MSB;
	datos[2] = LSB;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 3);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

	ina.current_LSB = (Maximum_Expected_Current*1000000000/32768); // Conseguir el mejor LSB posible en nA
	ina.calibration = (float)5120000 / ((float)ina.current_LSB * ina.Rshunt/1000);  // Compute calibration register(CAL)

	//La formula para "power_LSB" es solo multiplicar "current_LSB" por 25 (pag.16 datasheet)
	//Lo resto 3750 para calibrar(probar desde 2000 - 7000 )
	//Lo divido entre 1000 porq el numero es muy grande y para la operacion de "INA226_Power()" el numero excede de uint32_t
	ina.power_LSB   = (float)25 * (ina.current_LSB-3750)/1000;

	MSB = (uint8_t)(ina.calibration >> 8);
	LSB = (uint8_t)(ina.calibration & 0xFF);

	datos[0] = INA_CALIBRATION_REGISTER;
	datos[1] = MSB;
	datos[2] = LSB;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 3);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
}

float INA226_Vshunt_DMA(void){
	uint16_t dato;
	uint32_t Vshunt;
	uint8_t datos[3];

	datos[0] = INA_SHUNT_VOLTAGE_REGISTER;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

	HAL_I2C_Master_Receive(&hi2c1, INA226_I2C_ADDRESS_READ, (uint8_t*)datos, 2, 100);

	dato = ((datos[0]<<8)|datos[1]);

	if(dato >= 65534){//si no hay carga conectada, devuelve el dato 65535(puede oscilar entre 65534 y 65535)
		return 0;
	}else{
		Vshunt = (uint32_t)dato * INA_SHUNT_VOLTAGE_LSB;
		return Vshunt/10000.0;
	}
}

float INA226_Vbus_DMA(void){
	uint16_t dato;
	uint32_t Vbus;
	uint8_t datos[3];

	datos[0] = INA_BUS_VOLTAGE_REGISTER;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

	HAL_I2C_Master_Receive(&hi2c1, INA226_I2C_ADDRESS_READ, (uint8_t*)datos, 2, 100);

	dato = ((datos[0]<<8)|datos[1]);
	Vbus = (uint32_t)dato * INA_BUS_VOLTAGE_LSB;

	return Vbus/100000.0;//Voltios;
}

float INA226_Current_DMA(void){
	uint16_t dato;
	uint32_t current;
	uint8_t datos[3];

	//todo esto es lo mismo que float INA226_Vshunt(void) ya que si ingreso esta funcion dentro de esta funcion, no me deja hacer debug
	datos[0] = INA_SHUNT_VOLTAGE_REGISTER;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

	HAL_I2C_Master_Receive(&hi2c1, INA226_I2C_ADDRESS_READ, (uint8_t*)datos, 2, 100);

	dato = ((datos[0]<<8)|datos[1]);

	if(dato >= 65534){//si no hay carga conectada, devuelve el dato 65535(puede oscilar entre 65534 y 65535)
		return 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	}else{
		datos[0] = INA_CURRENT_REGISTER;
		HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 1);
		while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

		HAL_I2C_Master_Receive(&hi2c1, INA226_I2C_ADDRESS_READ, (uint8_t*)datos, 2, 100);


		dato = ((datos[0]<<8)|datos[1]);
		current = (uint32_t)dato * (ina.current_LSB - 3750);//restar un valor entre 3000 y 5000 para calibrar las medidas (estoy lo hice con prueba error)
		return current/1000000;//mA;
	}

	/* Esto funciona pero no permite hacer debug, porque uso INA226_Vshunt(void) dentro de INA226_Current(void)
	if(INA226_Vshunt() == 0){
		return 0;
	}else{
		datos[0] = INA_CURRENT_REGISTER;
		HAL_I2C_Master_Transmit (&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 1, 100);

		HAL_I2C_Master_Receive(&hi2c1, INA226_I2C_ADDRESS_READ, (uint8_t*)datos, 2, 100);
		dato = ((datos[0]<<8)|datos[1]);
		current = (uint32_t)dato * (ina.current_LSB-900);
		return current/1000000.0;//mA;
	}*/
}

float INA226_Power_DMA(void){
	uint16_t dato;
	uint32_t power;
	uint8_t datos[3];

	datos[0] = INA_POWER_REGISTER;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

	HAL_I2C_Master_Receive(&hi2c1, INA226_I2C_ADDRESS_READ, (uint8_t*)datos, 2, 100);

	dato = ((datos[0]<<8)|datos[1]);
	power = (uint32_t)dato * ina.power_LSB;
	return power/1000000.0;//Watts;
}

uint16_t INA226_Mode_pinAlert_DMA(uint16_t umbral){// 1000 0xxx xxx0 0010 = 0x2002
	uint16_t dato;
	uint8_t datos[3];
	uint8_t MSB,LSB;

	ina.mask_Enable = umbral;
	/*
	 * bit 15: 1 (Vshunt excede el valor)
	 * bit 14: 0 (Vshunt cae por debajo del valor)
	 * bit 13: 0 (Vbus excede el valor)
	 * bit 12: 0 (Vbus cae por debajo del valor)
	 * bit 11: 0 (Potencia excede el valor)
	 * bit 10-5: x (sin registro: x=0)
	 * bit  4: 0 (Alert Function Flag)
	 * bit  3: 0 (Conversion Ready Flag)
	 * bit  2: 0 (Indicador de desbordamiento matematico)
	 * bit  1: 1 (Polaridad de alerta 1:Invertido 0:Normal)
	 * bit  0: 0 (Alert Latch Enable)
	*/

	MSB = (uint8_t)(umbral >> 8);
	LSB = (uint8_t)(umbral & 0xFF);

	//Envio el "umbral" al registro mask/enable
	datos[0] = INA_MASK_ENABLE_REGISTER;
	datos[1] = MSB;
	datos[2] = LSB;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 3);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

	//Leo el dato "umbral" que envie, para verificar que llego correctamente
	datos[0] = INA_MASK_ENABLE_REGISTER;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

	HAL_I2C_Master_Receive(&hi2c1, INA226_I2C_ADDRESS_READ, (uint8_t*)datos, 2, 100);
	dato = ((datos[0]<<8)|datos[1]);
	return dato;
}

uint16_t INA226_Alert_Limit_DMA(uint16_t limite){//presicion ±2mA, el mV y mW falta probar, pero es muy preciso
	uint16_t dato;
	uint8_t datos[3];
	uint8_t MSB,LSB;
	float calibrar=1.05;////lo multiplico por 1.0099 para calibrarlo, esto lo hice con varias pruebas fisicas y probando con varios Rload

	/* PARA "SHUNT_VOLTAGE_OVER" o "SHUNT_VOLTAGE_UNDER"
	 *
	 * Vshunt = Ishunt * Rshunt;			   Rshunt en miliohmios
	 * Ishunt = Vshunt/Rshunt                  25 = INA_SHUNT_VOLTAGE_LSB
	 * Ishunt = (dato * 25)/Rshunt             dato: es el valor(0-32767) que nos devuelve el IN226 en "INA226_Shunt()"
	 * Ishunt = (dato * 25)/(Rshunt*10)        se multiplica por 10 para llevar a miliamperios
	 *
	 * dato = (Ishunt*Rshunt*10)/25 		   limite = Ishunt (Ingresar "uint16_t limite" en miliamperios)
	 *
	 * PARA "BUS_VOLTAGE_OVER" o "BUS_VOLTAGE_UNDER"
	 *
	 * Vbus = dato * 125;
	 * dato = Vbus/125                   	   125 = INA_BUS_VOLTAGE_LSB
	 * dato = (Vbus*100)/125                   se multiplica por 100 para tener el "dato" en 16bits
	 * dato = Vbus * 0.8;					   dato: es el valor(0-32767) que nos devuelve el IN226 en "INA226_Vbus()"
	 * 										   limite = Vbus (Ingresar "uint16_t limite" en milivoltios)
	 *
	 * PARA "OVER_LIMIT_POWER"
	 *
	 * power = dato * ina.power_LSB;
	 * dato = power/ina.power_LSB;
	 * dato = (power*1000)/ina.power_LSB;      se multiplica por 1000 para tener el "dato" en 16bits
	 * 										   dato: es el valor(0-32767) que nos devuelve el IN226 en "INA226_Vbus()"
	 * 										   limite = power (Ingresar "uint16_t limite" en miliwatts)
	*/
	switch(ina.mask_Enable){
		case SHUNT_VOLTAGE_OVER : limite = ((float)limite*ina.Rshunt*10.0*calibrar)/INA_SHUNT_VOLTAGE_LSB;break;
		case SHUNT_VOLTAGE_UNDER: limite = ((float)limite*ina.Rshunt*10.0*calibrar)/INA_SHUNT_VOLTAGE_LSB;break;
		case BUS_VOLTAGE_OVER   : limite = ((float)limite*0.8);break;
		case BUS_VOLTAGE_UNDER  : limite = ((float)limite*0.8);break;
		case OVER_LIMIT_POWER   : limite = ((float)limite*1000.0)/ina.power_LSB;break;
	}

	MSB = (uint8_t)(limite >> 8);
	LSB = (uint8_t)(limite & 0xFF);

	//Envio el "limite" al registro mask/enable
	datos[0] = INA_ALERT_LIMIT_REGISTER;
	datos[1] = MSB;
	datos[2] = LSB;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 3);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

	//Leo el dato "limite" que envie, para verificar que llego correctamente
	datos[0] = INA_ALERT_LIMIT_REGISTER;
	HAL_I2C_Master_Transmit_DMA(&hi2c1, INA226_I2C_ADDRESS_WRITE, (uint8_t*)datos, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}

	HAL_I2C_Master_Receive(&hi2c1, INA226_I2C_ADDRESS_READ, (uint8_t*)datos, 2, 100);
	dato = ((datos[0]<<8)|datos[1]);
	return dato;
}


