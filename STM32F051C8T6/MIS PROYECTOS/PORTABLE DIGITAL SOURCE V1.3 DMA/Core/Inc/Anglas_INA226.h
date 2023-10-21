/*
 * Anglas_INA226.h
 *
 *  Created on: May 20, 2023
 *      Author: Giovanny Anglas
 */

#ifndef INC_ANGLAS_INA226_H_
#define INC_ANGLAS_INA226_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/* Exported constant ---------------------------------------------------------*/
//Direccion del esclavo(7 bits) + bit R/W  1000000+0          // A1=GND A2=GND
#define INA226_I2C_ADDRESS_WRITE 0b10000000
#define INA226_I2C_ADDRESS_READ  0b10000001

#define I2C_DELAY                         10                       // Microsecond delay on write       //
#define INA_CONFIGURATION_REGISTER         0                       // Registers common to all INAs     //
#define INA_SHUNT_VOLTAGE_REGISTER         1                       //                                  //
#define INA_BUS_VOLTAGE_REGISTER           2                       //                                  //
#define INA_POWER_REGISTER                 3                       //                                  //
#define INA_CURRENT_REGISTER               4                       //                                  //
#define INA_CALIBRATION_REGISTER           5                       //                                  //
#define INA_MASK_ENABLE_REGISTER           6                       //                                  //
#define INA_ALERT_LIMIT_REGISTER           7                       //                                  //

//#define INA_MANUFACTURER_ID_REGISTER    0xFE                       //                                  //
//#define INA_RESET_DEVICE              0x8000                       // Write to configuration to reset  //
//#define INA_DEFAULT_CONFIGURATION     0x4127                       // Default configuration register   //
#define INA_BUS_VOLTAGE_LSB              125                       // LSB in uV *100 1.25mV            //
#define INA_SHUNT_VOLTAGE_LSB             25                       // LSB in uV *10  2.5uV             //

#define INA_CONFIG_AVG_MASK           0x0E00                       // Bits 9-11                        //
#define INA_CONFIG_BUS_TIME_MASK      0x01C0                       // Bits 6-8                         //
#define INA_CONFIG_SHUNT_TIME_MASK    0x0038                       // Bits 3-5                         //
#define INA_CONVERSION_READY_MASK     0x0080                       // Bit 4                            //
#define INA_CONFIG_MODE_MASK          0x0007                       // Bits 0-3                         //

#define NO_RESET_INA226 0x4000
#define RESET_INA226    0xC000

#define AVG_1    0
#define AVG_4 	 1
#define AVG_16 	 2
#define AVG_64   3
#define AVG_128  4
#define AVG_256  5
#define AVG_512  6
#define AVG_1024 7

#define T_Vbus_140us    0
#define T_Vbus_204us    1
#define T_Vbus_332us    2
#define T_Vbus_588us    3
#define T_Vbus_1_1ms    4
#define T_Vbus_2_116ms  5
#define T_Vbus_4_156ms  6
#define T_Vbus_8_244ms  7

#define T_Vshunt_140us    0
#define T_Vshunt_204us    1
#define T_Vshunt_332us    2
#define T_Vshunt_588us    3
#define T_Vshunt_1_1ms    4
#define T_Vshunt_2_116ms  5
#define T_Vshunt_4_156ms  6
#define T_Vshunt_8_244ms  7

#define MODE_POWER_DOWN  			   0
#define MODE_SHUNT_VOLTAGE_TRIGGERED   1
#define MODE_BUS_VOLTAGE_TRIGGERED     2
#define MODE_SHUNT_BUS__TRIGGERED      3
#define MODE_SHUNT_VOLTAGE_CONTINUOUS  5
#define MODE_BUS_VOLTAGE_CONTINUOUS    6
#define MODE_SHUNT_BUS_CONTINUOUS      7

//Polaridad del pin alert invertida
#define SHUNT_VOLTAGE_OVER  0x8002
#define SHUNT_VOLTAGE_UNDER 0x4002
#define BUS_VOLTAGE_OVER    0x2002
#define BUS_VOLTAGE_UNDER   0x1002
#define OVER_LIMIT_POWER    0x802

/* Exported macros -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
typedef struct {                                                            // Structure of values per device   //
  uint8_t  address;                                                         // I2C Address of device            //
  uint16_t calibration;                                                     // Calibration register value       //
  uint32_t current_LSB;                                                     // Amperage LSB                     //
  uint32_t power_LSB;                                                       // Wattage LSB                      //
  uint8_t  operatingMode;                                                   // Default continuous mode operation//
  uint16_t  Rshunt;															//miliohmios
  uint16_t mask_Enable;
} Anglas_IN226; // of structure
/* Exported functions prototypes ---------------------------------------------*/
void INA226_Init_DMA(const double Maximum_Expected_Current, const uint16_t resMiliohmios, uint16_t AVG, uint16_t Time_Vbus, uint16_t Time_Vshunt, uint16_t ModeOperation);

float INA226_Vshunt_DMA(void);
float INA226_Vbus_DMA(void);
float INA226_Current_DMA(void);
float INA226_Power_DMA(void);

uint16_t INA226_Mode_pinAlert_DMA(uint16_t umbral);
uint16_t INA226_Alert_Limit_DMA(uint16_t limite);


#endif /* INC_ANGLAS_INA226_H_ */
