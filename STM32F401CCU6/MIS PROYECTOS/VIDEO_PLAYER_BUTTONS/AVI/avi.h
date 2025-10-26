/*
 *  Copyright (C) 2019-2023, VadRov, all right reserved.
 *
*AVI playback
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */
#ifndef AVI_H_
#define AVI_H_

#include "display.h"

//----------------------Audio DAC Configuration (User Settings)------------------------
#define AudioDAC_I2S		SPI3				//spi, on the basis of which i2s is used for dac
#define AudioDAC_DMA		DMA1				//dma controller serving dac
#define AudioDAC_DMA_Stream	LL_DMA_STREAM_5		//dma controller thread serving dac
#define AudioDAC_IRQn		DMA1_Stream5_IRQn	//interruption in the DMA thread serving dac
//----------------------------------------------------------------------------------------------

void PlayAVI(char *dir, char *fname, LCD_Handler *lcd, uint16_t x, uint16_t y, uint16_t win_wdt, uint16_t win_hgt);
void AVIIntro(LCD_Handler *lcd);

#endif /* AVI_H_ */
