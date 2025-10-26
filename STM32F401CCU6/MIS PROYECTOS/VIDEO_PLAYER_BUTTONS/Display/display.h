/*
* Display control driver via SPI
  * Author: VadRov
  * Copyright (C) 2019 - 2022, VadRov, all rights reserved.
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
 *
 *  Версия: 1.4 Turbo (CMSIS, LL, assembler) для STM32F4
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "main.h"
#include "fonts.h"
#include "display_config.h"

//some predefined colors
//format 0xRRGGBB
#define	COLOR_BLACK			0x000000
#define	COLOR_BLUE			0x0000FF
#define	COLOR_RED			0xFF0000
#define	COLOR_GREEN			0x00FF00
#define COLOR_CYAN			0x00FFFF
#define COLOR_MAGENTA		0xFF00FF
#define COLOR_YELLOW		0xFFFF00
#define COLOR_WHITE			0xFFFFFF
#define COLOR_NAVY			0x000080
#define COLOR_DARKGREEN		0x2F4F2F
#define COLOR_DARKCYAN		0x008B8B
#define COLOR_MAROON		0xB03060
#define COLOR_PURPLE		0x800080
#define COLOR_OLIVE			0x808000
#define COLOR_LIGHTGREY		0xD3D3D3
#define COLOR_DARKGREY		0xA9A9A9
#define COLOR_ORANGE		0xFFA500
#define COLOR_GREENYELLOW	0xADFF2F

#define LCD_UPR_COMMAND		0
#define LCD_UPR_DATA		1
#define LCD_UPR_PAUSE		2
#define LCD_UPR_END			3

//data width
typedef enum {
	LCD_DATA_UNKNOW_BUS,
	LCD_DATA_8BIT_BUS,
	LCD_DATA_16BIT_BUS
} LCD_DATA_BUS;

//display statuses
typedef enum {
	LCD_STATE_READY,
	LCD_STATE_BUSY,
	LCD_STATE_ERROR,
	LCD_STATE_UNKNOW
} LCD_State;

//display orientation
typedef enum {
	PAGE_ORIENTATION_PORTRAIT,			//portrait
	PAGE_ORIENTATION_LANDSCAPE,			//landscape
	PAGE_ORIENTATION_PORTRAIT_MIRROR,	//PORTRAIT_MIRROR
	PAGE_ORIENTATION_LANDSCAPE_MIRROR	//LANDSCAPE_MIRROR
} LCD_PageOrientation;

//character printing modes
typedef enum {
	LCD_SYMBOL_PRINT_FAST,		//quick with background erasing
	LCD_SYMBOL_PRINT_PSETBYPSET	//slow, point by point, without erasing the background
} LCD_PrintSymbolMode;

//data DMA
typedef struct {
	DMA_TypeDef *dma;
	uint32_t stream;
} LCD_DMA_TypeDef;

//spi connection data
typedef struct {
	SPI_TypeDef *spi;
	LCD_DMA_TypeDef dma_tx;
	GPIO_TypeDef *reset_port;
	uint16_t reset_pin;
	GPIO_TypeDef *dc_port;
	uint16_t dc_pin;
	GPIO_TypeDef *cs_port;
	uint16_t cs_pin;
} LCD_SPI_Connected_data;

//backlight
typedef struct {
	TIM_TypeDef *htim_bk;		//------- for backlight with PWM:- timer
	uint32_t channel_htim_bk;	//----------------------------- timer channel

	GPIO_TypeDef *blk_port;		//just to turn the backlight on and off if htim_bk = 0 (no PWM, output port defined)
	uint16_t blk_pin;			//----------------------------------------------------------------------- пин порта

	uint8_t bk_percent;			//backlight brightness for mode PWM, %
								//or 0 - backlight is disabled, > 0 backlight is enabled if htim_bk = 0 (without PWM, output port defined)
} LCD_BackLight_data;

//callback
typedef uint8_t* (*DisplayInitCallback)(uint8_t);
typedef uint8_t* (*DisplaySetWindowCallback)(uint16_t, uint16_t, uint16_t, uint16_t);
typedef uint8_t* (*DisplaySleepInCallback)(void);
typedef uint8_t* (*DisplaySleepOutCallback)(void);

//position stamps
typedef struct {
	uint16_t x;
	uint16_t y;
} LCD_xy_pos;

//display handler
typedef struct {
	uint16_t Width_Controller;    	//maximum matrix width supported by the display controller, pixels
	uint16_t Height_Controller;		//maximum matrix height supported by the display controller, pixels
	uint16_t Width;					//actual matrix width of the display used, pixels
	uint16_t Height;				//actual matrix height of the display used, pixels
	LCD_PageOrientation Orientation;//display orientation
	int16_t x_offs;					//x offset
	int16_t y_offs;					//y offset
	LCD_xy_pos AtPos;				//current character print position
	DisplayInitCallback Init_callback;					//initialization callback
	DisplaySetWindowCallback SetActiveWindow_callback;	//callback to set the output window
	DisplaySleepInCallback SleepIn_callback;			//callback "entry to sleep"
	DisplaySleepOutCallback SleepOut_callback;			//callback "exit from sleep"
	LCD_SPI_Connected_data spi_data;					//connection data via SPI
	LCD_DATA_BUS data_bus;								//data width
	LCD_BackLight_data bkl_data;						//backlight data
	uint16_t *tmp_buf;									//pointer to display buffer
	uint32_t size_mem;									//size of untransmitted data - for DMA restart
	uint8_t display_number;								//display number
	uint8_t cs_control;
	uint8_t dc_control;
	uint16_t fill_color;
	void *prev;					//pointer to previous display
	void *next;					//pointer to next display
} LCD_Handler;

extern LCD_Handler *LCD;		//pointer to the list of displays (first display in the list)

//callback (include in the interrupt handler of the DMA stream allocated for the display, see file stm32f4xx_it.c)
extern void Display_TC_Callback(DMA_TypeDef*, uint32_t);

void LCD_SetCS(LCD_Handler *lcd);
void LCD_ResCS(LCD_Handler *lcd);
void LCD_SetDC(LCD_Handler *lcd);
void LCD_ResDC(LCD_Handler *lcd);

//creates a display handler and adds it to the list of displays
//returns a pointer to the created display or 0 on failure
LCD_Handler* LCD_DisplayAdd(LCD_Handler *lcds,			/* pointer to the first display in the list or 0,
														   if there are no displays in the list yet */
#ifndef LCD_DYNAMIC_MEM
							LCD_Handler *lcd,	        /* pointer to the display handler to create
							 	 	 	 	 	 	 	   in case of static memory allocation */
#endif
							uint16_t resolution1,
							uint16_t resolution2,
							uint16_t width_controller,
							uint16_t height_controller,
							int16_t w_offs,
							int16_t h_offs,
							LCD_PageOrientation orientation,
							DisplayInitCallback init,
							DisplaySetWindowCallback set_win,
							DisplaySleepInCallback sleep_in,
							DisplaySleepOutCallback sleep_out,
							void *connection_data,
							LCD_DATA_BUS data_bus,
							LCD_BackLight_data bkl_data
					   );

//removes the display handler
void LCD_Delete(LCD_Handler* lcd);
//hard reset display
void LCD_HardWareReset(LCD_Handler* lcd);
//initialization display
void LCD_Init(LCD_Handler* lcd);
//command line interpreter
void LCD_String_Interpretator(LCD_Handler* lcd, uint8_t *str);
//setting display brightness
void LCD_SetBackLight(LCD_Handler* lcd, uint8_t bk_percent);
//returns the current display brightness
uint8_t LCD_GetBackLight(LCD_Handler* lcd);
//returns display width, pixels
uint16_t LCD_GetWidth(LCD_Handler* lcd);
//returns display height, pixels
uint16_t LCD_GetHeight(LCD_Handler* lcd);
//returns display status
LCD_State LCD_GetState(LCD_Handler* lcd);
//puts the display into sleep mode
void LCD_SleepIn(LCD_Handler* lcd);
//wakes the display from sleep mode
void LCD_SleepOut(LCD_Handler* lcd);
//sets the output window
void LCD_SetActiveWindow(LCD_Handler* lcd, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
//sends data to display - no DMA
void LCD_WriteData(LCD_Handler *lcd, uint16_t *data, uint32_t len);
//sends data to display using DMA
void LCD_WriteDataDMA(LCD_Handler *lcd, uint16_t *data, uint32_t len);
//fills the window with the given coordinates with the given color
void LCD_FillWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);
//fills the entire screen with the specified color
void LCD_Fill(LCD_Handler* lcd, uint32_t color);
//draws a point at the given coordinates with the given color
void LCD_DrawPixel(LCD_Handler* lcd, int16_t x, int16_t y, uint32_t color);
//draws a line at the given coordinates with the given color
void LCD_DrawLine(LCD_Handler* lcd, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color);
//draws a rectangle at the given coordinates with the given color
void LCD_DrawRectangle(LCD_Handler* lcd, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color);
//draws a filled rectangle at the given coordinates with the given color
void LCD_DrawFilledRectangle(LCD_Handler* lcd, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color);
//draws a triangle
void LCD_DrawTriangle(LCD_Handler* lcd, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint32_t color);
//draws a filled triangle
void LCD_DrawFilledTriangle(LCD_Handler* lcd, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint32_t color);
//draws a circle with a given center and radius with a given color
void LCD_DrawCircle(LCD_Handler* lcd, int16_t x0, int16_t y0, int16_t r, uint32_t color);
//draws a filled circle
void LCD_DrawFilledCircle(LCD_Handler* lcd, int16_t x0, int16_t y0, int16_t r, uint32_t color);
//sends a block of memory to the display (for example, a piece of an image)
void LCD_DrawImage(LCD_Handler* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data, uint8_t dma_use_flag);
//reads data from the display window with coordinates of the upper left corner (x, y), width w, height h into the data buffer
//data buffer format 16 bits (color R5G6B5)
void LCD_ReadImage(LCD_Handler* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);
//outputs the character at the specified position
void LCD_WriteChar(LCD_Handler* lcd, uint16_t x, uint16_t y, char ch, FontDef *font, uint32_t txcolor, uint32_t bgcolor, LCD_PrintSymbolMode modesym);
//prints a string of characters from the specified position
void LCD_WriteString(LCD_Handler* lcd, uint16_t x, uint16_t y, const char *str, FontDef *font, uint32_t color, uint32_t bgcolor, LCD_PrintSymbolMode modesym);
//converts color in R8G8B8 format (24 bits) to 16 bit R5G6B5
uint16_t LCD_Color(LCD_Handler *lcd, uint8_t r, uint8_t g, uint8_t b);
uint16_t LCD_Color_24b_to_16b(LCD_Handler *lcd, uint32_t color);

#endif /* INC_DISPLAY_H_ */
