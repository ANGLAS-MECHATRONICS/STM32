/*
 * DHT11.c
 *
 *  Created on: Aug 10, 2022
 *      Author: Giovanny Anglas
 *      Modificado:
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DHT11.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
uint8_t check_DHT11;//creo una variable q solo sirve para ver si el dht11 respondio o esta teniendo problemas

/* Private functions ---------------------------------------------------------*/
static void delay_us(uint16_t time){// 0-1200us 16MHz, 0-800us 72MHz, 0-300us 180MHz
	uint16_t delay = time *(HAL_RCC_GetHCLKFreq() / 1000000);
	for(int  i = 0; i < delay; i+=14){
		__ASM("NOP");
	}
}

static void delay_ms(uint16_t time){// 0-5000ms 96MHz(es el mas preciso), las demas frecuencias funcionan bien con un error minimo
	for(int  i=0; i<time*10; i++)delay_us(100);
}

/* Exported functions --------------------------------------------------------*/


static void DHT11_Pin_In(void){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /*Configure GPIO pin : DHT11_Pin */
	  GPIO_InitStruct.Pin = DHT11_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

static void DHT11_Pin_Out(void){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /*Configure GPIO pin : DHT11_Pin */
	  GPIO_InitStruct.Pin = DHT11_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}


// ENVIAMOS LA SEÑAL DE START HACIA EL DHT11
static void StartSignal(void){
	DHT11_Pin_Out();								     //configuro como salida
	HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, RESET);//envio 0 hacia el sensor
	//HAL_Delay(18);//ya que estoy leyendo cada 1200 por interrupciones, no puedo usar esta funcion de delay
	delay_ms(18);								         //durante 18ms
	HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, SET);  //envio un 1 hacia el sensor
	delay_us(30);										 //esperamos la respuesta del sensor, comentar para frecuencias <=16Mhz
	DHT11_Pin_In();									     //configuro como entrada
	//HAL_GPIO_DeInit(DHT11_GPIO_Port, DHT11_Pin);       //esto tmbn configura como entrada y demora menos
}

// RESPUESTA DEL SENSOR DHT11 //////////////////////////////
static void CheckResponse(void){
	check_DHT11=0;//creo una variable q solo sirve para ver si el dht11 respondio o esta teniendo problemas
	delay_us(40);//espero 40us (la mitad de 80us es decir la trama central del nivel bajo)

    if(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin)==0){   //consulto si el DHTPIN esta a nivel bajo
    	delay_us(80); 								       //espero 80us(trama central del nivel alto)
       if(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin)==1){//consulto si el DHTPIN esta a nivel alto
    	   check_DHT11=1;								   //la comunicacion es correcta y espero a leer los 40bits del sensor
    	   delay_us(40);								   //completo los 40us de la trama de nivel alto
       }
    }
}

// LECTURA DE DATOS DEL DHT11
static uint8_t ReadData(void){// lectura de 8bits o 1Byte o una trama de datos
	uint8_t i,j;
	for(j=0;j<8;j++){//8bits
		while(!HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin));//espera hasta que la entrada de DHTPIN sea un alto aprox 50us segun el datasheet
		delay_us(30);//tiempo de espera segun el datasheet 26-28us, le agrego 2us mas para dejarle pasar al estado bajo
		//si leyo un cero, es decir que el pulso de 26-28us se dio y paso a bajo (la cual le dimos 2us mas de seguridad)
		 if(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin)==0){//si despues del puslo de 26-28us paso a cero que quiere decir que transmitio un "0"logico
			i=(uint8_t)(i & ~(1<<(7-j))); //entonces escribe un "0"(23-28us datasheet) en la posicion (7-j) de i
		 }
		 //si leyo un uno, es decir que el pulso de 26-28us se dio y no paso a bajo (la cual debe ser un pulso mayor a 26-28us)
		 else{                   //si despues del puslo de 26-28us no paso a cero que quiere decir que transmitio un "1"logico o un pulso de 70us
			i=(uint8_t)(i | (1<<(7-j)));   //entonces escribe un "1" en la posicion (7-j) de i
			while(HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin));
		 }
	}
	return i;
}


void Leer_DHT11(uint8_t *temperatura, uint8_t *humedad){ //creo dos variables tipo punteros(*)enteras
	uint8_t datosDHT11[5],k; 		 //variable para almacenar los 5bytes ----- variable para la iteracion de los 5bytes
	StartSignal();  				 //enviamos la funcion de start
	CheckResponse(); 				 //esperamos la respuesta del sensor
    if(check_DHT11==1){              //if la comunucacion es correcta
       for(k=0;k<5;k++){             //leemos los 5bytes
           datosDHT11[k]= ReadData();//los almacenamos en la matriz datosDHT11[k]
       }
    }

    if(datosDHT11[4] == (datosDHT11[0]+datosDHT11[1]+datosDHT11[2]+datosDHT11[3])){  //verifico si el ultimo byte es igual ala suma de los 4 primeros bytes
       *humedad     = datosDHT11[0];//guarda el  datosDHT11[0] en la direccion de memoria de *humedad
       *temperatura = datosDHT11[2];//la direccion de memoria(temperatura=0xFA), donde quiero que guardes "datosDHT11[2]"
	}
}








