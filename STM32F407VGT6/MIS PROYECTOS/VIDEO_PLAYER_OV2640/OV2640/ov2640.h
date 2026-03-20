#ifndef __OV2640_H
#define __OV2640_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#define CAMERA_Monitor	1
#define CAMERA_Movie	2
#define CAMERA_Picture	3
#define CAMERA_Play		4

void     ov2640_Init(uint16_t DeviceAddr, uint8_t action);
void     ov2640_Config(uint16_t DeviceAddr, uint32_t feature, uint32_t value, uint32_t BR_value);
uint16_t ov2640_ReadID(uint16_t DeviceAddr);

void     CAMERA_IO_Init(void);
void     CAMERA_IO_Write(uint8_t addr, uint8_t reg, uint8_t value);
uint8_t  CAMERA_IO_Read(uint8_t addr, uint8_t reg);
void     CAMERA_Delay(uint32_t delay);
#ifdef __cplusplus
}
#endif
#endif /* __OV2640_H */
