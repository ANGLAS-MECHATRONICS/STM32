/*
 * Anglas_MAX17048.h
 *
 *  Created on: Dec 16, 2024
 *      Author: PC
 */

#ifndef INC_ANGLAS_MAX17048_H_
#define INC_ANGLAS_MAX17048_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/* Exported constant ---------------------------------------------------------*/
//Direccion del esclavo(7 bits) + bit R/W    W:0110110+0  R:0110110+1
#define MAX17048_ADDR_WRITE     0b01101100
#define MAX17048_ADDR_READ      0b01101101
#define MAX17048_VCELL_ADDR     0x02
#define MAX17048_SOC_ADDR       0x04
#define MAX17048_MODE_ADDR      0x06
#define MAX17048_VERSION_ADDR   0x08
#define MAX17048_CONFIG_ADDR    0x0C
#define MAX17048_RCOMP_ADDR     0x0C
#define MAX17048_ATHRD_ADDR     0x0D
#define MAX17048_COMMAND_ADDR   0xFE

#define MAX17048_STATUS_ADDR    0x1A

//BITS OF CONFIG REGISTER
#define SLEEP_ON  0b10000000
#define SLEEP_OFF 0b00000000

#define ALSC_ON   0b01000000
#define ALSC_OFF  0b00000000

#define ALRT_ON  0b00100000
#define ALRT_OFF 0b00000000


/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
uint16_t MAX17048_Init(void);
void MAX17048_QuickStart(void);
void MAX17048_SetAlertThreshold(uint8_t threshold);
uint8_t MAX17048_GetAlert(void);
float MAX17048_GetVoltage(void);
float MAX17048_GetSoc(void);
void MAX17048_Reset(void);


uint8_t Read_STATUS_MAX17048(void);//esto creo eliminar




#endif /* INC_ANGLAS_MAX17048_H_ */
