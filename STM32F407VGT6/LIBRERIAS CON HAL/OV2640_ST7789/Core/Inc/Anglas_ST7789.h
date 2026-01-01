/*
 * Anglas_ST7789.h
 *
 *  Created on: Jul 22, 2024
 *  Author: Golinskiy Konstantin (golinskiy.konstantin@gmail.com)
 *  Modificado por: Giovanny Anglas
 */

#ifndef INC_ANGLAS_ST7789_H_
#define INC_ANGLAS_ST7789_H_

#include "fonts.h"

extern SPI_HandleTypeDef hspi2;

//#define ST7789_IS_135X240 // 1.14" 135 x 240 ST7789
//#define ST7789_IS_240X240 //  1.3" 240 x 240 ST7789
//#define ST7789_IS_240X320	//    2" 240 x 320 ST7789
#define ST7789_IS_170X320   //  1.9" 170 x 320 ST7789

#define RGB565(r, g, b)         (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

//--- color------------------------------
#define   	ST7789_BLACK   	0x0000
#define   	ST7789_BLUE    	0x001F
#define   	ST7789_RED     	0xF800
#define   	ST7789_GREEN   	0x07E0
#define 	ST7789_CYAN    	0x07FF
#define 	ST7789_MAGENTA 	0xF81F
#define 	ST7789_YELLOW  	0xFFE0
#define 	ST7789_WHITE   	0xFFFF
// ST7789 ----
#define ST7789_ColorMode_65K    0x50
#define ST7789_ColorMode_262K   0x60
#define ST7789_ColorMode_12bit  0x03
#define ST7789_ColorMode_16bit  0x05
#define ST7789_ColorMode_18bit  0x06
#define ST7789_ColorMode_16M    0x07
//------------------------------------------------
#define ST7789_MADCTL_MY  		0x80
#define ST7789_MADCTL_MX  		0x40
#define ST7789_MADCTL_MV  		0x20
#define ST7789_MADCTL_ML  		0x10
#define ST7789_MADCTL_RGB 		0x00
#define ST7789_MADCTL_BGR 		0x08
#define ST7789_MADCTL_MH  		0x04
//-------------------------------------------------
#define ST7789_SWRESET 0x01
#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_NORON   0x13
#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36
//-----------------------------------------------
#define DELAY 0x80


#ifdef ST7789_IS_240X240

	#define ST7789_WIDTH  			240
	#define ST7789_HEIGHT 			240
	#define ST7789_XSTART 			0
	#define ST7789_YSTART 			0
	#define ST7789_ROTATION 		(ST7789_MADCTL_MV|ST7789_MADCTL_ML|ST7789_MADCTL_MH)

#endif


#ifdef ST7789_IS_135X240

	#define ST7789_WIDTH  			135
	#define ST7789_HEIGHT 			240
	#define ST7789_XSTART 			0
	#define ST7789_YSTART 			0
	#define ST7789_ROTATION 		(ST7789_MADCTL_MV|ST7789_MADCTL_ML|ST7789_MADCTL_MH)

#endif


#ifdef ST7789_IS_240X320

	#define ST7789_WIDTH  			240
	#define ST7789_HEIGHT 			320
	#define ST7789_XSTART 			0
	#define ST7789_YSTART 			0
	#define ST7789_ROTATION 		(ST7789_MADCTL_MV|ST7789_MADCTL_ML|ST7789_MADCTL_MH)

#endif

#ifdef ST7789_IS_170X320

	#define ST7789_WIDTH  			170
	#define ST7789_HEIGHT 			320
	#define ST7789_XSTART 			0
	#define ST7789_YSTART 			0
	#define ST7789_ROTATION 		(ST7789_MADCTL_MV|ST7789_MADCTL_ML|ST7789_MADCTL_MH)

#endif

void ST7789_Init(void);
void ST7789_Select(void);
void ST7789_Unselect(void);
//static void ST7789_ExecuteCommandList(const uint8_t *addr);
void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void ST7789_HardReset(void);
void ST7789_SendCmd(uint8_t Cmd);
void ST7789_SendData(uint8_t Data );
void ST7789_SendDataMASS(uint8_t* buff, size_t buff_size);
void ST7789_SleepModeEnter( void );
void ST7789_SleepModeExit( void );
void ST7789_ColorModeSet(uint8_t ColorMode);
void ST7789_MemAccessModeSet(uint8_t Rotation, uint8_t VertMirror, uint8_t HorizMirror, uint8_t IsBGR);
void ST7789_InversionMode(uint8_t Mode);
void ST7789_FillScreen(uint16_t color);
void ST7789_Clear(void);
void ST7789_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void ST7789_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789_RamWrite(uint16_t *pBuff, uint32_t Len);
//static void ST7789_ColumnSet(uint16_t ColumnStart, uint16_t ColumnEnd);
//static void ST7789_RowSet(uint16_t RowStart, uint16_t RowEnd);
void ST7789_SetBL(uint8_t Value);
void ST7789_DisplayPower(uint8_t On);
void ST7789_DrawRectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
//static void SwapInt16Values(int16_t *pValue1, int16_t *pValue2);
void ST7789_DrawRectangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fillcolor);
//static void ST7789_DrawLine_Slow(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void ST7789_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void ST7789_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void ST7789_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void ST7789_DrawPixel(int16_t x, int16_t y, uint16_t color);
void ST7789_DrawCircleFilled(int16_t x0, int16_t y0, int16_t radius, uint16_t fillcolor);
void ST7789_DrawCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color);
void ST7789_DrawChar(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, unsigned char ch);
void ST7789_print(uint16_t x, uint16_t y, uint16_t TextColor, uint16_t BgColor, uint8_t TransparentBg, FontDef_t* Font, uint8_t multiplier, char *str);
void ST7789_rotation( uint8_t rotation );
void ST7789_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);


#endif /* INC_ANGLAS_ST7789_H_ */
