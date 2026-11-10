/*
 * Anglas_MotorPAP.c
 *
 *  Created on: Nov 13, 2025
 *      Author: Giovanny Anglas
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Anglas_MotorPAP.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constant ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void MotorPAP_1(uint16_t tiempo_ms){
	//esto es para girar 1 vulenta en 10 pequeños pasos
	HAL_GPIO_WritePin(A4988_SLEEP_1_GPIO_Port, A4988_SLEEP_1_Pin, SET);//modo normal
	HAL_GPIO_WritePin(A4988_EN_1_GPIO_Port, A4988_EN_1_Pin, RESET);//habilito el motor
	HAL_GPIO_WritePin(A4988_DIR_1_GPIO_Port, A4988_DIR_1_Pin, SET);//sentido horario

	//for(int i = 0; i < STEP_MOTOR * SIXT_STEP * 0.2; i++){
	for(int i = 0; i < 16; i++){
	  HAL_GPIO_WritePin(A4988_STEP_1_GPIO_Port, A4988_STEP_1_Pin, SET);
	  HAL_Delay(tiempo_ms);
	  //delay_us(tiempo_us);
	  HAL_GPIO_WritePin(A4988_STEP_1_GPIO_Port, A4988_STEP_1_Pin, RESET);
	  HAL_Delay(tiempo_ms);
	  //delay_us(tiempo_us);
	}

	HAL_GPIO_WritePin(A4988_EN_1_GPIO_Port, A4988_EN_1_Pin, SET);//deshabilito el motor
	HAL_Delay(1000);
}
