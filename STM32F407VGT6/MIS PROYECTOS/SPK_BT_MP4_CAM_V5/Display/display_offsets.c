/*
 *  Copyright (C)2023, VadRov, all right reserved.
 *
 *  Display Driver v1.4+ (includes fast assembler subroutines).
 *  Contains variable offsets in the LCD_Handler structure for use from an assembler program.
 *
* Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
  *
  * Generating header file display_offsets.h with variable offsets in the LCD_Handler structure.
  * This C program is first translated into an assembler program by the compiler, and then by the assembler
  * will generate the header file display_offsets.h, highlighting display_offsets.s in the file according to the specified
  * template (#define name #value) the required lines and converting them to C macro format (#define name value).
  *
  * To enable auto-generation of a header file in the C/C++ project settings Build -> Settings -> Build Steps
  * in the Command column of the Pre-build steps section you must write the following command:
  * make -f ../Display/makefile
  * Thus, before the project build stage, the header file display_offsets.h will be automatically generated,
  * containing macros with offsets of LCD_Handler structure variables. These offsets are used by the functions
  * display driver implemented in assembler (provided they are connected with the LCD_USE_ASSEMBLER option
  * in the header file display_config.h).
  *
  * It will be necessary to adjust the core and include build variables in the makefile (Display directory),
  * having written in it the parameters corresponding to the microcontroller used (kernel and drivers for it).
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */

#include "display.h"
#include <stddef.h>

//Macro for forming a string in an assembler program using a template: #define name #value
#define DEFINE(sym, val) asm volatile("\n#define " #sym " %0 " : : "i" (val))
#define OFFSET(sym, str, var) DEFINE(sym, offsetof(str, var))

void LCD_GenerateOffsets(void)
{
	//We create macros that determine the offsets of the used variables of the LCD_Handler structure
	OFFSET(lcd_width, LCD_Handler, Width);
	OFFSET(lcd_height, LCD_Handler, Height);
	OFFSET(lcd_x_offs, LCD_Handler, x_offs);
	OFFSET(lcd_y_offs, LCD_Handler, y_offs);
	OFFSET(lcd_spi, LCD_Handler, spi_data.spi);
	OFFSET(lcd_dma_ctrl, LCD_Handler, spi_data.dma_tx.dma);
	OFFSET(lcd_dma_strm, LCD_Handler, spi_data.dma_tx.stream);
	OFFSET(lcd_dc_port, LCD_Handler, spi_data.dc_port);
	OFFSET(lcd_dc_pin, LCD_Handler, spi_data.dc_pin);
	OFFSET(lcd_cs_port, LCD_Handler, spi_data.cs_port);
	OFFSET(lcd_cs_pin, LCD_Handler, spi_data.cs_pin);
	OFFSET(lcd_sz_mem, LCD_Handler, size_mem);
	OFFSET(lcd_cs_ctrl, LCD_Handler, cs_control);
	OFFSET(lcd_dc_ctrl, LCD_Handler, dc_control);
	OFFSET(lcd_fill_clr, LCD_Handler, fill_color);
}
