/*
 * Anglas_MotorPAP.h
 *
 *  Created on: Nov 9, 2026
 *      Author: Giovanny Anglas
 */

#ifndef INC_ANGLAS_MOTORPAP_H_
#define INC_ANGLAS_MOTORPAP_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/


/* Exported constant ---------------------------------------------------------*/
#define STEP_MOTOR 20   // Pasos por vuelta en modo Full Step

#define FULL_STEP  1    // Full step      MS1=0 MS2=0 MS3=0   → STEP_MOTOR * 1   = 20 pasos/vuelta
#define HALF_STEP  2    // Half step      MS1=1 MS2=0 MS3=0   → STEP_MOTOR * 2   = 40 pasos/vuelta
#define QUAR_STEP  4    // Quarter step   MS1=0 MS2=1 MS3=0   → STEP_MOTOR * 4   = 80 pasos/vuelta
#define EIGH_STEP  8    // Eighth step    MS1=1 MS2=1 MS3=0   → STEP_MOTOR * 8   = 160 pasos/vuelta
#define SIXT_STEP 16    // Sixteenth step MS1=1 MS2=1 MS3=1   → STEP_MOTOR * 16  = 320 pasos/vuelta

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void MotorPAP_1(uint16_t tiempo_ms);

#endif /* INC_ANGLAS_MOTORPAP_H_ */
