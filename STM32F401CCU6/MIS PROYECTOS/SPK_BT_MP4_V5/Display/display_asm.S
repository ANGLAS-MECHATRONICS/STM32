/*
  * Display control driver via SPI
  * Author: VadRov
  * Copyright (C) 2019 - 2023, VadRov, all rights reserved.
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
  *
  *Version: for STM32F4
  * Assembly (“turbo”) version.

 *  Включает следующие функции драйвера дисплея, реализованные на ассемблере:
 *  void LCD_SetActiveWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 *  void LCD_WriteData(LCD_Handler *lcd, uint16_t *data, uint32_t len)
 *  void LCD_WriteDataDMA(LCD_Handler *lcd, uint16_t *data, uint32_t len)
 *  void LCD_FillWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
 *  void LCD_DrawImage(LCD_Handler* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data, uint8_t dma_use_flag)
 *  void Display_TC_Callback(DMA_TypeDef *dma_x, uint32_t stream)
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

.syntax unified
.cpu cortex-m4
.thumb

#include "display_config.h"

.if LCD_USE_ASSEMBLER

#include "display_offsets.h"

//void LCD_SetActiveWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
//sets the output to 16-bit spi frame format
		.section  .text.LCD_SetActiveWindow
		.weak  LCD_SetActiveWindow
		.type  LCD_SetActiveWindow, %function
LCD_SetActiveWindow:
 		//r0 = lcd, r1 = x1, r2 = y1, r3 = x2, [sp + 0] = y2
 		push 	{r4, r5, lr}  			//save registers sp = sp - 12
 		//-----------form a list of commands with parameters on the stack ----------
		//the list is formed in reverse order (according to the stack rule:
		//LIFO (англ. last in, first out, "last to come, first to leave")
		movs	r5, #0x2c				//display controller command 0x2c
		ldrh	lr, [r0, #lcd_y_offs]	//y_offs from Lcd handler
		ldrh	r4, [sp, #12] 			//y2 = [sp + 12] from the stack
		add		r4, r4, lr				//y2 = y2 + y_offs
		add		r2, r2, lr				//y1 = y1 + y_offs
		push 	{r2, r4, r5}	 		//в стек: 0x2c, y2, y1
		movs	r5, #0x2b				//from the stack
		ldrh	lr, [r0, #lcd_x_offs]	//x_offs from Lcd handler
		add		r4, r3, lr				//x2 = x2 + x_offs
		add		r3, r1, lr				//x1 = x1 + x_offs
		movs	r2, 0x2a				//display controller command 0x2a
		push 	{r2, r3, r4, r5}		//в стек: 0x2b, x2, x1, 0x2a
		ldr		lr, [r0, #lcd_spi]		//spi from LCD handler
		ldr		r3, [r0, #lcd_cs_port]	//cs port from Lcd handler
		ldr 	r4, [r0, #lcd_dc_port]	//dc port from Lcd handler
		ldrh	r5, [r0, #lcd_cs_pin]	//pin cs from Lcd handler
		ldrh	r12, [r0, #lcd_dc_pin]	//dc pin from Lcd handler
.LCD_SetWin1:							//waiting for spi release
		ldr 	r0, [lr, #0x00] 		//read register CR1
		movs	r1, r0					//save CR1 in r1 for later use
		ands	r0, r0, #64  			//check the status of the SPE reg bit. CR1
		bne		.LCD_SetWin1 			//if the bit is set, then we go to re-polling
		//------------------- настройка spi -------------------
		bic		r1, r1, #0b1010110000000000 //clear: BIDIMODE (15 bits), CRCEN (13 bits), DFF (11 bits), RXONLY (10 bits)
		orr		r1, r1, #64					//setting SPE to 1 (6 bits) - enabling spi
		str		r1, [lr, #0x00] 			//entry in reg. CR1
		//-----------------------------------------------------
		cbz		r3, .LCD_SetWin1_1		//check: is cs supported?
		mov		r0, r5, lsl #16			//cs low
		str		r0, [r3, #0x18] 		//write to BSRR port register
.LCD_SetWin1_1:
		mov		r0, r12, lsl #16		//dc low
		str		r0, [r4, #0x18]			//write to BSRR port register
		pop		{r0}					//"pop the command from the stack
		strb	r0, [lr, #0x0c]			//writing a command to the DR register
.LCD_SetWin2:							//waiting to be ready to receive new spi data
		ldr 	r1, [lr, #0x08] 		//read from SR register
		ands	r1, r1, #2 				//check 1 bit (TXE flag)
		beq		.LCD_SetWin2			//While the bit is cleared, we continue to poll the status
.LCD_SetWin3:
		ldr 	r1, [lr, #0x08]			//read from SR register
		ands	r1, r1, #128 			//check bit 7 (BSY flag)
		bne		.LCD_SetWin3			//While the bit is set, continue polling the status
		cmp		r0, #0x2c				//last command (no parameters)?
		beq		.LCD_SetWin6_1			//let's go to completion
		str		r12, [r4, #0x18]		//dc hi
		movs	r2, #2					//r2 = 2, loop through the number of command parameters
.LCD_SetWin3_1:
		pop		{r0}					//"pop the parameter from the stack
		mov 	r1, r0, lsr #8			//x1 = x >> 8
		strb	r1, [lr, #0x0c]			//transmit the high byte of the parameter
.LCD_SetWin4:
		ldr 	r1, [lr, #0x08] 		//read from SR register
		ands	r1, r1, #2 				//check 1 bit (TXE flag)
		beq		.LCD_SetWin4
		strb	r0, [lr, #0x0c]			//transmit the low byte of the parameter
.LCD_SetWin5:
		ldr 	r1, [lr, #0x08]			//wait for the TXE flag
		lsls	r1, r1, #2
		beq		.LCD_SetWin5
		subs	r2, r2, #1				//decrease the counter by updating the flags
		bne		.LCD_SetWin3_1			//the counter is not zero -> pass the second command parameter
.LCD_SetWin6:
		ldr 	r1, [lr, #0x08]			//waiting for the BSY flag
		ands	r1, r1, #128
		bne		.LCD_SetWin6
		b		.LCD_SetWin1_1			//go to next command
.LCD_SetWin6_1:
		cbz		r3, .LCD_SetWin7		//check: is cs supported?
		str		r5, [r3, #0x18]			//cs hi
.LCD_SetWin7:
		ldr 	r0, [lr, #0x00]			//read reg. CR1 spi
		bic		r0, r0, #64				//reset page 6 (SPE)
		orr		r0, r0, #2048			//install 11 bit (DEF) - 16-bit frame spi
		str 	r0, [lr, #0x00]			//write down reg. CR1 spi
		pop 	{r4, r5, pc}			//restore the registers, the return address from the subroutine is placed in pc
.size  LCD_SetActiveWindow, .-LCD_SetActiveWindow

//void LCD_WriteData(LCD_Handler *lcd, uint16_t *data, uint32_t len)
//input spi frame must be 16 bits (LCD_SetActiveWindow sets 16 bit spi frame on output)
		.section  .text.LCD_WriteData
		.weak  LCD_WriteData
		.type  LCD_WriteData, %function
LCD_WriteData:
		//r0 = lcd, r1 = data, r2 = len
		push 	{r4, r5, lr}			//save registers/save registers
		eors	r5, r5					//flag - to copy mode (memory address increment is enabled)
.LCD_WriteData0:
		ldr		lr, [r0, #lcd_spi]		//get the spi address from the lcd handler
.LCD_WriteData1:						//wait for spi to be released if it is busy
		ldr 	r3, [lr, #0x00] 		//read register CR1
		mov		r12, r3					//save CR1 in r1 for later use
		ands	r3, r3, #64  			//check the status of the SPE reg bit. CR1 (6 bits), 64 defines 6 bits
		bne		.LCD_WriteData1			//if the bit is set (not zero), then continue waiting
		orr		r12, r12, #64			//set 6 bits (SPE) - enable spi, 64 defines 6 bits
		str 	r12, [lr, #0x00]		//write in reg. CR1 spi
		ldr		r3, [r0, #lcd_cs_port]	//get the cs output port address from the lcd handler
		cbz		r3, .LCD_WriteData2		//check: is cs supported (checking port address to zero)?
		ldrh	r12, [r0, #lcd_cs_pin]	//get the cs pin number from the lcd handler
		mov		r4, r12, lsl #16		//cs low (pin bit offset 16 bits to the left)
		str		r4, [r3, #0x18] 		//writing value from r4 to BSRR port register
.LCD_WriteData2:
		ldr 	r4, [r0, #lcd_dc_port]	//get the dc output port address from the handler lcd
		ldrh	r0, [r0, #lcd_dc_pin]	//get the dc pin number from the handler lcd
		str		r0, [r4, #0x18]			//dc hi, write to BSRR port register
.LCD_WriteData3:
		ldrh	r0, [r1, #0]			//take a half word at the address in r1
		str		r0, [lr, #12]			//and put it in the reg. DR spi
		orrs	r5, r5					//checking the copy/fill flag to zero
		cbnz	r5, .LCD_WriteData4		//if not zero, then skip the address increment in r1
		adds	r1, #2					//if 0 (copy mode), then increase the address in r1 by 2 (copying half-words)
.LCD_WriteData4:
		ldr 	r0, [lr, #0x08]			//wait for the TXE flag (1 bit)
		ands	r0, r0, #2				//2 defines 1 bit
		beq		.LCD_WriteData4			//if zero
		subs	r2, #1					//decrement the counter
		bne		.LCD_WriteData3			//the counter is not zero
.LCD_WriteData5:
		ldr 	r0, [lr, #0x08]			//wait for the BSY flag (bit 7)
		ands	r0, r0, #128			//128 defines 7 bits
		bne		.LCD_WriteData5   		//if not zero, then continue waiting
		cbz		r3, .LCD_WriteData6		//check: is cs supported?
		str		r12, [r3, #0x18]		//cs hi, write to BSRR port register
.LCD_WriteData6:
		ldr 	r0, [lr, #0x00]			//read reg. CR1 spi
		bic		r0, r0, #64				// reset 6 bits (SPE), 64 specifies 6 bits
		str 	r0, [lr, #0x00]			//write down reg. CR1 spi
		pop 	{r4, r5, pc}			//restore the registers, the return address from the subroutine is placed in pc
.size  LCD_WriteData, .-LCD_WriteData

//void LCD_WriteDataDMA(LCD_Handler *lcd, uint16_t *data, uint32_t len)
//input spi frame must be 16 bits (LCD_SetActiveWindow sets 16 bit spi frame on output)
		.section  .text.LCD_WriteDataDMA
		.weak  LCD_WriteDataDMA
		.type  LCD_WriteDataDMA, %function
LCD_WriteDataDMA:
		//r0 = lcd, r1 = data, r2 = len
		ldr		r12, [r0, #lcd_dma_ctrl]//dma from lcd handler
		orrs	r12, r12				//checking: dma == 0 ?
		beq		LCD_WriteData			//if dma == 0, then proceed to the transfer procedure without DMA
		push 	{r4, r5, lr}			//save registers
		eors	r5, r5					//flag - to copy mode (memory address increment is enabled)
.LCD_WriteDataDMA0:
		ldr		lr, [r0, #lcd_spi]		//spi from lcd handler
.LCD_WriteDataDMA1:						//waiting for spi to be released if it is busy
		ldr 	r3, [lr, #0x00] 		//read register CR1
		movs	r4, r3					//save CR1 in r1 for later use//save CR1 in r1 for later use
		ands	r3, r3, #64  			//check the state of the SPE reg bit. CR1
		bne		.LCD_WriteData1			//if the bit is set (not zero), then continue waiting
		orr		r4, r4, #64				//set 6 bits (SPE) - enable spi
		str 	r4, [lr, #0x00]			//write to reg. CR1 spi
		ldr		r4, [lr, #0x04]			//read reg. CR2 spi
		orr		r4, r4, #2				//set bit 1 (TXDMAEN) CR2
		str 	r4, [lr, #0x04]			//write to reg. CR2 spi
		ldr		r3, [r0, #lcd_cs_port]	//cs port from lcd handler
		cbz		r3, .LCD_WriteDataDMA2	//check: is cs supported?
		ldrh	r4, [r0, #lcd_cs_pin]	//pin cs from lcd handler
		movs	r4, r4, lsl #16			//cs low
		str		r4, [r3, #0x18] 		//write to BSRR port register
.LCD_WriteDataDMA2:
		ldr 	r3, [r0, #lcd_dc_port]	//dc port from lcd handler
		ldrh	r4, [r0, #lcd_dc_pin]	//dc pin from lcd handler
		str		r4, [r3, #0x18]			//dc hi
		add		r4, r0, #lcd_sz_mem		//in r4 the address of the variable lcd->size_mem
		ldr		r0, [r0, #lcd_dma_strm]	//DMA stream number from lcd handler
		movs 	r3, #24					//multiplier for formula: stream address = address DMA + 24 * thread number + 16
		mla		r12, r0, r3, r12		//r12 = r12 + r0 * r3
		add		r0, r12, #16			//r0 = r12 + 16, в r0 - stream address DMA
		movw	r3, #0xffff				//r3 write to 0xffff
		cmp		r2, r3					//r2 > 65535 ?
		itee	gt						//execution of an IT block of 3 commands:
		subgt	r2, r2, r3				//all that remains is to convey len = len - 65535, for the current transmission 65535
		movle	r3, r2					//for the current transmission len
		movle	r2, #0x00				//all that remains is to convey 0
		str		r2, [r4]				//writing to a handler variable lcd->size_mem how much data remains to be transferred
		str		r3, [r0, #4]			//writing to the NDTR DMA register the amount of data for the current transfer
		add		lr, lr, #0x0c			//DR spi register address, 0x0c offset from spi base address defining DR

		str		lr, [r0, #8]			//entry in registry. PAR DMA registry address DR spi (spi->DR)
		str		r1, [r0, #12]			//entry in registry. M0AR DMA data block addresses for transmission (data)
		ldr		r1, [r0, #0]			//reading reg. CR DMA
		orrs	r5, r5					//copy/fill flag
		ite		eq						//IT-block by type if zero then else
		orreq	r1, r1, #1024			//installation 10 (MINC) bits - mode for copying a data block to the display (flag in r5 = 0)
		bicne	r1, r1, #1024			//reset bit 10 (MINC) - fill mode with one color (flag r5 != 0)
		orr		r1, r1, #1				//setting bit 1 (CR_EN) - enable/enable DMA stream
		bic		r1, r1, #512			//reset bit 9 (PINC) - peripheral address increment is disabled
		str		r1, [r0, #0]			//writing a parameter to reg. CR DMA
		pop 	{r4, r5, pc}			//we restore the registers, the return address from the subroutine is placed in pc
.size  LCD_WriteDataDMA, .-LCD_WriteDataDMA

//void LCD_FillWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
		.section  .text.LCD_FillWindow
		.weak  LCD_FillWindow
		.type  LCD_FillWindow, %function
LCD_FillWindow:
		//r0 = lcd, r1 = x1, r2 = y1, r3 = x2, [sp + 0] = y2, [sp + 4] = color
		push	{r4, r5, lr}			//save registers, sp = sp - 12
		cmp		r1, r3					//x1 > x2 ? Sort Ascending
		ittt	gt						//IT - block
		movgt	lr, r1					//lr = r1, lr = x1
		movgt	r1, r3					//r1 = r3, x1 = x2
		movgt	r3, lr					//r3 = lr, x2 = lr
		ldrh	r4, [sp, #12]			//get the parameter from the stack y2
		cmp		r2, r4					//y1 > y2 ?  Sort Ascending
		ittt	gt						//IT - block
		movgt	lr, r2					//lr = r2, lr = y1
		movgt	r2, r4					//r2 = r4, y1 = y2
		movgt	r4, lr					//r4 = lr, y2 = lr
		ldrh	lr, [r0, #lcd_width]	//lcd->Width
		sub		lr, lr, #1				//lcd->Width - 1
		cmp		r1, lr					//x1 > lcd->Width - 1 ?
		bgt		.LCD_FillWindow_exit	//output if x1 goes beyond display boundaries
		ldrh	r12, [r0, #lcd_height]	//lcd->Height
		sub		r12, r12, #1			//lcd->Height - 1
		cmp		r2, r12					//y1 > lcd->Height - 1
		bgt		.LCD_FillWindow_exit	//output if y1 goes beyond display boundaries
		cmp		r3, lr					//x2 > lcd->Width - 1
		it		gt						//"cut off" x2 if it goes beyond the display boundaries
		movgt	r3, lr					//x2 = lcd->Width - 1
		cmp		r4, r12					//y2 > lcd->Height - 1
		it		gt						//“cut off” y2 if it goes beyond the display boundaries
		movgt	r4, r12					//y2 = lcd->Height - 1
		sub		sp, #8					//sp = sp - 8 (convention for passing parameters through the stack)
		str		r4, [sp, #0]			//push y2 to the top of the stack
		//--------------  Расчет len = (x2 - x1 + 1) * (y2 - y1 + 1) ---------------------------
		subs	r5, r3, r1				//r5 = x2 - x1
		add 	r5, #1					//r5 = r5 + 1
		subs	r4, r4, r2				//r4 = y2 - y1
		mla		r5, r5, r4, r5			//r5 = r5 * r4 + r5 - saved on 1 team add r4, #1
		//--------------------------------------------------------------------------------------
		movs	r4, r0					//save r0 in r4 (so as not to lose the lcd address)
		bl		LCD_SetActiveWindow 	//setting the output window: transmit r0(lcd), r1(x1), r2(y1), r3(x2), [sp+0](y2)
		add		sp, #8					//restoring the stack pointer
		ldr		r0, [sp, #16]			//get the parameter from the stack color (R8G8B8)
		//------------- color conversion R8G8B8 в R5G6B5 --------------------------------------------------
		ubfx	r1, r0, #3, #5			//copy 5 bits of component b to r1 starting from bit 3
		ubfx	r2, r0, #10, #6			//copy 6 bits of component g to r2 starting from bit 10
		orr		r1, r1, r2, lsl #5		//combine r1 with r2, shifted 5 bits to the left
		ubfx	r2, r0, #19, #5			//copy 5 bits of component r to r2 starting from bit 19
		orr		r1, r1, r2, lsl #11 	//combine r1 with r2, shifted 11 bits to the left
		//------------------------------------------------------------------------------------------------------
		strh	r1, [r4, #lcd_fill_clr]	//write the color to the lcd->fill_color variable (address of the lcd variable + #104)
		movs	r0, r4					//r0 = lcd
		add		r1,	r0, #lcd_fill_clr	//r1 = &lcd->fill_color
		movs	r2, r5					//r2 = len
		movs	r5, #1					//flag for LCD_WriteDataDMA/LCD_WriteData
		ldr		r12, [r0, #lcd_dma_ctrl]//dma from handler lcd
		orrs	r12, r12            	//check for 0 in r12 (zero in r12 if DMA is not supported)
		beq		.LCD_WriteData0			//Entry point to procedure LCD_WriteData (with flag 1 in r5)
		b		.LCD_WriteDataDMA0  	//entry point into the LCD_WriteDataDMA procedure (with flag 1 in r5)
.LCD_FillWindow_exit:
		pop		{r4, r5, pc}			//we restore the registers, the return address from the subroutine is placed in pc
.size  LCD_FillWindow, .-LCD_FillWindow

//void LCD_DrawImage(LCD_Handler* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data, uint8_t dma_use_flag)
		.section  .text.LCD_DrawImage
		.weak  LCD_DrawImage
		.type  LCD_DrawImage, %function
LCD_DrawImage:
		//r0 = lcd, r1 = x, r2 = y, r3 = w, [sp + 0] = h, [sp + 4] = data, [sp + 8] = dma_use_flag
		push	{r4, r5, lr}			//save registers
		ldrh	lr, [r0, #lcd_width]	//lr = lcd->Width
		cmp		r1, lr					//x >= lcd->Width ?
		bcs		.LCD_DrawImage_exit 	//x outside the range - we go out
		add		r12, r1, r3				//r12 = x + w
		cmp		r12, lr					//x + w > lcd->Width ?
		bhi		.LCD_DrawImage_exit 	//x outside the range - we go out
		ldrh	lr, [r0, #lcd_height]	//lr = lcd->Height
		cmp		r2, lr					//y >= lcd->Height ?
		bcs		.LCD_DrawImage_exit		//y outside the range - we go out
		ldr		r4, [sp, #12]			//r4 = h
		adds	r5, r2, r4				//r5 = y + h
		cmp		r5, lr					//y + h > lcd->Height ?
		bhi		.LCD_DrawImage_exit		//y outside the range - we go out
		mul		r4, r4, r3				//r4 = w * h
		subs	r5, #1					//r5 = y + h - 1
		sub		sp, #8					//sp = sp - 8 --> move the stack pointer up to record the passed parameter
		str		r5, [sp, #0]			//r5 (y + h - 1) - write to the top of the stack
		sub		r3, r12, #1				//r3 = x + w - 1
		movs	r5, r0					//r5 = r0 = lcd
		bl		LCD_SetActiveWindow 	//r0 = lcd, r1 = x, r2 = y, r3 = x + w - 1, [sp + 0] = y + h - 1
		add		sp, #8					//restoring the stack pointer
		movs	r0, r5					//r0 = r5 = lcd
		ldr		r1, [sp, #16]			//r1 = data - data address to transfer (parameter from the stack)
		movs	r2, r4					//r2 = w * h
		ldr		r3, [sp, #20]			//r3 = dma_use_flag - flag defining the use of DMA (parameter from the stack)
		pop		{r4, r5, lr}			//restoring registers from the stack
		orrs	r3, r3					//usage check DMA: dma_use_flag == 0 ?
		beq		LCD_WriteData			//==0: go to LCD_WriteData - without DMA
		b		LCD_WriteDataDMA		//go to LCD_WriteDataDMA	- with DMA
.LCD_DrawImage_exit:
		pop		{r4, r5, pc}			//we restore the registers, the return address from the subroutine is placed in pc
.size  LCD_DrawImage, .-LCD_DrawImage

//void Display_TC_Callback(DMA_TypeDef *dma_x, uint32_t stream)
		.section  .text.Display_TC_Callback
		.weak  Display_TC_Callback
		.type  Display_TC_Callback, %function
Display_TC_Callback:
		//r0 = dma_x, r1 = stream
		push	{lr}					//store the return address from the subroutine on the stack
		movs	r3, r1					//in r3 stream number stream DMA (0...7)
		cmp		r3, #3					//checking: stream > 3 ?
		itte	gt						//it-block like if greater than then else
		subgt	r3, #4					//stream = stream - 4
		movgt	lr, #0x0c				//address offset for register HIFCR DMA
		movle	lr, #0x08				//address offset for register LIFCR DMA
		adr		r12, .DMA_ShiftTbl		//register flag bit position table address IFCR (hi и low)
		ldrb	r12, [r12, r3]			//obtaining the starting position of register flag bits from the table IFCR
		movs	r2, #0x3f				//flag drop mask
		mov		r2, r2, lsl r12			//shift the mask to the location of the flags to be cleared
		str		r2, [r0, lr]			//write the mask in reg. IFCR
		movs	r2, #24					//multiplier for formula: stream address = address DMA + 24 * thread number + 16)
		mla		r0, r1, r2, r0			//r0 = r0(dma) + r1(stream) * r2(24)
		add		r0, #16					//r0 = r0(dma) + 16, at the output in r0 the address of the DMA stream
		ldr		r1, [r0, #0]			//reading the CR register of a DMA stream
		bic		r1, r1, #1				//reset the CR_EN bit (turn off the stream)
		str		r1, [r0, #0]			//write in reg. CR
.Display_TC_Callback1:					//waiting for the stream to turn off DMA
		ldr		r1, [r0, #0]			//reading the CR register of a DMA stream
		ands	r1, r1, #1				//check bit 0 (CR_EN)
		bne		.Display_TC_Callback1	//if not zero, then we go to a new circle, reading the state of the reg. CR DMA stream
		ldr		r3, =LCD				//determine the location address of the display list variable
		ldr		r3, [r3]				//“pull out” the pointer to the first display in the list
		ldr		r2, [r3, #lcd_sz_mem]	//in r2 the value of the lcd->size_mem variable from the lcd handler
		cbz		r2, .Display_TC_Callback2	//if there is no more data to transfer, then proceed to complete the transfer
		movw	r1, 0xffff				//write to r1 0xffff (65535)
		cmp		r2, r1					//compare r2 with r1 (we are interested in: r2 > 65535 or not)
		itee	gt						//execution of IT block by type: if more then else
		subgt	r2, r2, r1				//remainder len = len - 65535, for the current transmission 65535
		movle	r1, r2					//to transfer len
		movle	r2, 0					//remains to be conveyed 0
		str		r1, [r0, #4]			//writing r1 to register NDTR DMA
		str		r2, [r3, #lcd_sz_mem]	//writing r3 to the lcd->size_mem variable (remaining for the next transfer)
		ldr		r1, [r0]				//reading reg. CR DMA
		orr		r1, r1, #1				//setting bit CR_EN reg. CR DMA (stream enable)
		str		r1, [r0]				//entry in registry. CR DMA
		pop		{pc}					//restore to reg. pc return address from the subroutine (the transition will occur automatically)
.Display_TC_Callback2:					//completion of transfer
		ldr		r0, [r3, #lcd_spi]		//in r0 - pointer to spi from Lcd handler
		ldr		r1, [r0, #0x04]			//read register CR2 spi
		bic		r1, r1, #2				//reset the TXDMAEN bit in the register CR2
		str		r1, [r0, #0x04]			//write to the register CR2
.Display_TC_Callback3:					//waiting for spi to be free
		ldr		r1, [r0, #0x08]			//read register SR spi
		ands	r1, r1, #128			//check the state of the bit 7 (BSY) register SR spi
		bne		.Display_TC_Callback3	//if the bit is set (not zero), then spi is busy, and we repeat polling of the registration status. S.R.
		ldr		r2, [r3, #lcd_cs_port]	//cs port from handler Lcd
		cbz		r2, .Display_TC_Callback4	//if cs control is not supported, then we “skip” through 2 commands
		ldrh	r1, [r3, #lcd_cs_pin]	//pin cs from handler Lcd
		str		r1, [r2, #0x18]			//cs hi, write 1 in the corresponding digits of the reg. Port BSRR
.Display_TC_Callback4:
		ldr		r1, [r0, #0x00]			//read reg. CR1 spi
		bic		r1, r1, #64				//reset the bit 6 (SPE) рег. CR1 spi
		str		r1, [r0, #0x00]			//write in CR1 spi
		pop		{pc}					//restore to reg. pc return address from the subroutine (the transition will occur automatically)
.DMA_ShiftTbl:							//table of starting positions of flag bits of DMA reg flows. IFCR (HIFCR, LIFCR)
		.byte	0, 6, 16, 22
.size  Display_TC_Callback, .-Display_TC_Callback

.endif
		.end
