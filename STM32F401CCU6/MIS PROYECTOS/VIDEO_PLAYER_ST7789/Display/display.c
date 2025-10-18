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
  * Version: 1.4 (on CMSIS and LL) for STM32F4
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

#include "display.h"
#include "fonts.h"
#include <string.h>
#ifdef LCD_DYNAMIC_MEM
#include "stdlib.h"
#endif

#define ABS(x) ((x) > 0 ? (x) : -(x))

#define min(x1,x2)	(x1 < x2 ? x1 : x2)
#define max(x1,x2)	(x1 > x2 ? x1 : x2)

#define min3(x1,x2,x3)	min(min(x1,x2),x3)
#define max3(x1,x2,x3)	max(max(x1,x2),x3)

#define LCD_CS_LOW		if (lcd->spi_data.cs_port) { lcd->spi_data.cs_port->BSRR = (uint32_t)lcd->spi_data.cs_pin << 16U; }
#define LCD_CS_HI		if (lcd->spi_data.cs_port) { lcd->spi_data.cs_port->BSRR = lcd->spi_data.cs_pin; }
#define LCD_DC_LOW		{lcd->spi_data.dc_port->BSRR = (uint32_t)lcd->spi_data.dc_pin << 16U; }
#define LCD_DC_HI		{lcd->spi_data.dc_port->BSRR = lcd->spi_data.dc_pin; }

#define DISABLE_IRQ		__disable_irq(); __DSB(); __ISB();
#define ENABLE_IRQ		__enable_irq();

LCD_Handler *LCD = 0; //display list

inline void LCD_SetCS(LCD_Handler *lcd)
{
	LCD_CS_HI
}

inline void LCD_ResCS(LCD_Handler *lcd)
{
	LCD_CS_LOW
}

inline void LCD_SetDC(LCD_Handler *lcd)
{
	LCD_DC_HI
}

inline void LCD_ResDC(LCD_Handler *lcd)
{
	LCD_DC_LOW
}

typedef enum {
	lcd_write_command = 0,
	lcd_write_data
} lcd_dc_select;

inline static void LCD_WRITE_DC(LCD_Handler* lcd, uint8_t data, lcd_dc_select lcd_dc)
{
	SPI_TypeDef *spi = lcd->spi_data.spi;
	if (lcd_dc == lcd_write_command)  {
		LCD_DC_LOW
	}
	else {
		LCD_DC_HI
	}
	*((volatile uint8_t *)&spi->DR) = data;
	while (!(spi->SR & SPI_SR_TXE)) { __NOP(); } //waiting for the end of the transmission
	while (spi->SR & SPI_SR_BSY)    { __NOP(); } //waiting for the SPI to become free
}

void LCD_HardWareReset (LCD_Handler* lcd)
{
	if (lcd->spi_data.reset_port) {
		lcd->spi_data.reset_port->BSRR = (uint32_t)lcd->spi_data.reset_pin << 16U;
		LL_mDelay(25);
		lcd->spi_data.reset_port->BSRR = lcd->spi_data.reset_pin;
		LL_mDelay(25);
	}
}

//interpreter of strings with control codes: “command”, “data”, “pause”, “end of package”
void LCD_String_Interpretator(LCD_Handler* lcd, uint8_t *str)
{
	SPI_TypeDef *spi = lcd->spi_data.spi;
	int i;
	while (LCD_GetState(lcd) == LCD_STATE_BUSY) { __NOP(); } //wait for the display to clear
	if (!lcd->cs_control) { LCD_CS_LOW }
	spi->CR1 &= ~ (SPI_CR1_BIDIMODE |  	//here we set the mode
				   SPI_CR1_RXONLY |   	//  Transmit only
				   SPI_CR1_CRCEN | 		//turn off hardware calculation CRC
				   SPI_CR1_DFF); 		//set 8-bit transmission
	spi->CR1 |= SPI_CR1_SPE; // SPI turn on
	while (1) {
		switch (*str++) {
			//control code "command"
			case LCD_UPR_COMMAND:
				//send the command code to the display controller
				LCD_WRITE_DC(lcd, *str++, lcd_write_command);
				//number of command parameters
				i = *str++;
				//send command parameters to the display controller
				while(i--) {
					LCD_WRITE_DC(lcd, *str++, lcd_write_data);
				}
				break;
			//control code "data"
			case LCD_UPR_DATA:
				//amount of data
				i = *str++;
				//send data to the display controller
				while(i--) {
					LCD_WRITE_DC(lcd, *str++, lcd_write_data);
				}
				break;
			//control code "pause"
			case LCD_UPR_PAUSE:
				//wait according to parameter (0...255)
				LL_mDelay(*str++);
				break;
			//control code "packet completion"
			case LCD_UPR_END:
			default:
				if (!lcd->cs_control) { LCD_CS_HI }
				//turn off spi
				spi->CR1 &= ~SPI_CR1_SPE;
				return;
		}
	}
}

//creating a display handler and adding it to the list of displays lcds
//returns a pointer to the created handler or 0 on failure
LCD_Handler* LCD_DisplayAdd(LCD_Handler *lcds,     /* pointer to list of displays
													  (first display in the list) */
#ifndef LCD_DYNAMIC_MEM
							LCD_Handler *lcd,	   /* pointer to the display handler to create
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
					   )
{

#ifdef LCD_DYNAMIC_MEM
	LCD_Handler* lcd = (LCD_Handler*)malloc(sizeof(LCD_Handler));
#endif
	if (!lcd) return 0;
	memset(lcd, 0, sizeof(LCD_Handler));
	LCD_DMA_TypeDef *hdma = 0;
	lcd->data_bus = data_bus;
	//initializing connection data
	lcd->spi_data = *((LCD_SPI_Connected_data*)connection_data);
	hdma = &lcd->spi_data.dma_tx;
	//setting DMA
	if (hdma->dma) {
		DMA_Stream_TypeDef *dma_x = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)hdma->dma + STREAM_OFFSET_TAB[hdma->stream])));
		dma_x->CR &= ~DMA_SxCR_EN; //turn off the channel DMA
		while(dma_x->CR & DMA_SxCR_EN) { __NOP(); } //waiting for the channel to turn off
		if (lcd->data_bus == LCD_DATA_8BIT_BUS) {
			dma_x->CR &= ~(DMA_SxCR_MSIZE | DMA_SxCR_PSIZE);
			dma_x->CR |= LL_DMA_MDATAALIGN_BYTE | LL_DMA_PDATAALIGN_BYTE;
		}
		else if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
			dma_x->CR &= ~(DMA_SxCR_MSIZE | DMA_SxCR_PSIZE);
			dma_x->CR |= LL_DMA_MDATAALIGN_HALFWORD | LL_DMA_PDATAALIGN_HALFWORD;
		}
		//disable interruptions for some tx transmission channel events and double buffer mode
		dma_x->CR &= ~(DMA_SxCR_DMEIE | DMA_SxCR_HTIE | DMA_SxCR_DBM | DMA_SxCR_TEIE);
		dma_x->FCR &= ~DMA_SxFCR_FEIE;
		//enable interrupt at the end of transmission
		dma_x->CR |= DMA_SxCR_TCIE;
		dma_x->CR &= ~DMA_SxCR_PINC; //peripheral address increment disabled

		dma_x->CR |= DMA_SxCR_MINC;  //memory address increment enabled
	}
	//setting display orientation and origin offset
	uint16_t max_res = max(resolution1, resolution2);
	uint16_t min_res = min(resolution1, resolution2);
	if (orientation==PAGE_ORIENTATION_PORTRAIT || orientation==PAGE_ORIENTATION_PORTRAIT_MIRROR) {
		lcd->Width = min_res;
		lcd->Height = max_res;
		lcd->Width_Controller = width_controller;
		lcd->Height_Controller = height_controller;
		if (orientation==PAGE_ORIENTATION_PORTRAIT) {
			lcd->x_offs = w_offs;
			lcd->y_offs = h_offs;
		}
		else {
			lcd->x_offs = lcd->Width_Controller - lcd->Width - w_offs;
			lcd->y_offs = lcd->Height_Controller - lcd->Height - h_offs;
		}
	}
	else if (orientation==PAGE_ORIENTATION_LANDSCAPE || orientation==PAGE_ORIENTATION_LANDSCAPE_MIRROR)	{
		lcd->Width = max_res;
		lcd->Height = min_res;
		lcd->Width_Controller = height_controller;
		lcd->Height_Controller = width_controller;
		if (orientation==PAGE_ORIENTATION_LANDSCAPE) {
			lcd->x_offs = h_offs;
			lcd->y_offs = lcd->Height_Controller - lcd->Height - w_offs;
		}
		else {
			lcd->x_offs = lcd->Width_Controller - lcd->Width - h_offs;
			lcd->y_offs = w_offs;
		}
	}
	else {
		LCD_Delete(lcd);
		return 0;
	}

	if (lcd->Width_Controller < lcd->Width ||
		lcd->Height_Controller < lcd->Height ||
		init==NULL ||
		set_win==NULL )	{
		LCD_Delete(lcd);
		return 0;
	}
	lcd->Orientation = orientation;
	lcd->Init_callback = init;
	lcd->SetActiveWindow_callback = set_win;
	lcd->SleepIn_callback = sleep_in;
	lcd->SleepOut_callback = sleep_out;
	lcd->bkl_data = bkl_data;
	lcd->display_number = 0;
	lcd->next = 0;
	lcd->prev = 0;
	if (!lcds) {
		return lcd;
	}
	LCD_Handler *prev = lcds;
	while (prev->next) {
		prev = (LCD_Handler *)prev->next;
		lcd->display_number++;
	}
	lcd->prev = (void*)prev;
	prev->next = (void*)lcd;
	return lcd;
}

//removes display
void LCD_Delete(LCD_Handler* lcd)
{
	if (lcd) {
#ifdef LCD_DYNAMIC_MEM
		if (lcd->tmp_buf) {
			free(lcd->tmp_buf);
		}
#endif
		memset(lcd, 0, sizeof(LCD_Handler));
#ifdef LCD_DYNAMIC_MEM
		free(lcd);
#endif
	}
}

//initializes the display
void LCD_Init(LCD_Handler* lcd)
{
	LCD_HardWareReset(lcd);
	LCD_String_Interpretator(lcd, lcd->Init_callback(lcd->Orientation));
	LCD_SetBackLight(lcd, lcd->bkl_data.bk_percent);
}

//returns backlight brightness, %
inline uint8_t LCD_GetBackLight(LCD_Handler* lcd)
{
	return lcd->bkl_data.bk_percent;
}

//returns display width, pixels
inline uint16_t LCD_GetWidth(LCD_Handler* lcd)
{
	return lcd->Width;
}

//returns display height, pixels
inline uint16_t LCD_GetHeight(LCD_Handler* lcd)
{
	return lcd->Height;
}

//returns display status: busy or free (required to send new data to the display)
//display is busy if the spi it is connected to is busy
inline LCD_State LCD_GetState(LCD_Handler* lcd)
{
	if (lcd->spi_data.spi->CR1 & SPI_CR1_SPE) {
		return LCD_STATE_BUSY;
	}
	return LCD_STATE_READY;
}

//backlight control
void LCD_SetBackLight(LCD_Handler* lcd, uint8_t bk_percent)
{
	if (bk_percent > 100) {
		bk_percent = 100;
	}
	lcd->bkl_data.bk_percent = bk_percent;
	//backlight using PWM
	if (lcd->bkl_data.htim_bk) {
		//calculate % brightness as part of the counter period
		uint32_t bk_value = lcd->bkl_data.htim_bk->ARR * bk_percent / 100;
		//set the PWM duty cycle of a specific channel
		switch(lcd->bkl_data.channel_htim_bk) {
			case LL_TIM_CHANNEL_CH1:
				lcd->bkl_data.htim_bk->CCR1 = bk_value;
				break;
			case LL_TIM_CHANNEL_CH2:
				lcd->bkl_data.htim_bk->CCR2 = bk_value;
				break;
			case LL_TIM_CHANNEL_CH3:
				lcd->bkl_data.htim_bk->CCR3 = bk_value;
				break;
			case LL_TIM_CHANNEL_CH4:
				lcd->bkl_data.htim_bk->CCR4 = bk_value;
				break;
			default:
				break;
		}
		//if the timer is not running, then start it
		if (!(lcd->bkl_data.htim_bk->CR1 & TIM_CR1_CEN)) {
			//turn on the channel
			lcd->bkl_data.htim_bk->CCER |= lcd->bkl_data.channel_htim_bk;
			//turn on the counter
			lcd->bkl_data.htim_bk->CR1 |= TIM_CR1_CEN;
		}
	}
	//backlight without PWM (just on/off), if the timer with PWM is not availableн
	else if (lcd->bkl_data.blk_port) {
		if (bk_percent) {
			lcd->bkl_data.blk_port->BSRR = lcd->bkl_data.blk_pin;
		}
		else {
			lcd->bkl_data.blk_port->BSRR = (uint32_t)lcd->bkl_data.blk_pin << 16U;
		}
	}
}

//putting the display into "sleep mode" (turning off the display and backlight)
void LCD_SleepIn(LCD_Handler* lcd)
{
	//backlight using PWM
	if (lcd->bkl_data.htim_bk) {
		//turn off the backlight by setting the duty cycle to zero
		switch(lcd->bkl_data.channel_htim_bk) {
			case LL_TIM_CHANNEL_CH1:
				lcd->bkl_data.htim_bk->CCR1 = 0;
				break;
			case LL_TIM_CHANNEL_CH2:
				lcd->bkl_data.htim_bk->CCR2 = 0;
				break;
			case LL_TIM_CHANNEL_CH3:
				lcd->bkl_data.htim_bk->CCR3 = 0;
				break;
			case LL_TIM_CHANNEL_CH4:
				lcd->bkl_data.htim_bk->CCR4 = 0;
				break;
			default:
				break;
		}
	}
	//backlight without PWM (just on/off) if timer with PWM is not available
	else if (lcd->bkl_data.blk_port) {
		lcd->bkl_data.blk_port->BSRR = (uint32_t)lcd->bkl_data.blk_pin << 16U;
	}
	if (lcd->SleepIn_callback) {
		LCD_String_Interpretator(lcd, lcd->SleepIn_callback());
	}
}

//waking up the display from sleep mode (turning on the display and backlight)
void LCD_SleepOut(LCD_Handler* lcd)
{
	if (lcd->SleepOut_callback) {
		LCD_String_Interpretator(lcd, lcd->SleepOut_callback());
	}
	//turning on the backlight
	LCD_SetBackLight(lcd, lcd->bkl_data.bk_percent);
}

#if LCD_USE_ASSEMBLER == 0
/*
void Display_TC_Callback(DMA_TypeDef *dma_x, uint32_t stream)
{
	*(&DMA2->LIFCR) = 0x3f << DMA_LISR_FEIF3_Pos; //DMA2 Stream3
	DMA2_Stream3->CR &= ~DMA_SxCR_EN;
	while (DMA2_Stream3->CR & DMA_SxCR_EN) ;
	if (LCD->size_mem) {
		if (LCD->size_mem > 65535) {
			DMA2_Stream3->NDTR = 65535;
			LCD->size_mem -= 65535;
		}
		else {
			DMA2_Stream3->NDTR = LCD->size_mem;
			LCD->size_mem = 0;
		}
		//включаем поток DMA
		DMA2_Stream3->CR |= (DMA_SxCR_EN);
		return;
	}
	SPI1->CR2 &= ~SPI_CR2_TXDMAEN;
	while (SPI1->SR & SPI_SR_BSY) ;
	if (!LCD->cs_control) {
		if (LCD->spi_data.cs_port) {
			LCD->spi_data.cs_port->BSRR = LCD->spi_data.cs_pin;
		}
	}
	SPI1->CR1 &= ~SPI_CR1_SPE;
	return;
}
*/

//callback to interrupt the transmission flow
//this handler must be registered in the interrupt handling functions in DMA streams,
//which are used by displays - stm32f4xx_it.c
void Display_TC_Callback(DMA_TypeDef *dma_x, uint32_t stream)
{
	//reset interrupt flags
	uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //битовое смещение во флаговом регистре IFCR (L и H)
	volatile uint32_t *ifcr_tx = (stream > 3) ? &(dma_x->HIFCR) : &(dma_x->LIFCR);
	*ifcr_tx = 0x3F << shift[stream];
	LCD_Handler *lcd = LCD; //указатель на первый дисплей в списке
	uint32_t stream_ct = 0;
	DMA_TypeDef *dma_ct = 0;
	//проходим по списку дисплеев (пока есть следующий в списке)
	while (lcd) {
		//получаем параметры DMA потока дисплея
		dma_ct = lcd->spi_data.dma_tx.dma;
		stream_ct = lcd->spi_data.dma_tx.stream;
		//проверка на соответствие текущего потока DMA потоку, к которому привязан i-тый дисплей
		if (dma_ct == dma_x && stream_ct == stream) {
			if (lcd->spi_data.cs_port) {//управление по cs поддерживается?
				//на выводе cs дисплея низкий уровень?
				if (lcd->spi_data.cs_port->ODR & lcd->spi_data.cs_pin) { //проверяем состояние пина выходного регистра порта
					lcd = (LCD_Handler *)lcd->next;		   //если высокий уровень cs, то не этот дисплей активен
					continue;							   //и переходим к следующему
				}
			}
			//указатель на поток: aдрес контроллера + смещение
			DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)dma_x + STREAM_OFFSET_TAB[stream])));
			//выключаем поток DMA
			dma_TX->CR &= ~DMA_SxCR_EN;
			while (dma_TX->CR & DMA_SxCR_EN) { __NOP(); } //ждем отключения потока
			if (lcd->size_mem) { //если переданы не все данные из памяти, то перезапускаем DMA и выходим из прерывания
				if (lcd->size_mem > 65535) {
					dma_TX->NDTR = 65535;
					lcd->size_mem -= 65535;
				}
				else {
					dma_TX->NDTR = lcd->size_mem;
					lcd->size_mem = 0;
				}
				//включаем поток DMA
				dma_TX->CR |= (DMA_SxCR_EN);
				return;
			}
#ifdef LCD_DYNAMIC_MEM
			//очищаем буфер дисплея
			if (lcd->tmp_buf) {
		    	//так как память выделяется динамически, то на всякий случай,
				//чтобы не было коллизий, запретим прерывания перед освобождением памяти
				DISABLE_IRQ
				free(lcd->tmp_buf);
				lcd->tmp_buf = 0;
				ENABLE_IRQ
			}
#endif
			//запрещаем SPI отправлять запросы к DMA
			lcd->spi_data.spi->CR2 &= ~SPI_CR2_TXDMAEN;
			while (lcd->spi_data.spi->SR & SPI_SR_BSY) { __NOP(); } //ждем пока SPI освободится
			//отключаем дисплей от MK (притягиваем вывод CS дисплея к высокому уровню)
			if (!lcd->cs_control) { LCD_CS_HI }
			//выключаем spi
			lcd->spi_data.spi->CR1 &= ~SPI_CR1_SPE;
			return;
		}
		//переходим к следующему дисплею в списке
		lcd = (LCD_Handler *)lcd->next;
	}
}

//установка на дисплее окна вывода
//------------------- 397 тактов ----------------------------
//если интерпретатор использовать, то 616 тактов
//ассемблерный вариант - 371 такт
//для подключения ассемблерного закомментировать функцию
void LCD_SetActiveWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	//этот вариант 616 тактов
	//LCD_String_Interpretator(lcd, lcd->SetActiveWindow_callback(x1 + lcd->x_offs, y1 + lcd->y_offs, x2 + lcd->x_offs, y2 + lcd->y_offs));

	x1 += lcd->x_offs;
	x2 += lcd->x_offs;
	y1 += lcd->y_offs;
	y2 += lcd->y_offs;
	SPI_TypeDef *spi = lcd->spi_data.spi;
	while (spi->CR1 & SPI_CR1_SPE) { __NOP(); }
	if (!lcd->cs_control) { LCD_CS_LOW }
	spi->CR1 &= ~ (SPI_CR1_BIDIMODE |  //здесь задаем режим
				    SPI_CR1_RXONLY   |  //  Transmit only
				    SPI_CR1_CRCEN    | 	//выключаем аппаратный расчет CRC
				    SPI_CR1_DFF); 		//установим 8-битную передачу
	spi->CR1 |= SPI_CR1_SPE; // SPI включаем
	LCD_DC_LOW
	*((volatile uint8_t *)&spi->DR) = 0x2a;
	while (!(spi->SR & SPI_SR_TXE)) ;
	while (spi->SR & SPI_SR_BSY) ;

	LCD_DC_HI
	*((volatile uint8_t *)&spi->DR) = x1 >> 8;
	while (!(spi->SR & SPI_SR_TXE)) ;
	*((volatile uint8_t *)&spi->DR) = x1 & 0xff;
	while (!(spi->SR & SPI_SR_TXE)) ;
	*((volatile uint8_t *)&spi->DR) = x2 >> 8;
	while (!(spi->SR & SPI_SR_TXE)) ;
	*((volatile uint8_t *)&spi->DR) = x2 & 0xff;
	while (!(spi->SR & SPI_SR_TXE)) ;
	while (spi->SR & SPI_SR_BSY) ;

	LCD_DC_LOW
	*((volatile uint8_t *)&spi->DR) = 0x2b;
	while (!(spi->SR & SPI_SR_TXE)) ;
	while (spi->SR & SPI_SR_BSY) ;

	LCD_DC_HI
	*((volatile uint8_t *)&spi->DR) = y1 >> 8;
	while (!(spi->SR & SPI_SR_TXE)) ;
	*((volatile uint8_t *)&spi->DR) = y1 & 0xff;
	while (!(spi->SR & SPI_SR_TXE)) ;
	*((volatile uint8_t *)&spi->DR) = y2 >> 8;
	while (!(spi->SR & SPI_SR_TXE)) ;
	*((volatile uint8_t *)&spi->DR) = y2 & 0xff;
	while (!(spi->SR & SPI_SR_TXE)) ;
	while (spi->SR & SPI_SR_BSY) ;

	LCD_DC_LOW
	*((volatile uint8_t *)&spi->DR) = 0x2c;
	while (!(spi->SR & SPI_SR_TXE)) ;
	while (spi->SR & SPI_SR_BSY) ;

	if (!lcd->cs_control) { LCD_CS_HI }
	spi->CR1 &= ~SPI_CR1_SPE;
}

//вывод блока данных на дисплей
void LCD_WriteData(LCD_Handler *lcd, uint16_t *data, uint32_t len)
{
	SPI_TypeDef *spi = lcd->spi_data.spi;
	while (LCD_GetState(lcd) == LCD_STATE_BUSY) { __NOP(); } //ждем когда дисплей освободится
	if (!lcd->cs_control) { LCD_CS_LOW }
	if (!lcd->dc_control) { LCD_DC_HI  }
	spi->CR1 &= ~ (SPI_CR1_BIDIMODE |  	//здесь задаем режим
				   SPI_CR1_RXONLY |   	//  Transmit only
				   SPI_CR1_CRCEN | 		//выключаем аппаратный расчет CRC
				   SPI_CR1_DFF); 		//8-битная передача
	if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
		spi->CR1 |= SPI_CR1_DFF; //16-битная передача
	}
	spi->CR1 |= SPI_CR1_SPE; //SPI включаем
	if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
		while (len--) {
			spi->DR = *data++; //записываем данные в регистр
			while (!(spi->SR & SPI_SR_TXE)) { __NOP(); } //ждем окончания передачи
		}
	}
	else {
		len *= 2;
		uint8_t *data1 = (uint8_t*)data;
		while (len--)	{
			*((volatile uint8_t *)&spi->DR) = *data1++; //записываем данные в регистр
			while (!(spi->SR & SPI_SR_TXE)) { __NOP(); } //ждем окончания передачи
		}
	}
	while (spi->SR & SPI_SR_BSY) { __NOP(); } //ждем когда SPI освободится
	if (!lcd->cs_control) { LCD_CS_HI }
	//выключаем spi
	spi->CR1 &= ~SPI_CR1_SPE;
}

//вывод блока данных на дисплей с DMA
void LCD_WriteDataDMA(LCD_Handler *lcd, uint16_t *data, uint32_t len)
{
	if (lcd->spi_data.dma_tx.dma) {
		if (lcd->data_bus == LCD_DATA_8BIT_BUS) {
			len *= 2;
		}
		SPI_TypeDef *spi = lcd->spi_data.spi;
		while (LCD_GetState(lcd) == LCD_STATE_BUSY) { __NOP(); } //ждем когда дисплей освободится
		if (!lcd->cs_control) { LCD_CS_LOW }
		if (!lcd->dc_control) { LCD_DC_HI  }
		lcd->size_mem = len;
		spi->CR1 &= ~ (SPI_CR1_BIDIMODE |  	//здесь задаем режим
					   SPI_CR1_RXONLY |   	//  Transmit - передача
					   SPI_CR1_CRCEN | 		//выключаем аппаратный расчет CRC
					   SPI_CR1_DFF); 		//8-битная передача
		if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
			spi->CR1 |= SPI_CR1_DFF; //16-битная передача
		}
		spi->CR1 |= SPI_CR1_SPE; //SPI включаем
		DMA_TypeDef *dma_x = lcd->spi_data.dma_tx.dma;
		uint32_t stream = lcd->spi_data.dma_tx.stream;
		DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)dma_x + STREAM_OFFSET_TAB[stream])));
		uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //битовое смещение во флаговых регистрах IFCR (L и H)
		volatile uint32_t *ifcr_tx = (stream > 3) ? &(dma_x->HIFCR) : &(dma_x->LIFCR);
		//сбрасываем флаги прерываний tx
		*ifcr_tx = 0x3F << shift[stream];
		//разрешаем spi отправлять запросы к DMA
		spi->CR2 |= SPI_CR2_TXDMAEN;
		//настраиваем адреса, длину, инкременты
		dma_TX->PAR = (uint32_t)(&spi->DR); //приемник периферия - адрес регистра DR spi
		dma_TX->M0AR = (uint32_t)data;		//источник память - адрес буфера исходящих данных
		dma_TX->CR &= ~DMA_SxCR_PINC;		//инкремент адреса периферии отключен
		dma_TX->CR |= DMA_SxCR_MINC;		//инкремент адреса памяти включен
		if (len <= 65535) {
			dma_TX->NDTR = (uint32_t)len;	//размер передаваемых данных
			lcd->size_mem = 0;
		}
		else {
			dma_TX->NDTR = 65535;
			lcd->size_mem = len - 65535;
		}
		dma_TX->CR |= (DMA_SxCR_EN);		//включение потока передачи (старт DMA передачи)
		return;
	}
	LCD_WriteData(lcd, data, len);
}
/*
void LCD_WriteDataDMA(LCD_Handler *lcd, uint16_t *data, uint32_t len)
{
  if (lcd->spi_data.dma_tx.dma) {
	while (SPI1->CR1 & SPI_CR1_SPE) { __NOP(); }
	if (!lcd->cs_control) { LCD_CS_LOW }
	if (!lcd->dc_control) { LCD_DC_HI  }
	SPI1->CR1 |= SPI_CR1_SPE; //SPI включаем
	SPI1->CR2 |= SPI_CR2_TXDMAEN;
	DMA2_Stream3->PAR = (uint32_t)(&SPI1->DR);
	DMA2_Stream3->M0AR = (uint32_t)data;
	DMA2_Stream3->CR &= ~DMA_SxCR_PINC;
	DMA2_Stream3->CR |= DMA_SxCR_MINC;
	if (len <= 65535) {
		DMA2_Stream3->NDTR = (uint32_t)len;
		lcd->size_mem = 0;
	}
	else {
		DMA2_Stream3->NDTR = 65535;
		lcd->size_mem = len - 65535;
	}
	DMA2_Stream3->CR |= (DMA_SxCR_EN);
	return;
  }
	LCD_WriteData(lcd, data, len);
}
*/

void LCD_FillWindow(LCD_Handler* lcd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color)
{
	uint16_t tmp;
	if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
	if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }
	if (x1 > lcd->Width - 1 || y1 > lcd->Height - 1) return;
	if (x2 > lcd->Width - 1)  x2 = lcd->Width - 1;
	if (y2 > lcd->Height - 1) y2 = lcd->Height - 1;
	uint32_t len = (x2 - x1 + 1) * (y2 - y1 + 1); //количество закрашиваемых пикселей
	LCD_SetActiveWindow(lcd, x1, y1, x2, y2);
	if (!lcd->cs_control) LCD_CS_LOW
	if (!lcd->dc_control) LCD_DC_HI
	uint16_t color16 = lcd->fill_color = LCD_Color_24b_to_16b(lcd, color);
	SPI_TypeDef *spi = lcd->spi_data.spi;
	spi->CR1 &= ~ (SPI_CR1_BIDIMODE |  	//здесь задаем режим
				   SPI_CR1_RXONLY |   	//  Transmit only
				   SPI_CR1_CRCEN | 		//выключаем аппаратный расчет CRC
				   SPI_CR1_DFF); 		//8-битная передача
	if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
		spi->CR1 |= SPI_CR1_DFF; //16-битная передача
	}
	spi->CR1 |= SPI_CR1_SPE; // SPI включаем
	if (lcd->spi_data.dma_tx.dma)
	{
		if (lcd->data_bus == LCD_DATA_8BIT_BUS)	{
			len *= 2;
		}
		DMA_TypeDef *dma_x = lcd->spi_data.dma_tx.dma;
		uint32_t stream = lcd->spi_data.dma_tx.stream;
		DMA_Stream_TypeDef *dma_TX = ((DMA_Stream_TypeDef *)((uint32_t)((uint32_t)dma_x + STREAM_OFFSET_TAB[stream])));
		uint8_t shift[8] = {0, 6, 16, 22, 0, 6, 16, 22}; //битовое смещение во флаговых регистрах IFCR (L и H)
		volatile uint32_t *ifcr_tx = (stream > 3) ? &(dma_x->HIFCR) : &(dma_x->LIFCR);
		//сбрасываем флаги прерываний tx
		*ifcr_tx = 0x3F<<shift[stream];
		//разрешаем spi отправлять запросы к DMA
		spi->CR2 |= SPI_CR2_TXDMAEN;
		//настраиваем адреса, длину, инкременты
		dma_TX->PAR = (uint32_t)(&spi->DR); //приемник периферия - адрес регистра DR spi
		dma_TX->M0AR = (uint32_t)&lcd->fill_color; //источник память - адрес буфера исходящих данных
		dma_TX->CR &= ~DMA_SxCR_PINC; //инкремент адреса периферии отключен
		dma_TX->CR &= ~DMA_SxCR_MINC; //инкремент адреса памяти отключен
		if (len <= 65535) {
			dma_TX->NDTR = (uint32_t)len; //размер передаваемых данных
			lcd->size_mem = 0;
		}
		else {
			dma_TX->NDTR = 65535;
			lcd->size_mem = len - 65535;
		}
		dma_TX->CR |= (DMA_SxCR_EN); //включение канала передачи (старт DMA передачи)
		return;
	}
	if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
		while(len--) {
			spi->DR = color16; //записываем данные в регистр
			while (!(spi->SR & SPI_SR_TXE)) { __NOP(); } //ждем окончания передачи
		}
	}
	else {
		uint8_t color1 = color16 & 0xFF, color2 = color16 >> 8;
		while(len--) {
			*((volatile uint8_t *)&spi->DR) = color1;
			while (!(spi->SR & SPI_SR_TXE)) { __NOP(); } //ждем окончания передачи
			*((volatile uint8_t *)&spi->DR) = color2;
			while (!(spi->SR & SPI_SR_TXE)) { __NOP(); } //ждем окончания передачи
			len--;
		}
	}
	while (spi->SR & SPI_SR_BSY) { __NOP(); } //ждем когда SPI освободится
	if (!lcd->cs_control) LCD_CS_HI
	//выключаем spi
	spi->CR1 &= ~SPI_CR1_SPE;
}

/*
 * Выводит в заданную область дисплея блок памяти (изображение) по адресу в data:
 * x, y - координата левого верхнего угла области дисплея;
 * w, h - ширина и высота области дисплея;
 * data - указатель на блок памяти (изображение) для вывода на дисплей;
 * dma_use_flag - флаг, определяющий задействование DMA (0 - без DMA, !=0 - с DMA)
 */
void LCD_DrawImage(LCD_Handler* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data, uint8_t dma_use_flag)
{
	if ((x > lcd->Width - 1) || (y > lcd->Height - 1) || x + w > lcd->Width || y + h > lcd->Height) return;
	LCD_SetActiveWindow(lcd, x, y, x + w - 1, y + h - 1);
	if (dma_use_flag) {
		LCD_WriteDataDMA(lcd, data, w * h);
	}
	else {
		LCD_WriteData(lcd, data, w * h);
	}
}

#endif

/* Paints the entire display with the specified color */
void LCD_Fill(LCD_Handler* lcd, uint32_t color)
{
	LCD_FillWindow(lcd, 0, 0, lcd->Width - 1, lcd->Height - 1, color);
}

/* Draws a point at the given coordinates */
void LCD_DrawPixel(LCD_Handler* lcd, int16_t x, int16_t y, uint32_t color)
{
	if (x > lcd->Width - 1 || y > lcd->Height - 1 || x < 0 || y < 0)	return;
	LCD_SetActiveWindow(lcd, x, y, x, y);
	uint16_t color1 = LCD_Color_24b_to_16b(lcd, color);
	LCD_WriteData(lcd, &color1, 1);
}

/*
  * Draws a line based on the coordinates of two points
  * Horizontal and vertical lines are drawn very quickly
  */
void LCD_DrawLine(LCD_Handler* lcd, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{
	if(x0 == x1 || y0 == y1) {
		int16_t tmp;
		if (x0 > x1) { tmp = x0; x0 = x1; x1 = tmp; }
		if (y0 > y1) { tmp = y0; y0 = y1; y1 = tmp; }
		if (x1 < 0 || x0 > lcd->Width - 1)  return;
		if (y1 < 0 || y0 > lcd->Height - 1) return;
		if (x0 < 0) x0 = 0;
		if (y0 < 0) y0 = 0;
		LCD_FillWindow(lcd, x0, y0, x1, y1, color);
		return;
	}
	int16_t swap;
    uint16_t steep = ABS(y1 - y0) > ABS(x1 - x0);
    if (steep) {
		swap = x0;
		x0 = y0;
		y0 = swap;

		swap = x1;
		x1 = y1;
		y1 = swap;
    }

    if (x0 > x1) {
		swap = x0;
		x0 = x1;
		x1 = swap;

		swap = y0;
		y0 = y1;
		y1 = swap;
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = ABS(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            LCD_DrawPixel(lcd, y0, x0, color);
        } else {
            LCD_DrawPixel(lcd, x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

/* Draws a rectangle at the coordinates of the upper left and lower right corners */
void LCD_DrawRectangle(LCD_Handler* lcd, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color)
{
	LCD_DrawLine(lcd, x1, y1, x2, y1, color);
	LCD_DrawLine(lcd, x1, y1, x1, y2, color);
	LCD_DrawLine(lcd, x1, y2, x2, y2, color);
	LCD_DrawLine(lcd, x2, y1, x2, y2, color);
}

/* Draws a filled rectangle at the coordinates of the upper left and lower right corners */
void LCD_DrawFilledRectangle(LCD_Handler* lcd, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint32_t color)
{
	int16_t tmp;
	if (x1 > x2) { tmp = x1; x1 = x2; x2 = tmp; }
	if (y1 > y2) { tmp = y1; y1 = y2; y2 = tmp; }
	if (x2 < 0 || x1 > lcd->Width - 1)  return;
	if (y2 < 0 || y1 > lcd->Height - 1) return;
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	LCD_FillWindow(lcd, x1, y1, x2, y2, color);
}

/* Draws a triangle using the coordinates of three points */
void LCD_DrawTriangle(LCD_Handler* lcd, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint32_t color)
{
	LCD_DrawLine(lcd, x1, y1, x2, y2, color);
	LCD_DrawLine(lcd, x2, y2, x3, y3, color);
	LCD_DrawLine(lcd, x3, y3, x1, y1, color);
}

/* Types of intersections of segments */
typedef enum {
	LINES_NO_INTERSECT = 0, //do not intersect  لا تتقاطع
	LINES_INTERSECT,		//intersect
	LINES_MATCH				//coincide (overlap)
} INTERSECTION_TYPES;

/*
  * Determination of the type of intersection and coordinates (along the x axis) of the intersection of a segment with coordinates (x1,y1)-(x2,y2)
  * with horizontal straight line y = y0
  * Returns one of the types of intersection of type INTERSECTION_TYPES, and in the variables x_min, x_max - the coordinate
  * or the range of intersection (if they overlap).
  * In match, increments the number of overlaps (we count the results from all the necessary calls)
  */
static INTERSECTION_TYPES LinesIntersection(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t y0, int16_t *x_min, int16_t *x_max, uint8_t *match)
{
	if (y1 == y2) { //A special case - the segment is parallel to the x axis
		if (y0 == y1) { //Match check
			*x_min = min(x1, x2);
			*x_max = max(x1, x2);
			(*match)++;
			return LINES_MATCH;
		}
		return LINES_NO_INTERSECT;
	}
	if (x1 == x2) { //A special case - the segment is parallel to the y axis
		if (min(y1, y2) <= y0 && y0 <= max(y1, y2)) {
			*x_min = *x_max = x1;
			return LINES_INTERSECT;
		}
		return LINES_NO_INTERSECT;
	}
	//Determine the point of intersection of the lines (we obtain the equation of the line from the coordinates of the points defining the segment)
	*x_min = *x_max = (x2 - x1) * (y0 - y1) / (y2 - y1) + x1;
	if (min(x1, x2) <= *x_min && *x_min <= max(x1, x2)) { //If the x-coordinate of the intersection point belongs to the segment,
		return LINES_INTERSECT;							  //that is, the intersection
	}
	return LINES_NO_INTERSECT;
}

/* Draws a filled triangle using the coordinates of three points */
void LCD_DrawFilledTriangle(LCD_Handler* lcd, int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint32_t color)
{
	//Sort the coordinates in ascending order y
	int16_t tmp;
	if (y1 > y2) {
		tmp = y1; y1 = y2; y2 = tmp;
		tmp = x1; x1 = x2; x2 = tmp;
	}
	if (y1 > y3) {
		tmp = y1; y1 = y3; y3 = tmp;
		tmp = x1; x1 = x3; x3 = tmp;
	}
	if (y2 > y3) {
		tmp = y2; y2 = y3; y3 = tmp;
		tmp = x2; x2 = x3; x3 = tmp;
	}
	//Checking whether the triangle falls within the output area
	if (y1 > lcd->Height - 1 ||	y3 < 0) return;
	int16_t xmin = min3(x1, x2, x3);
	int16_t xmax = max3(x1, x2, x3);
	if (xmax < 0 || xmin > lcd->Width - 1) return;
	uint8_t c_mas, match;
	int16_t x_mas[8], x_min, x_max;
	//“Cut off” coordinates that extend beyond the working area of the display
	int16_t y_start = y1 < 0 ? 0: y1;
	int16_t y_end = y3 > lcd->Height - 1 ? lcd->Height - 1: y3;
	//We go through the points of the y coordinate range in a loop and look for the intersection of the segment y = y[i] (где y[i]=y1...y3, 1)
	//with the sides of a triangle
	for (int16_t y = y_start; y < y_end; y++) {
		c_mas = match = 0;
		if (LinesIntersection(x1, y1, x2, y2, y, &x_mas[c_mas], &x_mas[c_mas + 1], &match)) {
			c_mas += 2;
		}
		if (LinesIntersection(x2, y2, x3, y3, y, &x_mas[c_mas], &x_mas[c_mas + 1], &match)) {
			c_mas += 2;
		}
		if (LinesIntersection(x3, y3, x1, y1, y, &x_mas[c_mas], &x_mas[c_mas + 1], &match)) {
			c_mas += 2;
		}
		if (!c_mas) continue;
		x_min = x_max = x_mas[0];
		while (c_mas) {
			x_min = min(x_min, x_mas[c_mas - 2]);
			x_max = max(x_max, x_mas[c_mas - 1]);
			c_mas -= 2;
		}
		LCD_DrawLine(lcd, x_min, y, x_max, y, color);
	}
}

/* Draws a circle with a given center and radius */
void LCD_DrawCircle(LCD_Handler* lcd, int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	LCD_DrawPixel(lcd, x0, y0 + r, color);
	LCD_DrawPixel(lcd, x0, y0 - r, color);
	LCD_DrawPixel(lcd, x0 + r, y0, color);
	LCD_DrawPixel(lcd, x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		LCD_DrawPixel(lcd, x0 + x, y0 + y, color);
		LCD_DrawPixel(lcd, x0 - x, y0 + y, color);
		LCD_DrawPixel(lcd, x0 + x, y0 - y, color);
		LCD_DrawPixel(lcd, x0 - x, y0 - y, color);

		LCD_DrawPixel(lcd, x0 + y, y0 + x, color);
		LCD_DrawPixel(lcd, x0 - y, y0 + x, color);
		LCD_DrawPixel(lcd, x0 + y, y0 - x, color);
		LCD_DrawPixel(lcd, x0 - y, y0 - x, color);
	}
}

/* Draws a filled circle with the given center and radius */
void LCD_DrawFilledCircle(LCD_Handler* lcd, int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	LCD_DrawLine(lcd, x0 - r, y0, x0 + r, y0, color);

	while (x < y) {
		if (f >= 0)	{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		LCD_DrawLine(lcd, x0 - x, y0 + y, x0 + x, y0 + y, color);
		LCD_DrawLine(lcd, x0 + x, y0 - y, x0 - x, y0 - y, color);

		LCD_DrawLine(lcd, x0 + y, y0 + x, x0 - y, y0 + x, color);
		LCD_DrawLine(lcd, x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
}

/* Reads pixel color data from the display window with coordinates of the upper left corner (x, y), width w, height h into the data buffer.
* 2 modes of operation with spi are available: full duplex (full-duplex) and half-duplex (half-duplex).
* Full duplex is used on displays connected to the MCU via two unidirectional data lines: MOSI and MISO.
*Half duplex is used to read data from display controllers containing only one SDA pin sharing the lines
* in/out. CS output in half-duplex mode is STRICTLY MANDATORY, because only after setting this signal to a high level
* The display controller will exit MCU data transfer mode and will be ready to receive new commands from the MCU. With absence
* CS output return to the mode of receiving commands by the display controller can only be done after a reset
* display controller, which in this driver/library is executed by the initialization command LCD_Init(lcd),
* where lcd is a pointer to the display handler.
* The operation mode of the procedure is controlled by the SPI_HALF_DUPLEX_READ parameter in the display.h file.
* To enable half duplex:
* #define SPI_HALF_DUPLEX_READ 1
* To enable full duplex:
* #define SPI_HALF_DUPLEX_READ 0
* The reading speed is usually lower (sometimes significantly) than the data writing speed to the controller, so a parameter has been introduced
* defining the reading speed from the display controller SPI_SPEED_DISPLAY_READ. Accepts a value from 0 to 7 and is configurable
* in display.h. Moreover, 0 corresponds to clk/2, 1 - clk/4, ... 7 - clk/256. Where clk is the spi clock frequency.
*/
void LCD_ReadImage(LCD_Handler* lcd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data)
{
	uint16_t x1 = x + w - 1, y1 = y + h - 1; //Determine the coordinates of the lower right corner of the window
	//We check the window parameters, because We don’t work outside the display size
	if (x >= lcd->Width || y >= lcd->Height || x1 >= lcd->Width || y1 >= lcd->Height) return;
	//Recalculation of window coordinates depending on the characteristics of the display matrix and controller.
	//The corresponding offs in the display driver determines these characteristics, taking into account the difference in dimensions,
	//the initial offset of the display matrix relative to the memory field of the display controller and the orientation of the image on the matrix
	x += lcd->x_offs;
	y += lcd->y_offs;
	x1 += lcd->x_offs;
	y1 += lcd->y_offs;
	//We create a control string for the display driver interpreter, which will tell the display controller to
	//that we define a memory area and want to read this area. Let's first set the display to
	//18-bit color mode because the Memory Read (0x2E) command should only work with 18-bit color mode.
	uint8_t	set_win_and_read[] = { //Color mode selection command
								   LCD_UPR_COMMAND, 0x3A, 1, 0x66, //0x66 - 18-bit color, 0x55 - 16-bit
								   //Setting the addresses defining the block:
								   LCD_UPR_COMMAND, 0x2A, 4, 0, 0, 0, 0, //column
								   LCD_UPR_COMMAND, 0x2B, 4, 0, 0, 0, 0, //line
								   //Command Memory Read (read memory)
								   LCD_UPR_COMMAND, 0x2E, 0,
								   LCD_UPR_END	};
	//Enter the coordinates of the specified window into the control line
	set_win_and_read[7]  = x >> 8;  set_win_and_read[8]  = x & 0xFF;
	set_win_and_read[9]  = x1 >> 8; set_win_and_read[10]  = x1 & 0xFF;
	set_win_and_read[14] = y >> 8;  set_win_and_read[15] = y & 0xFF;
	set_win_and_read[16] = y1 >> 8; set_win_and_read[17] = y1 & 0xFF;
	//We are waiting for the display to become free and ready to receive new commands and data
	while (LCD_GetState(lcd) != LCD_STATE_READY) { __NOP(); }
	lcd->cs_control = 1; //Disable control of the CS line from the control string interpreter
	SPI_TypeDef *spi = lcd->spi_data.spi;
	uint32_t spi_param = spi->CR1; //Remember spi parameters
	//Configure spi (spi is already turned off)
	//full duplex settings
	spi->CR1 &= ~(SPI_CR1_BIDIMODE |  //2 lines, unidirectional mode
				  SPI_CR1_RXONLY   |  //reception and transmission
				  SPI_CR1_CRCEN    |
				  SPI_CR1_BR_Msk   |  //Speed mask spi
				  SPI_CR1_DFF); 	  //Frame width 8 bits
	//Set spi speed for display reading.
	//The SPI_SPEED_DISPLAY_READ parameter is configured in display.h
	//The fact is that according to the specifications for display controllers, the speed
	//in read mode from the controller, as a rule, the speed in data writing mode is lower
	//to the controller.
	spi->CR1 |= (uint32_t)((SPI_SPEED_DISPLAY_READ & 7) << SPI_CR1_BR_Pos);
	//Send a control string to the display controller through the interpreter
	//You can't "twitch" the CS output after sending the 0x2E command, because controller
	//the display may “think” that we want to interrupt reading.
	LCD_CS_LOW //Connecting the display controller to the MK
	LCD_String_Interpretator(lcd, set_win_and_read);
	LCD_DC_HI //Set the DC output of the display controller to the “data” position. But my experiments show
	//that reading also works in the “command” position, which is strange, because according to the specification, the first command, incl.
	//NOP should interrupt the controller memory read operation. In general, reading goes on until we read
	//the entire area we have selected, or until we stupidly interrupt the reading process.
	uint32_t len = w * h; //Number of pixels to read
	uint16_t *data_ptr = data; //Pointer to the location of the buffer for storing pixels
	uint8_t r, g, b; //Variables with color components
#if (SPI_HALF_DUPLEX_READ == 1) //If half duplex is used, then adjust the spi settings
	//Settings for half duplex (receive only):
	spi->CR1 |= SPI_CR1_BIDIMODE; //Bidirectional data line
	spi->CR1 &= ~SPI_CR1_BIDIOE;  //Reception mode
	spi->CR1 ^= SPI_CR1_CPHA_Msk; //According to the specification, when receiving, we change the phase to the opposite one (for half-duplex)
#endif
	//Turn on spi. For half-duplex mode, reception starts immediately as soon as we turn on spi (clocks will come from the MCU)
	spi->CR1 |= SPI_CR1_SPE;
	//16 idle clocks to prepare the display controller to send data to the MCU
	int i = 2;
	while (i--) {
#if (SPI_HALF_DUPLEX_READ == 0) //В In half-duplex, when receiving, there is no need to fill data into DR to start clocking
		*((volatile uint8_t *)&spi->DR) = 0x00; //NOP
#endif
		while (!(spi->SR & SPI_SR_RXNE)) { __NOP(); } //Waiting to receive a response from the display controller
		r = LL_SPI_ReceiveData8(spi);
	}
	//------------------------------ Reading color data for len pixels --------------------------
	while (len--) {
		//Read the color components sequentially
		//According to the specification, the sequence of read component colors is stated as r, g, b,
		//If the read colors do not match the actual ones, then reduce the spi speed for reading,
		//but sometimes reading stability is helped by connecting the MISO spi line to the power supply.
#if (SPI_HALF_DUPLEX_READ == 0)
		*((volatile uint8_t *)&spi->DR) = 0x00; //NOP
#endif
		while (!(spi->SR & SPI_SR_RXNE)) { __NOP(); }//We are waiting for a response from the display controller
		r = *((volatile uint8_t *)&spi->DR);
#if (SPI_HALF_DUPLEX_READ == 0)
		*((volatile uint8_t *)&spi->DR) = 0x00; //NOP
#endif
		while (!(spi->SR & SPI_SR_RXNE)) { __NOP(); }
		g = *((volatile uint8_t *)&spi->DR);
#if (SPI_HALF_DUPLEX_READ == 0)
		*((volatile uint8_t *)&spi->DR) = 0x00; //NOP
#endif
		while (!(spi->SR & SPI_SR_RXNE)) { __NOP(); }
		b = *((volatile uint8_t *)&spi->DR);
		*data_ptr++ = LCD_Color(lcd, r, g, b); //Convert the color from R8G8B8 to R5G6B5 and remember it
	}
	LCD_CS_HI //Disconnect the display controller from the MK.
	//In half-duplex mode, according to the specification, this will also be a signal for
	//switching the direction of the SDA line to receive information from the MCU. So, without
	//CS lines on the display are NOT BYPASSED.
#if (SPI_HALF_DUPLEX_READ == 0)
	while (spi->SR & SPI_SR_BSY) { __NOP(); } //We are waiting for spi to become free
											  //And in half-duplex there is no need to wait (see MCU specification),
											  //but you have to finish reading...
#endif
	spi->CR1 &= ~SPI_CR1_SPE; //spi turn off
#if (SPI_HALF_DUPLEX_READ == 1)
	//Mandatory reading after turning off spi for half-duplex mode, otherwise there will be “no buzz”
	while (!(spi->SR & SPI_SR_RXNE)) { __NOP(); }
#endif
	spi->CR1 = spi_param; //Restoring parameters spi
	//Restoring 16-bit color mode
	lcd->cs_control = 0; //Enable control of the CS line from the control string interpreter
	uint8_t	color_restore[]  = { LCD_UPR_COMMAND, 0x3A, 1, 0x55,
								 LCD_UPR_END };
	LCD_String_Interpretator(lcd, color_restore);
}

/*
  * Display a symbol with a code in ch, with initial coordinates (x, y), font font, color color,
  * environment color bgcolor.
  * modesym - defines how to output the symbol:
  * LCD_SYMBOL_PRINT_FAST - fast output with complete overwriting of familiarity;
  * LCD_SYMBOL_PRINT_PSETBYPSET - display the symbol by pixels, while the surrounding color bgcolor is ignored (overlay mode).
  * Character width up to 32 pixels (4 bytes per line). The height of the symbol is not limited by the library.
  */
void LCD_WriteChar(LCD_Handler* lcd, uint16_t x, uint16_t y, char ch, FontDef *font, uint32_t txcolor, uint32_t bgcolor, LCD_PrintSymbolMode modesym)
{
	int i, j, k;
	uint32_t tmp = 0;
	const uint8_t *b = font->data;
	uint16_t color;
	uint16_t txcolor16 = LCD_Color_24b_to_16b(lcd, txcolor);
	uint16_t bgcolor16 = LCD_Color_24b_to_16b(lcd, bgcolor);
	ch = ch < font->firstcode || ch > font->lastcode ? 0: ch - font->firstcode;
	int bytes_per_line = ((font->width - 1) >> 3) + 1;
	if (bytes_per_line > 4) { //Support character widths up to 32 pixels (4 bytes per line)
		return;
	}
	k = 1 << ((bytes_per_line << 3) - 1);
	b += ch * bytes_per_line * font->height;
	SPI_TypeDef *spi = lcd->spi_data.spi;
	if (modesym == LCD_SYMBOL_PRINT_FAST) {
		LCD_SetActiveWindow(lcd, x, y, x + font->width - 1, y + font->height - 1);
		LCD_CS_LOW
		LCD_DC_HI
		spi->CR1 &= ~SPI_CR1_SPE; // SPI turn it off to change settings
		spi->CR1 &= ~ (SPI_CR1_BIDIMODE |  	//here we set the mode
					   SPI_CR1_RXONLY |   	//  Transmit only
					   SPI_CR1_CRCEN | 		//turn off hardware calculation CRC
					   SPI_CR1_DFF); 		//8-bit transfer
		if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
			spi->CR1 |= SPI_CR1_DFF; //16-bit transfer
		}
		spi->CR1 |= SPI_CR1_SPE; // SPI включаем
		for (i = 0; i < font->height; i++) {
			if (bytes_per_line == 1)      { tmp = *((uint8_t*)b);  }
			else if (bytes_per_line == 2) { tmp = *((uint16_t*)b); }
			else if (bytes_per_line == 3) { tmp = (*((uint8_t*)b)) | ((*((uint8_t*)(b + 1))) << 8) |  ((*((uint8_t*)(b + 2))) << 16); }
			else { tmp = *((uint32_t*)b); }
			b += bytes_per_line;
			for (j = 0; j < font->width; j++)
			{
				color = (tmp << j) & k ? txcolor16: bgcolor16;
				while (!(spi->SR & SPI_SR_TXE)) { __NOP(); } //waiting for the end of the transmission
				if (lcd->data_bus == LCD_DATA_16BIT_BUS) {
					spi->DR = color;
				}
				else {
					uint8_t color1 = color & 0xFF, color2 = color >> 8;
					*((volatile uint8_t *)&spi->DR) = color1;
					while (!(spi->SR & SPI_SR_TXE)) { __NOP(); } //waiting for the end of the transmission
					*((volatile uint8_t *)&spi->DR) = color2;
				}
			}
		}
		while (!(spi->SR & SPI_SR_TXE)) { __NOP(); } //waiting for the end of the transmission
		while (spi->SR & SPI_SR_BSY) { __NOP(); } //waiting for the SPI to become free
		//turn off turn off spi
		spi->CR1 &= ~SPI_CR1_SPE;
		LCD_CS_HI
	}
	else {
		for (i = 0; i < font->height; i++) {
			if (bytes_per_line == 1) { tmp = *((uint8_t*)b); }
			else if (bytes_per_line == 2) { tmp = *((uint16_t*)b); }
			else if (bytes_per_line == 3) { tmp = (*((uint8_t*)b)) | ((*((uint8_t*)(b + 1))) << 8) |  ((*((uint8_t*)(b + 2))) << 16); }
			else if (bytes_per_line == 4) { tmp = *((uint32_t*)b); }
			b += bytes_per_line;
			for (j = 0; j < font->width; j++) {
				if ((tmp << j) & k) {
					LCD_DrawPixel(lcd, x + j, y + i, txcolor);
				}
			}
		}
	}
}

//output string str of text from position x, y, font font, letter color color, environment color bgcolor
//modesym - determines how to display text:
//LCD_SYMBOL_PRINT_FAST - fast output with complete overwriting of familiarity
//LCD_SYMBOL_PRINT_PSETBYPSET - output by pixels, while the surrounding color bgcolor is ignored (allows you to overlay labels on pictures)
void LCD_WriteString(LCD_Handler* lcd, uint16_t x, uint16_t y, const char *str, FontDef *font, uint32_t color, uint32_t bgcolor, LCD_PrintSymbolMode modesym)
{
	while (*str) {
		if (x + font->width > lcd->Width) {
			x = 0;
			y += font->height;
			if (y + font->height > lcd->Height) {
				break;
			}
		}
		LCD_WriteChar(lcd, x, y, *str, font, color, bgcolor, modesym);
		x += font->width;
		str++;
	}
	lcd->AtPos.x = x;
	lcd->AtPos.y = y;
}

inline uint16_t LCD_Color (LCD_Handler *lcd, uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t color = ((r & 0xF8) << 8) | ((g & 0xF8) << 3) | (b >> 3);
	if (lcd->data_bus == LCD_DATA_8BIT_BUS) {//8-bit transfer
		color = (color >> 8) | ((color & 0xFF) << 8);
	}
	return color;
}

inline uint16_t LCD_Color_24b_to_16b(LCD_Handler *lcd, uint32_t color)
{
	uint8_t r = (color >> 16) & 0xff;
	uint8_t g = (color >> 8) & 0xff;
	uint8_t b = color & 0xff;
	return LCD_Color(lcd, r, g, b);
}
